#pragma once

#include <DirectXMath.h>
#include <D3D11.h>

struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
};
// 点光源情報
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	color;		// 色
	float				range;		// 範囲
	float				influence;	// 光の影響力
	DirectX::XMFLOAT2	dummy;
};

// 点光源の最大数
static	constexpr	int	PointLightMax = 8;

// スポットライト情報
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
	float			range;		// 範囲
	float			innerCorn; 	// インナー角度範囲
	float			outerCorn; 	// アウター角度範囲
	float			influence; // 光の影響力;
};

// スポットライトの最大数
static	constexpr	int	SpotLightMax = 8;


// マスクデータ
struct MaskData
{
	ID3D11ShaderResourceView* maskTexture;
	float					dissolveThreshold;
	float					edgeThreshold; 	// 縁の閾値
	DirectX::XMFLOAT4		edgeColor;		// 縁の色

};


//	UVスクロール情報
struct UVScrollData
{
	DirectX::XMFLOAT2	uvScrollValue;	// UVスクロール値
};

// トゥーンシェーディング用アウトライン
struct OutlineData
{
	DirectX::XMFLOAT4X4		viewProjection;
	DirectX::XMFLOAT4		outlineColor;
	float					outlineSize;
	DirectX::XMFLOAT3		dummy;
};

//	シャドウマップ用情報
struct ShadowMapData
{
	ID3D11ShaderResourceView* shadowMap;				//	シャドウマップテクスチャ
	DirectX::XMFLOAT4X4			lightViewProjection;	//	ライトビュープロジェクション行列
	DirectX::XMFLOAT3			shadowColor;			//	影の色
	float						shadowBias;				//	深度比較用のオフセット値
	int                         PCFKernelSize;          // ソフトシャドーの行列サイズ
	//DirectX::XMFLOAT3           dummy;
};



// レンダーコンテキスト
struct RenderContext
{
	ID3D11DeviceContext*	deviceContext;		// ★変更
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;

	UVScrollData			uvScrollData;
	MaskData				maskData;

	//	ライト情報
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData		pointLightData[PointLightMax];	// 点光源情報
	int					pointLightCount = 0;			// 点光源数
	SpotLightData		spotLightData[SpotLightMax];	// スポットライト情報
	int					spotLightCount = 0;			// スポットライト数
	OutlineData				outlinedata;

	//	シャドウマップ情報
	ShadowMapData			shadowMapData;

};


