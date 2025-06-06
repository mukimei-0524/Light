#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraite.h"
#include "ProjectileHoming.h"

static Player* instance = nullptr;

// インスタンス取得
Player& Player::Instance()
{
	return *instance;
}



// コンストラクタ
Player::Player()
{
	// インスタンスポインタ設定
	instance = this;

	model = new Model("Data/Model/Jammo/Model.mdl");
	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.1f;

	// ヒットエフェクト読み込み
	hitEffect = new Effect("Data/Effect/Hit.efk");
	model->PlayAnimation(Animation::Anim_Walking, true);

}

// デストラクタ
Player::~Player()
{
	delete hitEffect;

	delete model;
}

// 更新処理
void Player::Update(float elapsedTime)
{
	// 移動入力処理
	InputMove(elapsedTime);

	// ジャンプ入力処理
	InputJump();

	// 弾丸入力処理
	InputProjectile();

	UpdateProjectile();

	// 速力更新処理
	UpdateVelocity(elapsedTime);

	// 弾丸更新処理
	projectileManager.Update(elapsedTime);

	// プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	// 弾丸と敵との衝突処理
	CollisionProjectilesVsEnemies();

	// オブジェクト行列を更新
	UpdateTransform();
	
	// 行列より前方取得
	direction.x = transform._31;
	direction.y = transform._32;
	direction.z = transform._33;

	// ベクトルの正規化
	float len = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	if (len > 0) {
		direction.x /= len;
		direction.y /= len;
		direction.z /= len;
	}

	// モデルアニメーション更新処理(10/23追加)
	model->UpdateAnimation(elapsedTime);



	// モデル行列更新
	model->UpdateTransform(transform);
}



// 描画処理
void Player::Render(const RenderContext& rc, ModelShader* shader)
{
	shader->Draw(rc, model);

	// 弾丸描画処理
	projectileManager.Render(rc, shader);
}

// デバッグ用GUI描画
void Player::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// デバッグプリミティブ表示
		ImGui::Checkbox("VisibleDebugPrimitive", &visibleDebugPrimitive);
		// 位置
		ImGui::InputFloat3("Position", &position.x);
		// 回転
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// スケール
		ImGui::InputFloat3("Scale", &scale.x);
		// 移動速度
		ImGui::InputFloat("MoveSpeed", &moveSpeed);
		// 旋回速度
		float turnDeg = DirectX::XMConvertToDegrees(turnSpeed);
		ImGui::InputFloat("TurnSpeed", &turnDeg);
		turnSpeed = DirectX::XMConvertToRadians(turnDeg);
		// 半径
		ImGui::InputFloat("Radius", &radius);
		// 高さ
		ImGui::InputFloat("Height", &height);
		// 摩擦
		ImGui::InputFloat("Friction", &friction);
		// 加速度
		ImGui::InputFloat("Acceleration", &acceleration);
		// 空中制御係数
		ImGui::InputFloat("AirControl", &airControl);
	}
}

// デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
	if (!visibleDebugPrimitive) return;

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// 弾丸デバッグプリミティブ描画
	projectileManager.DrawDebugPrimitive();
}

// 着地した時に呼ばれる
void Player::OnLanding()
{
	jumpCount = 0;

}

// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	// 入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// 移動ベクトルはXZ平面に水平なベクトルなるようにする

	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		// 単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// スティックの水平入力値をカメラ右方向に反映し、
	// スティックの垂直入力値をカメラ前方向に反映し、
	// 進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

#if 0
DirectX::XMFLOAT3 convert_quaternion_to_euler(DirectX::XMFLOAT4X4 rotation)
{
	//	ZXY回転
	DirectX::XMFLOAT3 euler;
	if (1.0f - fabs(rotation.m[2][1]) < 1.0e-6f)
	{
		euler.x = rotation.m[2][1] < 0 ? DirectX::XM_PIDIV2 : -DirectX::XM_PIDIV2;
		euler.y = atan2f(rotation.m[1][0], rotation.m[0][0]);
		euler.z = 0;
	}
	else
	{
		euler.x = asinf(-rotation.m[2][1]);
		euler.y = atan2f(rotation.m[2][0], rotation.m[2][2]);
		euler.z = atan2f(rotation.m[0][1], rotation.m[1][1]);
	}
	return euler;
}

DirectX::XMFLOAT3 convert_quaternion_to_euler(DirectX::XMFLOAT4 quaternion)
{
	DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
	DirectX::XMFLOAT4X4 rotation;
	DirectX::XMStoreFloat4x4(&rotation, Rotation);
	return convert_quaternion_to_euler(rotation);
}
{
	// 視線行列＞カメラのワールド行列に変換する
	DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	mat = DirectX::XMMatrixInverse(nullptr, mat);
	// カメラのワールド行列からスケール、回転、平行移動を取得
	DirectX::XMVECTOR S, R, T;
	DirectX::XMMatrixDecompose(&S, &R, &T, mat);
	// 必要なのは回転情報だけなので、スケールと平行移動は無視する
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMStoreFloat4(&quaternion, R);
	// 得られる回転情報はクォータニオンなので、オイラー角に変換する
	DirectX::XMFLOAT3 euler_rotation = convert_quaternion_to_euler(quaternion);
}
#endif


// 移動入力処理
void Player::InputMove(float elapsedTime)
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();	
	// Animation_change
	float fspeed = sqrtf(moveVec.x * moveVec.x + moveVec.z * moveVec.z);
	if (isGround) {
		if (fspeed > 0.6f) {
			if (model->IsPlayAnimation() != Animation::Anim_Walking)
			{
				model->PlayAnimation(Animation::Anim_Walking, true);
			}
		}

		else
		{
			if (model->IsPlayAnimation() != Animation::Anim_Idle)
			{
				model->PlayAnimation(Animation::Anim_Idle, true);
			}
		}
	}

	// 移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);

	Mouse& mouse = Input::Instance().GetMouse();
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	Graphics::Instance().GetDeviceContext()->RSGetViewports(&numViewports, &viewport);
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//マウスカーソル座標を取得
	DirectX::XMFLOAT3 screenPosition{};
	screenPosition.x = static_cast<float>(mouse.GetPositionX());
	screenPosition.y = static_cast<float>(mouse.GetPositionY());
	screenPosition.z = 0.0f;//ビューポートの深度の0.0を設定
	DirectX::XMVECTOR ScreenPosition, WorldPosition;
	ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

	//始点を２Dから３D に変換
	WorldPosition = DirectX::XMVector3Unproject(
		ScreenPosition,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		viewport.MinDepth, viewport.MaxDepth,
		Projection, View, World
	);

	// 旋回処理
	moveVec = Camera::Instance().GetFront();
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

// ジャンプ入力処理
void Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// ジャンプ回数制限
		//if (jumpCount < jumpLimit)
		{
			// ジャンプ
			jumpCount++;
			Jump(jumpSpeed);

			// ジャンプアニメーション
			model->PlayAnimation(Animation::Anim_Jump, false);


		}
	}
}

// 弾丸入力処理
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float mousePosX = Input::Instance().GetMouse().GetPositionX();
	float mousePosY = Input::Instance().GetMouse().GetPositionY();

	// 直進弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = Camera::Instance().GetFront().x;
		dir.y = 0.0f;
		dir.z = Camera::Instance().GetFront().z;
		// 発射位置（プレイヤーの腰あたり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.05f;
		pos.z = position.z;
		// 発射
		ProjectileStraite* projectile = new ProjectileStraite(&projectileManager);
		projectile->Launch(dir, pos);
	}

	// 追尾弾丸発射
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		// 発射位置（プレイヤーの腰あたり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y /*+ height * 0.05f*/;
		pos.z = position.z;

		// ターゲット（デフォルトではプレイヤーの前方）
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f + height * 0.5f;
		target.z = pos.z + dir.z * 1000.0f;

		// 一番近くの敵をターゲットにする
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			//// 敵との距離判定
			//Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			//DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			//DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			//DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			//DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			//float d;
			//DirectX::XMStoreFloat(&d, D);
			//if (d < dist)
			//{
			//	dist = d;
			//	target = enemy->GetPosition();
			//	target.y += enemy->GetHeight() * 0.5f;
			//}


			//target = this->GetPosition();
			//target.y += this->GetHeight() * 2.0f;

		}
		// 発射
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);
	}
}

void Player::UpdateProjectile()
{
	// 猫のHoming用にプレイヤーの位置設定
	if (projectileManager.GetProjectileCount() == 0) return;
	Projectile* projectile = projectileManager.GetProjectile(0);
	if (projectile != NULL) {
		projectile->setTarget(position);
	}
}


// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectCylinderVsCylinder(
			position,
			radius,
			height,
			enemy->GetPosition(),
			enemy->GetRadius(),
			enemy->GetHeight(),
			outPosition))
		{
			// 敵の真上付近に当たったかを判定
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			// 上から踏んづけた場合は小ジャンプする
			if (normal.y > 0.8f)
			{
				// 小ジャンプする
				Jump(jumpSpeed * 0.5f);
				model->PlayAnimation(Animation::Anim_Jump,false);

				// ダメージを与える
				enemy->ApplyDamage(1, 0.5f);
			}
			else
			{
				// 押し出し後の位置設定
				enemy->SetPosition(outPosition);
			}
		}
	}
}

// 弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				// ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f))
				{
					// 吹き飛ばす
					{
						const float power = 10.0f;
						const DirectX::XMFLOAT3& e = enemy->GetPosition();
						const DirectX::XMFLOAT3& p = projectile->GetPosition();
						float vx = e.x - p.x;
						float vz = e.z - p.z;
						float lengthXZ = sqrtf(vx * vx + vz * vz);
						vx /= lengthXZ;
						vz /= lengthXZ;

						DirectX::XMFLOAT3 impulse;
						impulse.x = vx * power;
						impulse.y = power * 0.5f;
						impulse.z = vz * power;

						enemy->AddImpulse(impulse);
					}

					// ヒットエフェクト再生
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					// 弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}
