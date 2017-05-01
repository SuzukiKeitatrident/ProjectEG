//敵

#include "Enmey.h"
#include"DirectXTK.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy()
{
	m_Timer = 0;
	m_DistAngle = 0.0f;
}


Enemy::~Enemy()
{
}

void Enemy::Initialize()
{
	m_Obj[PARTS_TANK].LoadModelFile(L"Resources/cModels/zentai.cmo");

	m_Obj[PARTS_BREAST].SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());

	m_Obj[PARTS_TANK].SetTrans(Vector3(0, 0.01f, 0));

	m_Obj[PARTS_BREAST].LoadModelFile(L"Resources/cModels/tankhou1.cmo");

	m_Obj[PARTS_HEAD].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());

	m_Obj[PARTS_BREAST].SetTrans(Vector3(0, 0.01f, 0));
	//
	m_Obj[PARTS_HEAD].LoadModelFile(L"Resources/cModels/tankhou2.cmo");

	m_Obj[PARTS_ARN_R].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());

	m_Obj[PARTS_HEAD].SetTrans(Vector3(0, 0.01f, 0));

	m_Obj[PARTS_ARN_R].LoadModelFile(L"Resources/cModels/houdaiiii.cmo");

	m_Obj[PARTS_ARN_R].SetTrans(Vector3(0, 0.01f, 0));

	//初期化
	m_CollisionNodeBody.Initialize();
	//親行列を設定
	m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	//球の半径
	m_CollisionNodeBody.SetLocalRadius(0.2f);
	//親行列からの位置
	m_CollisionNodeBody.SetTrans(Vector3(0,0.7f,0));

	//初期化
	m_CollisionNodeARN.Initialize();
	//親行列を設定
	m_CollisionNodeARN.SetParentMatrix(&m_Obj[PARTS_ARN_R].GetLocalWorld());
	//球の半径
	m_CollisionNodeARN.SetLocalRadius(0.4f);
	//親行列からの位置
	m_CollisionNodeARN.SetTrans(Vector3(0, 0.7f, 0));

	//初期化
	m_CollisionNodeBRE.Initialize();
	//親行列を設定
	m_CollisionNodeBRE.SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());
	//球の半径
	m_CollisionNodeBRE.SetLocalRadius(0.6f);
	//親行列からの位置
	m_CollisionNodeBRE.SetTrans(Vector3(0, 0.7f, 0));


	death = false;
	death2 = false;
	death3 = false;
}

void Enemy::Update()
{
	if (death)
	{
		return;
	}
	//定期的に進行方向を変える
	//m_Timer--;	//メンバ変数でタイマーを作ってき、カウントダウン
	//if (m_Timer < 0)
	//{
	//	//カウントが０に達したら
	//	m_Timer = 60;
	//	//目標角度を変更
	//	float rnd = (float)rand() / RAND_MAX - 0.5f;
	//	rnd *= 180.0f;
	//	m_DistAngle += rnd;//メンバ変数で目標角度を保持
	//}
	////目標角度に向かって、機体の角度をじわじわ補間
	//{
	//	Vector3 rotv = m_Obj[PARTS_TANK].GetRot();
	//	//目標角度への、最短角度を取得
	//	float angle = GetShortAngleRad(rotv.y, XMConvertToRadians(m_DistAngle));
	//	rotv.y += angle*0.01f;
	//	m_Obj[PARTS_TANK].SetRot(rotv);
	//}
	////機体の向いている方向に進む
	//{
	//	//今の座標を取得
	//	Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
	//	
	//	Vector3 move(0, 0, -0.02f);
	//	Vector3 rotv = m_Obj[PARTS_TANK].GetRot();
	//	Matrix rotm = Matrix::CreateRotationY(rotv.y);
	//	
	//	//座標を移動
	//	trans += move;

	//	//移動後の座標をセット
	//	m_Obj[PARTS_TANK].SetTrans(trans);

		//移動を反映して行列更新
		Calc();

		//当たり判定の更新（親行列更新後に行うこと）
		m_CollisionNodeBody.Update();
		m_CollisionNodeARN.Update();
		m_CollisionNodeBRE.Update();

		if (death2)
		{
			m_Obj[PARTS_ARN_R].SetTrans(Vector3(m_Obj[PARTS_ARN_R].GetTrans().x, m_Obj[PARTS_ARN_R].GetTrans().y + 0.1f, m_Obj[PARTS_ARN_R].GetTrans().z));
			m_Timer2++;
		}
}



float Enemy::GetShortAngleRad(float nowangle, float dstAngle)
{
	

	if (dstAngle - nowangle < XM_PI)
	{
		return dstAngle - nowangle;
	}
	else
	{
		return (XM_2PI - (nowangle-dstAngle));
	}
}

void Enemy::Calc()
{
	for (int i = 0; i < PARTS_NUM; i++)
	{
		if (i==PARTS_ARN_R)
		{
			if (death2&&60 < m_Timer2)
			{
				continue;
			}
		}
		if (i == PARTS_BREAST)
		{
			if (death3)
			{
				continue;
			}
		}
		m_Obj[i].Calc();
	}

}

void Enemy::Draw()
{
	if (death)
	{
		return;
	}
	for (int i = 0; i < PARTS_NUM; i++)
	{
		if (i==PARTS_ARN_R)
		{
			if (death2 && 60 < m_Timer2)
			{
				continue;
			}
		}
		if (i == PARTS_BREAST)
		{
			if (death3)
			{
				continue;
			}
		}

		m_Obj[i].Draw();
	}
	m_CollisionNodeBody.Draw();
	m_CollisionNodeARN.Draw();
	m_CollisionNodeBRE.Draw();
}

const DirectX::SimpleMath::Vector3& Enemy::GetTrans()
{
	return m_Obj[PARTS_TANK].GetTrans();
}

void Enemy::SetTrans(const DirectX::SimpleMath::Vector3& trans)
{
	m_Obj[PARTS_TANK].SetTrans(trans);
}

const DirectX::SimpleMath::Matrix& Enemy::GetLocalWorld()
{
	return m_Obj[PARTS_TANK].GetLocalWorld();
}

const DirectX::SimpleMath::Vector3 Enemy::GetRot()
{
	return m_Obj[PARTS_TANK].GetRot();
}

bool Enemy::GetDeath()
{
	return 	death;
}

void Enemy::SetDeath(bool flag)
{
	death = flag;
}

bool Enemy::GetDeath2()
{
	return 	death2;
}

void Enemy::SetDeath2(bool flag2)
{
	death2 = flag2;
}
bool Enemy::GetDeath3()
{
	return 	death3;
}

void Enemy::SetDeath3(bool flag3)
{
	death3 = flag3;
}

