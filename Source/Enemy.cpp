#include "Enemy.h"
#include "EnemyManager.h"
#include "Graphics/Graphics.h"
#include "Player.h"


DirectX::XMFLOAT3 Enemy::GetMoveVec()const
{
	DirectX::XMFLOAT3 target{ 5,0,-10 }; // (¼)
	target = Player::Instance().GetPosition();

	DirectX::XMFLOAT3 vec;
	vec.x = target.x  - GetPosition().x;
	vec.y = 0;
	vec.z = target.z - GetPosition().z;
	// ³K»
	float len = sqrtf(vec.x * vec.x + vec.z * vec.z);
	if (len > 0) { vec.x /= len; vec.z /= len; }

	return vec;

}


// fobOv~eBu`æ
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// ÕË»èpÌfobO~ð`æ
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// fobOpGUI`æ
void Enemy::DrawDebugGUI()
{
	// Êu
	ImGui::InputFloat3("Position", &position.x);
	// ¼a
	ImGui::InputFloat("Radius", &radius);
	// ³
	ImGui::InputFloat("Height", &height);
	// N
	ImGui::InputInt("Health", &health);
}

// jü
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
