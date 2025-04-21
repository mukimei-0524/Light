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

// �C���X�^���X�擾
Player& Player::Instance()
{
	return *instance;
}



// �R���X�g���N�^
Player::Player()
{
	// �C���X�^���X�|�C���^�ݒ�
	instance = this;

	model = new Model("Data/Model/Jammo/Model.mdl");
	//model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

	// ���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 0.1f;

	// �q�b�g�G�t�F�N�g�ǂݍ���
	hitEffect = new Effect("Data/Effect/Hit.efk");
	model->PlayAnimation(Animation::Anim_Walking, true);

}

// �f�X�g���N�^
Player::~Player()
{
	delete hitEffect;

	delete model;
}

// �X�V����
void Player::Update(float elapsedTime)
{
	// �ړ����͏���
	InputMove(elapsedTime);

	// �W�����v���͏���
	InputJump();

	// �e�ۓ��͏���
	InputProjectile();

	UpdateProjectile();

	// ���͍X�V����
	UpdateVelocity(elapsedTime);

	// �e�ۍX�V����
	projectileManager.Update(elapsedTime);

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	CollisionPlayerVsEnemies();

	// �e�ۂƓG�Ƃ̏Փˏ���
	CollisionProjectilesVsEnemies();

	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();
	
	// �s����O���擾
	direction.x = transform._31;
	direction.y = transform._32;
	direction.z = transform._33;

	// �x�N�g���̐��K��
	float len = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	if (len > 0) {
		direction.x /= len;
		direction.y /= len;
		direction.z /= len;
	}

	// ���f���A�j���[�V�����X�V����(10/23�ǉ�)
	model->UpdateAnimation(elapsedTime);



	// ���f���s��X�V
	model->UpdateTransform(transform);
}



// �`�揈��
void Player::Render(const RenderContext& rc, ModelShader* shader)
{
	shader->Draw(rc, model);

	// �e�ە`�揈��
	projectileManager.Render(rc, shader);
}

// �f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// �f�o�b�O�v���~�e�B�u�\��
		ImGui::Checkbox("VisibleDebugPrimitive", &visibleDebugPrimitive);
		// �ʒu
		ImGui::InputFloat3("Position", &position.x);
		// ��]
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// �X�P�[��
		ImGui::InputFloat3("Scale", &scale.x);
		// �ړ����x
		ImGui::InputFloat("MoveSpeed", &moveSpeed);
		// ���񑬓x
		float turnDeg = DirectX::XMConvertToDegrees(turnSpeed);
		ImGui::InputFloat("TurnSpeed", &turnDeg);
		turnSpeed = DirectX::XMConvertToRadians(turnDeg);
		// ���a
		ImGui::InputFloat("Radius", &radius);
		// ����
		ImGui::InputFloat("Height", &height);
		// ���C
		ImGui::InputFloat("Friction", &friction);
		// �����x
		ImGui::InputFloat("Acceleration", &acceleration);
		// �󒆐���W��
		ImGui::InputFloat("AirControl", &airControl);
	}
}

// �f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
	if (!visibleDebugPrimitive) return;

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// �Փ˔���p�̃f�o�b�O�~����`��
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

	// �e�ۃf�o�b�O�v���~�e�B�u�`��
	projectileManager.DrawDebugPrimitive();
}

// ���n�������ɌĂ΂��
void Player::OnLanding()
{
	jumpCount = 0;

}

// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	// ���͏����擾
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	// �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���Ȃ�悤�ɂ���

	// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	// �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	// �i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	// Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

#if 0
DirectX::XMFLOAT3 convert_quaternion_to_euler(DirectX::XMFLOAT4X4 rotation)
{
	//	ZXY��]
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
	// �����s�񁄃J�����̃��[���h�s��ɕϊ�����
	DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	mat = DirectX::XMMatrixInverse(nullptr, mat);
	// �J�����̃��[���h�s�񂩂�X�P�[���A��]�A���s�ړ����擾
	DirectX::XMVECTOR S, R, T;
	DirectX::XMMatrixDecompose(&S, &R, &T, mat);
	// �K�v�Ȃ͉̂�]��񂾂��Ȃ̂ŁA�X�P�[���ƕ��s�ړ��͖�������
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMStoreFloat4(&quaternion, R);
	// �������]���̓N�H�[�^�j�I���Ȃ̂ŁA�I�C���[�p�ɕϊ�����
	DirectX::XMFLOAT3 euler_rotation = convert_quaternion_to_euler(quaternion);
}
#endif


// �ړ����͏���
void Player::InputMove(float elapsedTime)
{
	// �i�s�x�N�g���擾
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

	// �ړ�����
	Move(moveVec.x, moveVec.z, moveSpeed);

	Mouse& mouse = Input::Instance().GetMouse();
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	Graphics::Instance().GetDeviceContext()->RSGetViewports(&numViewports, &viewport);
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//�}�E�X�J�[�\�����W���擾
	DirectX::XMFLOAT3 screenPosition{};
	screenPosition.x = static_cast<float>(mouse.GetPositionX());
	screenPosition.y = static_cast<float>(mouse.GetPositionY());
	screenPosition.z = 0.0f;//�r���[�|�[�g�̐[�x��0.0��ݒ�
	DirectX::XMVECTOR ScreenPosition, WorldPosition;
	ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

	//�n�_���QD����RD �ɕϊ�
	WorldPosition = DirectX::XMVector3Unproject(
		ScreenPosition,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		viewport.MinDepth, viewport.MaxDepth,
		Projection, View, World
	);

	// ���񏈗�
	moveVec = Camera::Instance().GetFront();
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

// �W�����v���͏���
void Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonDown() & GamePad::BTN_A)
	{
		// �W�����v�񐔐���
		//if (jumpCount < jumpLimit)
		{
			// �W�����v
			jumpCount++;
			Jump(jumpSpeed);

			// �W�����v�A�j���[�V����
			model->PlayAnimation(Animation::Anim_Jump, false);


		}
	}
}

// �e�ۓ��͏���
void Player::InputProjectile()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float mousePosX = Input::Instance().GetMouse().GetPositionX();
	float mousePosY = Input::Instance().GetMouse().GetPositionY();

	// ���i�e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_X)
	{
		// �O����
		DirectX::XMFLOAT3 dir;
		dir.x = Camera::Instance().GetFront().x;
		dir.y = 0.0f;
		dir.z = Camera::Instance().GetFront().z;
		// ���ˈʒu�i�v���C���[�̍�������j
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.05f;
		pos.z = position.z;
		// ����
		ProjectileStraite* projectile = new ProjectileStraite(&projectileManager);
		projectile->Launch(dir, pos);
	}

	// �ǔ��e�۔���
	if (gamePad.GetButtonDown() & GamePad::BTN_Y)
	{
		// �O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0.0f;
		dir.z = cosf(angle.y);

		// ���ˈʒu�i�v���C���[�̍�������j
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y /*+ height * 0.05f*/;
		pos.z = position.z;

		// �^�[�Q�b�g�i�f�t�H���g�ł̓v���C���[�̑O���j
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f + height * 0.5f;
		target.z = pos.z + dir.z * 1000.0f;

		// ��ԋ߂��̓G���^�[�Q�b�g�ɂ���
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; ++i)
		{
			//// �G�Ƃ̋�������
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
		// ����
		ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);
	}
}

void Player::UpdateProjectile()
{
	// �L��Homing�p�Ƀv���C���[�̈ʒu�ݒ�
	if (projectileManager.GetProjectileCount() == 0) return;
	Projectile* projectile = projectileManager.GetProjectile(0);
	if (projectile != NULL) {
		projectile->setTarget(position);
	}
}


// �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̓G�Ƒ�������ŏՓˏ���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// �Փˏ���
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
			// �G�̐^��t�߂ɓ����������𔻒�
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, N);

			// �ォ�瓥��Â����ꍇ�͏��W�����v����
			if (normal.y > 0.8f)
			{
				// ���W�����v����
				Jump(jumpSpeed * 0.5f);
				model->PlayAnimation(Animation::Anim_Jump,false);

				// �_���[�W��^����
				enemy->ApplyDamage(1, 0.5f);
			}
			else
			{
				// �����o����̈ʒu�ݒ�
				enemy->SetPosition(outPosition);
			}
		}
	}
}

// �e�ۂƓG�̏Փˏ���
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; ++i)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			// �Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition))
			{
				// �_���[�W��^����
				if (enemy->ApplyDamage(1, 0.5f))
				{
					// ������΂�
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

					// �q�b�g�G�t�F�N�g�Đ�
					{
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						hitEffect->Play(e);
					}

					// �e�۔j��
					projectile->Destroy();
				}
			}
		}
	}
}
