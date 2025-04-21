#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "LightManager.h"
#include "StageManager.h"
#include "Input/Input.h"

Light* ProjectileLight;

//	シャドウマップのサイズ
static	const	UINT	SHADOWMAP_SIZE = 2048;

// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化
	stage = new Stage();

	StageManager::Instance().Register(stage);

	// プレイヤー
	player = new Player();

	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(-10, 2, 0),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	// カメラコントローラー初期化
	cameraController = new CameraController();
	// 平行光源を追加
	//LightManager::Instance().Register(new Light(LightType::Directional));

	{
		mainDirectionalLight = new Light(LightType::Directional);
		mainDirectionalLight->SetDirection({ 1, -1, -1 });
		LightManager::Instance().Register(mainDirectionalLight);
	}

	// 点光源を追加
	{
		Light* light = new Light(LightType::Point);
		light->SetPosition(DirectX::XMFLOAT3(0, 4, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		light->SetRange(5.0f);
		LightManager::Instance().Register(light);
	}
	// 点光源を追加
	{
		Light* light = new Light(LightType::Point);
		light->SetPosition(DirectX::XMFLOAT3(5, 4, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 0, 1));
		light->SetRange(5.0f);
		LightManager::Instance().Register(light);
	}
	// スポットライトを追加
	{
		Light* light = new Light(LightType::Spot);
		light->SetPosition(DirectX::XMFLOAT3(-10, 10, 10));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		light->SetDirection(DirectX::XMFLOAT3(+1, -1, 0));

		light->SetRange(40.0f);
		LightManager::Instance().Register(light);
	}
	// 点光源を追加
	{
		ProjectileLight = new Light(LightType::Point);
		ProjectileLight->SetPosition(DirectX::XMFLOAT3(0, 4, 0));
		ProjectileLight->SetColor(DirectX::XMFLOAT4(0, 0, 0, 0));
		ProjectileLight->SetRange(5.0f);
		LightManager::Instance().Register(ProjectileLight);
	}

	// シャドウマップ用に深度ステンシルの生成
	{
		Graphics& graphics = Graphics::Instance();
		shadowmapDepthStencil = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
	}



	// エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 2; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(slime);
	}

	//スカイボックス
	skybox = new SkyBox("Data/SkyBox/town20220706.jpg");
}

// 終了化
void SceneGame::Finalize()
{
	// エネミー終了化
	EnemyManager::Instance().Clear();

	// カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	// プレイヤー終了化
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// ステージ終了化
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

	delete skybox;
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	
	//////////////////////////////////
	//横スクロールの場合ここを外す
	//////////////////////////////////

	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// ステージ更新処理
	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
	
	{
		// ライト更新処理(仮)
		Light* light = LightManager::Instance().GetLight(3);
		DirectX::XMFLOAT3 p = player->GetPosition();
		p.y += 1.5f;
		light->SetPosition(p);
		DirectX::XMFLOAT3 d = player->GetDirection();
		d.y -= 0.2f;
		light->SetDirection(d);
	}

	{
		// ライト更新処理(仮)
		Light* light = LightManager::Instance().GetLight(4);
		Projectile* p = nullptr;
		p = player->GetProjectile(0);

		if (p != nullptr)
		{
			DirectX::XMFLOAT3 pos = p->GetPosition();
			light->SetPosition(pos);
			light->SetInfluence(2.0f); //光の影響力
			light->SetColor(DirectX::XMFLOAT4(1, 1, 0.2f, 1)); //光の色
		}


		GamePad& gamePad = Input::Instance().GetGamePad();

		const GamePadButton anyButton =
			  GamePad::BTN_B;

		if (gamePad.GetButtonDown() & anyButton)
		{
			// 光の影響力を0にする(消すように見せている)
			light->SetInfluence(0.0f);
		}
	}

}

//-----------------------------------------------------
//              描画処理
//-----------------------------------------------------

//	シャドウマップの描画
void SceneGame::RenderShadowmap()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11DepthStencilView* dsv = shadowmapDepthStencil->GetDepthStencilView().Get();

	// 画面クリア
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (!mainDirectionalLight)
		return;

	// レンダーターゲット設定
	dc->OMSetRenderTargets(0, &rtv, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(shadowmapDepthStencil->GetWidth());
	vp.Height = static_cast<float>(shadowmapDepthStencil->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	// 描画処理
	RenderContext rc;
	rc.deviceContext = dc;

	// カメラパラメータ設定
	{
		// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&mainDirectionalLight->GetDirection());
		LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);//カメラ250上空
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// シャドウマップに描画したい範囲の射影行列を生成
		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowDrawRect, shadowDrawRect, 0.1f, 1000.0f);//遠すぎる可能性1000
		DirectX::XMStoreFloat4x4(&rc.view, V);
		DirectX::XMStoreFloat4x4(&rc.projection, P);
		DirectX::XMStoreFloat4x4(&lightViewProjection, V * P);
	}

	// 3Dモデル描画
	{
		ModelShader* shader = graphics.GetShader(ModelShaderId::ShadowmapCaster);
		shader->Begin(rc);

		// ステージ描画
		StageManager::Instance().Render(rc, shader);
		//stage->Render(rc, shader);

		// プレイヤー描画
		player->Render(rc, shader);

		// エネミー描画
		EnemyManager::Instance().Render(rc, shader);

		shader->End(rc);
	}
}

void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// シャドウマップの描画
	RenderShadowmap();

	// 3D空間の描画を別のバッファに対して行う
	//Render3DScene();


	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）
		// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// シャドウマップの設定
	rc.shadowMapData.shadowMap = shadowmapDepthStencil->GetShaderResourceView().Get();
	rc.shadowMapData.lightViewProjection = lightViewProjection;
	rc.shadowMapData.shadowColor = shadowColor;
	rc.shadowMapData.shadowBias = shadowBias;
	rc.shadowMapData.PCFKernelSize = PCFKernelSize;


	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = graphics.GetScreenWidth();
	vp.Height = graphics.GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);


	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
	rc.outlinedata.outlineColor = outlineColor;
	rc.outlinedata.outlineSize = outlinewidth;
	rc.deviceContext = dc;

	// 3Dモデル描画
	{
		//Shader* shader = graphics.GetShader();
		//ModelShader* shader = graphics.GetShader(ModelShaderId::Default);
		ModelShader* shader = graphics.GetShader(ModelShaderId::Phong);

		shader->Begin(rc);
		
		// ステージ描画
		//stage->Render(rc, shader);
		StageManager::Instance().Render(rc, shader);

		// プレイヤー描画
		player->Render(rc, shader);

		// エネミー描画
		EnemyManager::Instance().Render(rc, shader);

		shader->End(rc);
	}
	//{
	//	Shader* shader = graphics.GetShader();
	//	ModelShader* shader = graphics.GetShader(ModelShaderId::Toon);

	//	shader->Begin(rc);
	//	
	//	 プレイヤー描画
	//	player->Render(rc, shader);
	//	 エネミー描画
	//	EnemyManager::Instance().Render(rc, shader);


	//	shader->End(rc);
	//}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	//// 3Dデバッグ描画
	//{

	//	// ライトのデバッグプリミティブの描画
	//	LightManager::Instance().DrawDebugPrimitive();

	//	// プレイヤーデバッグ描画
	//	player->DrawDebugPrimitive();

	//	// エネミーデバッグ描画
	//	EnemyManager::Instance().DrawDebugPrimitive();

	//	// ラインレンダラ描画実行
	//	graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

	//	// デバッグレンダラ描画実行
	//	graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	//}

	// 2Dスプライト描画
	{
		skybox->Render(dc, rc);
	}

	// 2DデバッグGUI描画
	{
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_None))
		{
			int id = 0;
			// デバッグ情報の表示
			ImGui::PushID(id++);
			{
				LightManager::Instance().DrawDebugGUI();

			}
			ImGui::PopID();

			// 
			ImGui::PushID(id++);
			{
				if (ImGui::CollapsingHeader("Toon", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::SliderFloat("OutLine", &outlinewidth, 0, 1.0f);
					ImGui::ColorEdit3("OutLineColor", &outlineColor.x);
				}

			}
			ImGui::PopID();

			// プレイヤーデバッグ描画
			ImGui::PushID(id++);
			{
				player->DrawDebugGUI();
			}
			ImGui::PopID();
			// カメラデバッグ描画
			ImGui::PushID(id++);
			{
				camera.DrawDebugGUI();
			}
			ImGui::PopID();

			// カメラコントローラーデバッグ描画
			ImGui::PushID(id++);
			{
				cameraController->DrawDebugGUI();
			}
			ImGui::PopID();

			// エネミーマネージャーデバッグ描画
			ImGui::PushID(id++);
			{
				EnemyManager::Instance().DrawDebugGUI();
			}
			ImGui::PopID();

			//シャドーマップ
			ImGui::PushID(id++);
			{
				//ImGui::Separator();//線
				if (ImGui::CollapsingHeader("ShadowMap", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::SliderFloat("DrawRect", &shadowDrawRect, 1.0f,512.0f);//荒くなる
					ImGui::ColorEdit3("Color", &shadowColor.x);
					ImGui::SliderFloat("Bias", &shadowBias, 0.0f, 0.01f,"% 0.5f");
					ImGui::SliderInt("PCFKernelSize", &PCFKernelSize, 1, 15);
					ImGui::Text("texture");
					ImGui::Image(shadowmapDepthStencil->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });//textureのサイズ256,256

					//ImGui::TreePop();
				}
				//ImGui::Separator();
			}
			ImGui::PopID();
		}
		ImGui::End();
	}
}

//// 3D空間の描画
//void SceneGame::Render3DScene()
//{
//    // ライトの情報を詰め込む
//	LightManager::Instance().PushRenderContext(rc);
//
//	// シャドウマップの設定
//	rc.shadowMapData.shadowMap = shadowmapDepthStencil->GetShaderResourceView().Get();
//	rc.shadowMapData.lightViewProjection = lightViewProjection;
//	rc.shadowMapData.shadowColor = shadowColor;
//	rc.shadowMapData.shadowBias = shadowBias;
//}




