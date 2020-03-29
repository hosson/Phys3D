//世界クラス
#pragma once
#include "../hoLib/num/vector.h"
#include <map>
#include <list>
#include <memory>
#include "../hoLib/sp_init.hpp"
#include "Contact.h"

using namespace ho::num;

namespace Phys3D
{
	class Phys3D;
	class Body;
	class Collision;
	class Solver;

	class World : public ho::sp_base<World>
	{
	public:
		World(const std::weak_ptr<Phys3D> &wpPhys3DObj, const double &dt, const vec3d &vecGravity); //コンストラクタ
		~World(); //デストラクタ

		void Update(); //1フレーム毎の処理
		void AddspBodyObj(const std::shared_ptr<Body> &spBodyObj); //作成済みの物体オブジェクトを追加する

		//アクセッサ
		const int &getFrame() const { return Frame; }
	private:
		std::weak_ptr<World> wpThisObj;
		const std::weak_ptr<Phys3D> wpPhys3DObj;
		double dt; //刻み時間t[秒]
		vec3d vecGravity; //重力加速度[m/s^2] （通常の地球上なら (0, -9.80665) を入れる）
		std::list<std::shared_ptr<Body>> listspBodyObj; //物体オブジェクトリスト
		std::list<std::shared_ptr<Solver>> listspSolverObj[2]; //ソルバーリスト
		int FrameFlag; //1フレームごとに0と1が交互に切り替わる
		int Frame; //現在までの経過フレーム数

		void Delete(); //エンジン外部で参照されていない物体を削除する

		void ApplyGravity(); //全ての剛体に重力加速度をかける
		void CreateSolver(std::list<CONTACT> listContact); //接触情報からソルバーを生成
		void Solve(); //拘束解決を行う
		void Move(); //全ての剛体を移動
	};

}

/*Phys3Dクラスは1つまたは複数のWorldクラスオブジェクトを持つ。
複数のWorldクラスオブジェクトを持つと、同時に別々の世界をシミュレーションできる。
例えば場面転換などで画面外から消えた世界をそのまま保持することもできる。*/