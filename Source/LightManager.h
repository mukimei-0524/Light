#pragma once

#include <vector>
#include "Light.h"

#include "Graphics/RenderContext.h"

// ライト管理クラス
class LightManager
{
private:
	LightManager() {}
	~LightManager();

public:
	static LightManager& Instance()
	{
		static	LightManager	lightManager;
		return	lightManager;
	}

	// ライトを登録する
	void Register(Light* light);

	// ライトの登録を解除する
	void Remove(Light* light);

	//インデックス番号のライトを削除する
	void LightManager::RemoveIndex(int Index);

	// 登録済みのライトを全削除する
	void Clear();

	// ライト情報をRenderContextに積む
	void PushRenderContext(RenderContext& rc);

	// デバッグ情報の表示
	void DrawDebugGUI();

	// デバッグプリミティブの表示
	void DrawDebugPrimitive();

	// 環境光
	DirectX::XMFLOAT4	GetAmbientColor() { return	ambientColor; }
	void	SetAmbientColor(DirectX::XMFLOAT4 ambientColor) { this->ambientColor = ambientColor; }

	// ライトの情報
	int GetLightCount() const { return static_cast<int>(lights.size()); }
	Light* GetLight(int index) const { return lights.at(index); }

private:
	std::vector<Light*>	lights;
	DirectX::XMFLOAT4	ambientColor = { 0.2f, 0.2f, 0.2f, 1.0f };
};
