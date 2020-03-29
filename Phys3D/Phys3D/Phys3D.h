//物理演算エンジン Phys3D オブジェクト
#pragma once
#include "../hoLib/num/vector.h"
#include "../hoLib/num/Quaternion.h"
#include <memory>
#include "MassPoint.h"
#include "../hoLib/sp_init.hpp"

using namespace ho::num;

namespace Phys3D
{
	class World;
	class BodyCreator;
	class Body;
	class Collision;

	class Phys3D : public ho::sp_base<Phys3D>
	{
	public:
		Phys3D(const double &dt = 1.0 / 60.0, const int SolveNum = 10); //コンストラクタ
		~Phys3D(); //デストラクタ

		void Update(); //1フレーム毎の処理
		std::shared_ptr<Body> CreateBody(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const std::vector<MASSPOINT> &vectorMassPoint, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vecVtIndex, const double &e, const double &mu, const bool &Fixed); //物体を生成する
		std::shared_ptr<Body> CreateBody(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const double &Density, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vecVtIndex, const double &e, const double &mu, const bool &Fixed); //物体を生成する

		//アクセッサ
		const std::shared_ptr<Collision> &getspCollisionObj() const { return spCollisionObj; }
		const int getSolveNum() const { return SolveNum; }
	private:
		double dt; //刻み時間t [s] （60FPSなら1/60を入れる）
		int SolveNum; //拘束解決を行う回数
		bool WarmStart; //WarmStartを使うかどうか
		std::shared_ptr<BodyCreator> spBodyCreatorObj; //物体生成オブジェクト
		std::shared_ptr<Collision> spCollisionObj; //衝突検出オブジェクト
		std::shared_ptr<World> spWorldObj; //世界オブジェクト
	};
}

/*物理演算エンジンを利用するプログラム内からは、このクラスのオブジェクトを最初に
1つだけ生成する。使わなくなった終了時には破棄する。

世界に剛体を追加するには、Phys3D::CreateBody() を呼び出す。
Phys3D::CreateBody() の戻り値のスマートポインタをエンジン外部で保持する。
エンジン外部でスマートポインタが解放されると、エンジン内部でも自動的に物体が削除される。
*/