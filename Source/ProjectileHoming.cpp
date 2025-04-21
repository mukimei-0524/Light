#include "ProjectileHoming.h"

// コンストラクタ
ProjectileHoming::ProjectileHoming(ProjectileManager* manager)
    : Projectile(manager)
{
    model = new Model("Data/Model/Sword/Cat.mdl");

    // モデルが小さいのでスケーリング
    scale.x = scale.y = scale.z = 0.3f;

    Light_Line = new Model("Data/Model/Sword/light_line.mdl");
    Light_Line_Scale.x = Light_Line_Scale.y = Light_Line_Scale.z = 0.08f;


}

// デストラクタ
ProjectileHoming::~ProjectileHoming()
{
    delete model;
    delete Light_Line;

}

// 更新処理
//void ProjectileHoming::Update(float elapsedTime)
//{
//    // 寿命処理の延長（必要に応じて調整）
//    // 戻ってきていたら削除
//    lifeTimer -= elapsedTime;
//    if (lifeTimer <= 0.0f&& !beWalking)
//    {
//        // 寿命が尽きたら削除
//        Destroy();
//        return;
//    }
//
//    // 自機の座標をターゲットに設定（例：プレイヤーの座標を取得）
//    DirectX::XMFLOAT3 selfPosition = { -50.0f, 0.2f, 0.0f }; // 仮の自機座標
//    //target = selfPosition;
//
//    // 自機との距離を計算
//    DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
//    DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&selfPosition);
//    DirectX::XMVECTOR VecToTarget = DirectX::XMVectorSubtract(Target, Position);
//
//    // 距離を計算
//    DirectX::XMVECTOR Length = DirectX::XMVector3Length(VecToTarget);
//    float distance;
//    DirectX::XMStoreFloat(&distance, Length);
//
//    // 自機に近づいたら弾を削除
//    const float thresholdDistance = 1.0f; // 自機に接触と見なす距離の閾値
//    if (distance <= thresholdDistance)
//    {
//        //Destroy(); // 弾を削除
//        return;
//    }
//
//    // 円運動のための角速度
//    float angularSpeed = DirectX::XMConvertToRadians(30); // 低くすればするほど遠く回る
//    float moveSpeed = this->moveSpeed * elapsedTime;
//
//    if (distance > 50.0f)
//    {
//        // 円運動の計算
//        float angle = angularSpeed * elapsedTime;
//        float sinAngle = sinf(angle);
//        float cosAngle = cosf(angle);
//
//        // 現在の方向を基準に回転（右回り）
//        float newX = direction.x * cosAngle - direction.z * sinAngle;
//        float newZ = direction.x * sinAngle + direction.z * cosAngle;
//
//        // 新しい方向を正規化
//        direction.x = newX;
//        direction.z = newZ;
//        float norm = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
//        direction.x /= norm;
//        direction.z /= norm;
//
//        // 位置を更新
//        position.x += direction.x * moveSpeed;
//        position.y += direction.y * moveSpeed;
//        position.z += direction.z * moveSpeed;
//    }
//
//    // オブジェクト行列を更新
//    UpdateTransform();
//
//    // モデル行列更新
//    model->UpdateTransform(transform);
//}



// 更新処理
void ProjectileHoming::Update(float elapsedTime)
{
    // 寿命処理
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        // 自分を削除
        Destroy();
    }

    // 移動
    {
        float moveSpeed = this->moveSpeed * elapsedTime;
        position.x += direction.x * moveSpeed;
        position.y += direction.y * moveSpeed;
        position.z += direction.z * moveSpeed;
    }

    // 旋回
    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        // ターゲットまでのベクトルを算出
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // ゼロベクトルでないなら回転処理
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        if (lengthSq > 10.0f)
        //if (lengthSq > 0.00001f)
        {
            // ターゲットまでのベクトルを単位ベクトル化
            Vec = DirectX::XMVector3Normalize(Vec);

            // 向いている方向ベクトルを算出
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            // 前方方向ベクトルとターゲットまでのベクトルの内積(角度)を算出
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            // ２つの単位ベクトルの角度が小さいほど
            // 1.0に近づくという性質を利用して回転速度を調整する
            float rot = 1.0f - dot;
            if (rot > turnSpeed)
            {
                rot = turnSpeed;
            }

            // 回転角度があるなら回転処理をする
            if (fabsf(rot) > 0.0001f)
            {
                // 回転軸を算出
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);

                // 回転軸と回転量から回転行列を算出
                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                // 現在の行列を回転させる
                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                // 回転後の前方方向を取り出し、単位ベクトル化する
                Direction = DirectX::XMVector3Normalize(Transform.r[2]);
                DirectX::XMStoreFloat3(&direction, Direction);
            }
        }
    }

    // オブジェクト行列を更新
    UpdateTransform();

    // モデル行列更新
    model->UpdateTransform(transform);

    // オブジェクト行列を更新
    LIghtLineUpdateTransform();
    // モデル行列更新
    Light_Line->UpdateTransform(transform);

}


// 描画処理
void ProjectileHoming::Render(const RenderContext& rc, ModelShader* shader)
{
    //shader->Begin(rc);
    shader->Draw(rc, model);
    shader->Draw(rc, Light_Line);

}

// 発射
void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}
