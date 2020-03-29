#include "Solver.h"
#include "Body.h"
#include "Collision.h"
#include "../hoLib/num/numerical.hpp"
using namespace ho::num;

namespace Phys3D
{
	//コンストラクタ
	Solver::Solver(const CONTACT &Contact, const std::weak_ptr<Collision> &wpCollisionObj)
		: wpCollisionObj(wpCollisionObj), Contact(Contact), TotalForceNormal(0), TotalForceTangent(0)
	{
		std::shared_ptr<Body> spBodyObj[2] = { Contact.Pair.wpBodyObj[0].lock(), Contact.Pair.wpBodyObj[1].lock() };

		this->e = sqrt(spBodyObj[0]->gete() * spBodyObj[1]->gete()); //2物体間の反発係数を計算
		this->mu = sqrt(spBodyObj[0]->getmu() * spBodyObj[1]->getmu()); //2物体間の摩擦係数を計算

		vec3d vecRelVel = Body::GetRelativeVelocity(Contact.Pair.wpBodyObj[0], Contact.Pair.wpBodyObj[1], Contact.vecWorldPos); //接触位置における物体間の相対速度
		double RelVelNormal = vec3d::GetDot(vecRelVel, Contact.vecN); //相対速度の法線方向成分を内積により計算

		//法線方向の目標相対速度を設定
		if (RelVelNormal < -0.5) //0.5[m/s]以上の速さで近づいている場合
			this->TargetRelVelNormal = -RelVelNormal * this->e; //衝突後の目標相対速度は衝突前の近づく速度に反発係数を掛けた分だけ離れるものとなる
		else
			this->TargetRelVelNormal = 0;
	}

	//デストラクタ
	Solver::~Solver()
	{}

	//接触継続のため、接触情報を更新
	void Solver::Continue(const CONTACT &Contact)
	{
		this->Contact = Contact;

		vec3d vecRelVel = Body::GetRelativeVelocity(Contact.Pair.wpBodyObj[0], Contact.Pair.wpBodyObj[1], Contact.vecWorldPos); //接触位置における物体間の相対速度
		double RelVelNormal = vec3d::GetDot(vecRelVel, Contact.vecN); //相対速度の法線方向成分を内積により計算

		//法線方向の目標相対速度を設定
		if (RelVelNormal < -0.5) //0.5[m/s]以上の速さで近づいている場合
			this->TargetRelVelNormal = -RelVelNormal * this->e; //衝突後の目標相対速度は衝突前の近づく速度に反発係数を掛けた分だけ離れるものとなる
		else
			this->TargetRelVelNormal = 0;

		return;
	}

	//拘束解決の事前処理
	void Solver::PreSolve()
	{
		const std::shared_ptr<Body> spBodyObj1 = Contact.Pair.wpBodyObj[0].lock();
		const std::shared_ptr<Body> spBodyObj2 = Contact.Pair.wpBodyObj[1].lock();

		this->constant_a_normal = Get_constant_a(Contact.vecN); //法線方向の定数aを計算

		//剛体に前回最後に求まった力積を加える
		//this->TotalForceNormal = 0;
		spBodyObj1->ApplyForce(this->Contact.vecN * this->TotalForceNormal, this->Contact.vecWorldPos - spBodyObj1->getvecPos()); //法線方向の力を剛体1に加える
		spBodyObj2->ApplyForce(-this->Contact.vecN * this->TotalForceNormal, this->Contact.vecWorldPos - spBodyObj2->getvecPos()); //剛体2には反作用の力を加える

		//摩擦力は0にする
		this->TotalForceTangent = 0;

		return;
	}

	//速度成分について拘束解決を実行
	void Solver::SolveVelocity()
	{
		const std::shared_ptr<Body> spBodyObj1 = Contact.Pair.wpBodyObj[0].lock();
		const std::shared_ptr<Body> spBodyObj2 = Contact.Pair.wpBodyObj[1].lock();

		vec3d vecRelVel = Body::GetRelativeVelocity(Contact.Pair.wpBodyObj[0], Contact.Pair.wpBodyObj[1], Contact.vecWorldPos); //接触位置における物体間の相対速度
		double RelVelNormal = vec3d::GetDot(vecRelVel, Contact.vecN); //相対速度の法線方向成分を内積により計算

		const double SOR_omega = 1.0; //SOR法による加速係数ω

		//法線方向
		{
			//法線方向の力積を計算
			double ForceNormal = (this->TargetRelVelNormal - RelVelNormal) * this->constant_a_normal; //((理想相対速度N - 現在の相対速度N) * 適正質量N)を力積Nとする
			ForceNormal *= SOR_omega; //SOR法により加速
			ForceNormal = ho::num::Max(this->TotalForceNormal + ForceNormal, 0.0) - this->TotalForceNormal;
			this->TotalForceNormal += ForceNormal;
#ifdef _DEBUG
			this->vectorTotalForceNormal.push_back(this->TotalForceNormal);
#endif

			//剛体に力積を加える
			spBodyObj1->ApplyForce(Contact.vecN * ForceNormal, Contact.vecWorldPos - spBodyObj1->getvecPos()); //法線方向の力を剛体1に加える
			spBodyObj2->ApplyForce(-Contact.vecN * ForceNormal, Contact.vecWorldPos - spBodyObj2->getvecPos()); //剛体2には反作用の力を加える
		}

		//接線方向
		{
			vec3d vecRelVelTanjent = vecRelVel - Contact.vecN * RelVelNormal; //相対速度の接線方向成分を、[相対速度-法線方向成分]で計算
			double constant_a_tanjent = Get_constant_a(vecRelVelTanjent.GetNormalize()); //接線方向の定数aを計算

			//(-現在の相対速度T * 適正質量T)で完全にずれを打ち消す力積T
			//しかし摩擦力には上限があるので(力積N * 摩擦係数)でクランプ
			double ForceTanjent = -vecRelVelTanjent.GetPower() * constant_a_tanjent;
			ForceTanjent = ho::num::Clamp(this->TotalForceTangent + ForceTanjent, -this->TotalForceNormal * mu, this->TotalForceNormal * mu) - this->TotalForceTangent;
			this->TotalForceTangent += ForceTanjent;

			//剛体に摩擦による力積を加える
			spBodyObj1->ApplyForce(vecRelVelTanjent.GetNormalize() * ForceTanjent, Contact.vecWorldPos - spBodyObj1->getvecPos()); //接線方向の力を剛体1に加える
			spBodyObj2->ApplyForce(-vecRelVelTanjent.GetNormalize() * ForceTanjent, Contact.vecWorldPos - spBodyObj2->getvecPos()); //剛体2には反作用の力を加える
		}
		
		return;
	}

	//位置成分について拘束解決を実行
	void Solver::SolvePosition()
	{
		const std::shared_ptr<Body> spBodyObj1 = Contact.Pair.wpBodyObj[0].lock();
		const std::shared_ptr<Body> spBodyObj2 = Contact.Pair.wpBodyObj[1].lock();

		const double constant_a = this->Get_constant_a(Contact.vecN); //定数aを計算

		//めり込み修正に必要な力を計算
		double Force = (Contact.SinkDistance - 0.0001) * constant_a * 0.4; //最後の係数は0.5以下にする
		if (Force < 0)
			Force = 0;

		//剛体に位置修正の力を加える
		spBodyObj1->ApplyCorrect(Contact.vecN * Force, Contact.vecWorldPos - spBodyObj1->getvecPos()); //法線方向の力を剛体1に加える
		spBodyObj2->ApplyCorrect(Contact.vecN * -Force, Contact.vecWorldPos - spBodyObj2->getvecPos()); //法線方向の力を剛体2に加える

		this->wpCollisionObj.lock()->CollisionJudge(this->Contact); //位置修正したので接触情報を更新

		return;
	}

	//定数aを計算
	double Solver::Get_constant_a(const vec3d &vecN) 
	{
		const std::shared_ptr<Body> spBodyObj1 = Contact.Pair.wpBodyObj[0].lock();
		const std::shared_ptr<Body> spBodyObj2 = Contact.Pair.wpBodyObj[1].lock();
		
		//剛体1と2から見た相対位置を計算
		const vec3d vecRelativePos1 = Contact.vecWorldPos - spBodyObj1->getvecPos();
		const vec3d vecRelativePos2 = Contact.vecWorldPos - spBodyObj2->getvecPos();

		// 1/... になっているのは逆数をもとに戻すため。逆数を使うと無限大の質量も表現可能。
		return 1.0 / (spBodyObj1->getMassInv() + spBodyObj2->getMassInv() +
			//慣性モーメント・相対位置・法線から物体の角速度に与える影響を計算
			vec3d::GetDot(vecN, vec3d::GetCross(spBodyObj1->getmtrxInertiaNowInv() * vec3d::GetCross(vecRelativePos1, vecN), vecRelativePos1)) +
			vec3d::GetDot(vecN, vec3d::GetCross(spBodyObj2->getmtrxInertiaNowInv() * vec3d::GetCross(vecRelativePos2, vecN), vecRelativePos2))
			);
	}
}