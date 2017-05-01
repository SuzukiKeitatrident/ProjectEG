#include "Player.h"
#include"DirectXTK.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;
// static変数の実体
const float Player::GRAVITY_ACC = 0.03f;
const float Player::JUMP_SPEED_FIRST = 0.5f;
const float Player::JUMP_SPEED_MAX = 0.3f;

Player::Player()
{
}


Player::~Player()
{
}

void Player::Initialize()
{
	m_Obj[PARTS_TANK].LoadModelFile(L"Resources/cModels/zentai.cmo");
	
	m_Obj[PARTS_BREAST].SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());

	m_Obj[PARTS_TANK].SetTrans(Vector3(0, 0.1f, 0));

	m_Obj[PARTS_BREAST].LoadModelFile(L"Resources/cModels/tankhou1.cmo");

	m_Obj[PARTS_HEAD].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());

	m_Obj[PARTS_BREAST].SetTrans(Vector3(0, 0.01f, 0));
	//
	m_Obj[PARTS_HEAD].LoadModelFile(L"Resources/cModels/tankhou2.cmo");

	m_Obj[PARTS_ARN_R].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());

	m_Obj[PARTS_HEAD].SetTrans(Vector3(0, 0.01f, 0));

	m_Obj[PARTS_ARN_R].LoadModelFile(L"Resources/cModels/houdaiiii.cmo");

	m_Obj[PARTS_ARN_R].SetTrans(Vector3(0, 0.01f, 0));

	m_FireFlag = false;

	m_isJump = false;

	m_isFall = false;

	//初期化
	m_CollisionNodeBody.Initialize();
	//親行列を設定
	m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	//球の半径
	m_CollisionNodeBody.SetLocalRadius(0.6f);
	//親行列からの位置
	m_CollisionNodeBody.SetTrans(Vector3(0, 0.7f, 0));

	TT = 0;
	{
		//初期化
		m_CollsionNodeBullet.Initialize();
		//親行列を設定
		m_CollsionNodeBullet.SetParentMatrix(&m_Obj[PARTS_ARN_R].GetLocalWorld());
		//カプセルの半径
		m_CollsionNodeBullet.SetLocalRadius(0.75f);
		//カプセルの軸長さ
		m_CollsionNodeBullet.SetLenngth(2.0f);
		//親行列からの位置
		m_CollsionNodeBullet.SetTrans(Vector3(0, 0, 0));
		//親行列からの回転
		m_CollsionNodeBullet.SetRot(Vector3(-XM_PIDIV2, 0, 0));
	}
}

void Player::Update()
{
	
	const float ROT_SPEED = 0.03f;
	if (g_key.Up)
	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.x -= ROT_SPEED;
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (g_key.Down)
	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.x += ROT_SPEED;
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	if(g_key.A)
	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	//if (m_pInputManager->GetKeyboard()->IsKeyDown('D'))
	if (g_key.D)
	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	//前進後退
	if (g_key.W)
	{
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		float rot_x = m_Obj[PARTS_TANK].GetRot().x;
		float rot_z = m_Obj[PARTS_TANK].GetRot().z;
		
		SimpleMath::Vector3 moveV(0, 0,+0.1);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		rotm *= Matrix::CreateRotationX(rot_x);
		rotm *= Matrix::CreateRotationZ(rot_z);

		moveV = Vector3::TransformNormal(moveV,rotm);
		trans -= moveV;
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	if (g_key.S)
	{
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y=m_Obj[PARTS_TANK].GetRot().y;
		float rot_x = m_Obj[PARTS_TANK].GetRot().x;
		float rot_z = m_Obj[PARTS_TANK].GetRot().z;

		SimpleMath::Vector3 moveV(0, 0, +0.1f);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		rotm *= Matrix::CreateRotationX(rot_x);
		rotm *= Matrix::CreateRotationZ(rot_z);
		moveV = Vector3::TransformNormal(moveV,rotm);
		trans += moveV;
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	
	
	if (g_key.I)
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.x -= ROT_SPEED;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}
	if (g_key.R)
	{
		Vector3 rot = m_Obj[PARTS_WAIST].GetRot();
		rot.y += 0.1f;
		rot.x -= ROT_SPEED;
		m_Obj[PARTS_WAIST].SetRot(rot);
	}
	if (g_key.Y)
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.y += 0.1f;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}
	if (g_key.T)
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.y -= ROT_SPEED;
		m_Obj[PARTS_HEAD].SetRot(rot);
	}

	if (g_key.F)
	{
		Vector3 rot = m_Obj[PARTS_ARN_R].GetRot();
		rot.y += ROT_SPEED;
		m_Obj[PARTS_ARN_R].SetRot(rot);
	}
	if (g_key.G)
	{
		Vector3 rot = m_Obj[PARTS_ARN_R].GetRot();
		rot.y -= ROT_SPEED;
		m_Obj[PARTS_ARN_R].SetRot(rot);
	}

	if (m_FireFlag)
	{
		m_BulletVel.y -= 0.01f;
		m_Obj[PARTS_ARN_R].SetTrans(m_Obj[PARTS_ARN_R].GetTrans() + m_BulletVel);
	}
	
	if (g_keyTracker->pressed.P)
	{
		TT += 1;
		if (TT == 1)
		{
			FireBullet();
		}
		if (TT == 2)
		{
			ResetBullet();
			TT*= 0;
		}
	}
	if (m_isJump)
	{
		m_Velocity.y -= 0.01f;
	}

	if (g_keyTracker->pressed.Space)
	{
		StartJump();
	}

	m_Obj[PARTS_TANK].SetTrans(m_Obj[PARTS_TANK].GetTrans() + m_Velocity);
	


	Calc();

	m_CollsionNodeBullet.Update();
	m_CollisionNodeBody.Update();
}

void Player::Calc()
{

	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Calc();
	}
}

void Player::Draw()
{
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Draw();
	}
	m_CollsionNodeBullet.Draw();
	m_CollisionNodeBody.Draw();
}

//弾丸用のパーツを射出する
void Player::FireBullet()
{
	//既に発射中
	if (m_FireFlag)return;

	//親子関係を加味したワールド座標を取得
	Matrix worldm = m_Obj[PARTS_ARN_R].GetLocalWorld();

	Vector3 scale;
	Quaternion rotq;
	Vector3 pos;
	//行列の１行ずつをVector3として扱う
	Vector3* m0 = (Vector3*)&worldm.m[0];
	Vector3* m1 = (Vector3*)&worldm.m[1];
	Vector3* m2 = (Vector3*)&worldm.m[2];
	Vector3* m3 = (Vector3*)&worldm.m[3];
	//ワールド座標を取り出す
	pos = *m3;
	//ワールドスケーリングを取り出す
	scale = Vector3(m0->Length(), m1->Length(), m2->Length());
	//スケーリングを打ち消す
	//　※これにより、左上の3x3行列が、回転のみの行列になる
	m0->Normalize();
	m1->Normalize();
	m2->Normalize();
	//ワールド回転を取り出す
	rotq = Quaternion::CreateFromRotationMatrix(worldm);
	
	//親子関係を解除する
	m_Obj[PARTS_ARN_R].SetParentMatrix(nullptr);
	m_Obj[PARTS_ARN_R].SetScale(scale);
	m_Obj[PARTS_ARN_R].SetRotQ(rotq);
	m_Obj[PARTS_ARN_R].SetTrans(pos);

	//発射する弾丸の速度ベクトル
	m_BulletVel = Vector3(0, 0.1f, 0);

	//ベクトルをクォータニオンで回転
	m_BulletVel = Vector3::Transform(m_BulletVel, rotq);

	m_FireFlag = true;
}

//弾丸用のパーツをロボットに取り付けなおす
void Player::ResetBullet()
{
	//発射中ではない
	if (!m_FireFlag)		return;

	m_Obj[PARTS_ARN_R].SetParentMatrix(&m_Obj[PARTS_HEAD].GetLocalWorld());

	m_Obj[PARTS_ARN_R].SetScale(Vector3(1, 1, 1));
	m_Obj[PARTS_ARN_R].SetRot(Vector3(0, 0, 0));
	m_Obj[PARTS_ARN_R].SetTrans(Vector3(0.0f, 0.0f, 0.0f));

	m_FireFlag = false;
}

float GetShortAngleRad();

const DirectX::SimpleMath::Vector3& Player::GetTrans()
{
	return m_Obj[PARTS_TANK].GetTrans();
}

void Player::SetTrans(const DirectX::SimpleMath::Vector3& trans)
{
	m_Obj[PARTS_TANK].SetTrans(trans);
}

const DirectX::SimpleMath::Matrix& Player::GetLocalWorld()
{
	return m_Obj[PARTS_TANK].GetLocalWorld();
}

const DirectX::SimpleMath::Vector3 Player::GetRot()
{
	return m_Obj[PARTS_TANK].GetRot();
}
const DirectX::SimpleMath::Vector3& Player::GetVelocity()
{
	return m_Velocity;
}

// ジャンプを開始する
void Player::StartJump() 
{
	// ジャンプ中でないか
	if (!m_isJump)
	{
		// 上方向の初速を設定
		m_Velocity.y = JUMP_SPEED_FIRST;
		// ジャンプフラグを立てる
		m_isJump = true;
	}
}

// 落下を開始する
void Player::StartFall() 
{
	// ジャンプ中でないか
	if (!m_isJump)
	{
		// 上方向の初速は0
		m_Velocity.y = 0.0f;
		// ジャンプフラグを立てる
		m_isJump = true;
	}
}

// ジャンプを終了する
void Player::StopJump() {
	// ジャンプフラグを下ろす
	m_isJump = false;
	// デフォルト値で初期化
	m_Velocity = Vector3();
}


