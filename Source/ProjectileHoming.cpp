#include "ProjectileHoming.h"

// �R���X�g���N�^
ProjectileHoming::ProjectileHoming(ProjectileManager* manager)
    : Projectile(manager)
{
    model = new Model("Data/Model/Sword/Cat.mdl");

    // ���f�����������̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.3f;

    Light_Line = new Model("Data/Model/Sword/light_line.mdl");
    Light_Line_Scale.x = Light_Line_Scale.y = Light_Line_Scale.z = 0.08f;


}

// �f�X�g���N�^
ProjectileHoming::~ProjectileHoming()
{
    delete model;
    delete Light_Line;

}

// �X�V����
//void ProjectileHoming::Update(float elapsedTime)
//{
//    // ���������̉����i�K�v�ɉ����Ē����j
//    // �߂��Ă��Ă�����폜
//    lifeTimer -= elapsedTime;
//    if (lifeTimer <= 0.0f&& !beWalking)
//    {
//        // �������s������폜
//        Destroy();
//        return;
//    }
//
//    // ���@�̍��W���^�[�Q�b�g�ɐݒ�i��F�v���C���[�̍��W���擾�j
//    DirectX::XMFLOAT3 selfPosition = { -50.0f, 0.2f, 0.0f }; // ���̎��@���W
//    //target = selfPosition;
//
//    // ���@�Ƃ̋������v�Z
//    DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
//    DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&selfPosition);
//    DirectX::XMVECTOR VecToTarget = DirectX::XMVectorSubtract(Target, Position);
//
//    // �������v�Z
//    DirectX::XMVECTOR Length = DirectX::XMVector3Length(VecToTarget);
//    float distance;
//    DirectX::XMStoreFloat(&distance, Length);
//
//    // ���@�ɋ߂Â�����e���폜
//    const float thresholdDistance = 1.0f; // ���@�ɐڐG�ƌ��Ȃ�������臒l
//    if (distance <= thresholdDistance)
//    {
//        //Destroy(); // �e���폜
//        return;
//    }
//
//    // �~�^���̂��߂̊p���x
//    float angularSpeed = DirectX::XMConvertToRadians(30); // �Ⴍ����΂���قǉ������
//    float moveSpeed = this->moveSpeed * elapsedTime;
//
//    if (distance > 50.0f)
//    {
//        // �~�^���̌v�Z
//        float angle = angularSpeed * elapsedTime;
//        float sinAngle = sinf(angle);
//        float cosAngle = cosf(angle);
//
//        // ���݂̕�������ɉ�]�i�E���j
//        float newX = direction.x * cosAngle - direction.z * sinAngle;
//        float newZ = direction.x * sinAngle + direction.z * cosAngle;
//
//        // �V���������𐳋K��
//        direction.x = newX;
//        direction.z = newZ;
//        float norm = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
//        direction.x /= norm;
//        direction.z /= norm;
//
//        // �ʒu���X�V
//        position.x += direction.x * moveSpeed;
//        position.y += direction.y * moveSpeed;
//        position.z += direction.z * moveSpeed;
//    }
//
//    // �I�u�W�F�N�g�s����X�V
//    UpdateTransform();
//
//    // ���f���s��X�V
//    model->UpdateTransform(transform);
//}



// �X�V����
void ProjectileHoming::Update(float elapsedTime)
{
    // ��������
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        // �������폜
        Destroy();
    }

    // �ړ�
    {
        float moveSpeed = this->moveSpeed * elapsedTime;
        position.x += direction.x * moveSpeed;
        position.y += direction.y * moveSpeed;
        position.z += direction.z * moveSpeed;
    }

    // ����
    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        // �^�[�Q�b�g�܂ł̃x�N�g�����Z�o
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // �[���x�N�g���łȂ��Ȃ��]����
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        if (lengthSq > 10.0f)
        //if (lengthSq > 0.00001f)
        {
            // �^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����
            Vec = DirectX::XMVector3Normalize(Vec);

            // �����Ă�������x�N�g�����Z�o
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            // �O�������x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓���(�p�x)���Z�o
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            // �Q�̒P�ʃx�N�g���̊p�x���������ق�
            // 1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
            float rot = 1.0f - dot;
            if (rot > turnSpeed)
            {
                rot = turnSpeed;
            }

            // ��]�p�x������Ȃ��]����������
            if (fabsf(rot) > 0.0001f)
            {
                // ��]�����Z�o
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);

                // ��]���Ɖ�]�ʂ����]�s����Z�o
                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                // ���݂̍s�����]������
                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                // ��]��̑O�����������o���A�P�ʃx�N�g��������
                Direction = DirectX::XMVector3Normalize(Transform.r[2]);
                DirectX::XMStoreFloat3(&direction, Direction);
            }
        }
    }

    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���s��X�V
    model->UpdateTransform(transform);

    // �I�u�W�F�N�g�s����X�V
    LIghtLineUpdateTransform();
    // ���f���s��X�V
    Light_Line->UpdateTransform(transform);

}


// �`�揈��
void ProjectileHoming::Render(const RenderContext& rc, ModelShader* shader)
{
    //shader->Begin(rc);
    shader->Draw(rc, model);
    shader->Draw(rc, Light_Line);

}

// ����
void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}
