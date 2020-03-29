//接触情報クラス
#pragma once
#include "../hoLib/num/vector.h"
#include <vector>
#include <memory>
#include "Contact.h"
using namespace ho::num;

namespace Phys3D
{
	class Body;
	class Collision;

	class Solver
	{
	public:
		Solver(const CONTACT &Contact, const std::weak_ptr<Collision> &wpCollisionObj); //コンストラクタ
		~Solver(); //デストラクタ

		void Continue(const CONTACT &Contact); //接触継続のため、接触情報を更新

		void PreSolve(); //拘束解決の事前処理
		void SolveVelocity(); //速度成分について拘束解決を実行
		void SolvePosition(); //位置成分について拘束解決を実行

		//アクセッサ
		const CONTACT &getContact() const { return Contact; }
	private:
		const std::weak_ptr<Collision> wpCollisionObj;
		CONTACT Contact; //接触情報
		double TargetRelVelNormal; //法線方向の目標相対速度
		double TotalForceNormal; //法線方向の力の合計
		double TotalForceTangent; //接線方向の力の合計
		double constant_a_normal; //法線方向の定数a
		double e; //反発係数
		double mu; //摩擦係数

#ifdef _DEBUG
		std::vector<double> vectorTotalForceNormal; //法線方向の力の反復履歴
#endif
		double Get_constant_a(const vec3d &vecN); //定数aを計算
	};
}
