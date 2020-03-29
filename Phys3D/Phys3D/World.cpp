#include "World.h"
#include "Body.h"
#include "Collision.h"
#include "Contact.h"
#include <algorithm>
#include "Phys3D.h"
#include "Solver.h"

using namespace ho::num;

namespace Phys3D
{
	//コンストラクタ
	World::World(const std::weak_ptr<Phys3D> &wpPhys3DObj, const double &dt, const vec3d &vecGravity)
		: wpPhys3DObj(wpPhys3DObj), dt(dt), vecGravity(vecGravity)
	{
		this->wpThisObj = this->sp_this;
		this->FrameFlag = 0;
	}

	//デストラクタ
	World::~World()
	{
	}

	//1フレーム毎の処理
	void World::Update() 
	{
		Delete(); //エンジン外部で参照されていない物体を削除する
		ApplyGravity(); //全ての剛体に重力加速度をかける
	
		std::list<CONTACT> listContact; //接触情報リスト
		this->wpPhys3DObj.lock()->getspCollisionObj()->DetectContact(listContact, this->listspBodyObj); //全ての物体から接触情報を取得

		CreateSolver(listContact); //接触情報からソルバー生成
		Solve(); //拘束解決を行う

		Move(); //全ての剛体を移動

		FrameFlag = 1 - FrameFlag;

		return;
	}

	//作成済みの物体オブジェクトを追加する
	void World::AddspBodyObj(const std::shared_ptr<Body> &spBodyObj) 
	{
		spBodyObj->setwpWorldObj(this->wpThisObj);
		this->listspBodyObj.push_back(spBodyObj);

		return;
	}

	//エンジン外部で参照されていない物体を削除する
	void World::Delete()
	{
		for (std::list<std::shared_ptr<Body>>::iterator itr = this->listspBodyObj.begin(); itr != this->listspBodyObj.end();)
		{
			if (itr->use_count() == 1)
				itr = this->listspBodyObj.erase(itr);
			else
				itr++;
		}

		return;
	}

	//全ての剛体に重力加速度をかける
	void World::ApplyGravity() 
	{
		const vec3d vecGravity = this->vecGravity * this->dt;

		std::for_each(this->listspBodyObj.begin(), this->listspBodyObj.end(), [&](const std::shared_ptr<Body> &spBodyObj)
		{
			spBodyObj->ApplyGravity(vecGravity);
		});

		return;
	}

	//接触情報からソルバーを生成
	void World::CreateSolver(std::list<CONTACT> listContact)
	{
		this->listspSolverObj[this->FrameFlag].clear(); //ソルバーをクリア

		for (std::list<std::shared_ptr<Solver>>::iterator itr1 = this->listspSolverObj[1 - this->FrameFlag].begin(); itr1 != this->listspSolverObj[1 - this->FrameFlag].end(); itr1++) //1フレーム前のソルバーリストを走査
		{
			for (std::list<CONTACT>::iterator itr2 = listContact.begin(); itr2 != listContact.end(); itr2++) //接触リストを走査
			{
				if ((*itr1)->getContact().Pair == itr2->Pair) //物体の組み合わせが同じ場合
				{
					(*itr1)->Continue(*itr2); //接触継続のため、接触情報を更新
					this->listspSolverObj[this->FrameFlag].push_back(*itr1); //現在のフレームのリストにコピー

					//ソルバーが継続された接触オブジェクトは削除
					itr2 = listContact.erase(itr2);
					break;
				}
			}
		}

		const std::weak_ptr<Collision> wpCollisionObj = this->wpPhys3DObj.lock()->getspCollisionObj();

		for (std::list<CONTACT>::const_iterator itr = listContact.begin(); itr != listContact.end(); itr++) //接触リストを走査
			this->listspSolverObj[this->FrameFlag].push_back(std::shared_ptr<Solver>(new Solver(*itr, wpCollisionObj))); //接触情報からソルバーを生成してリストに追加

		return;
	}

	//拘束解決を行う
	void World::Solve()
	{
		const int SolveNum = this->wpPhys3DObj.lock()->getSolveNum(); //反復回数

		for (std::list<std::shared_ptr<Solver>>::iterator itr = this->listspSolverObj[this->FrameFlag].begin(); itr != this->listspSolverObj[this->FrameFlag].end(); itr++) //現在のフレームのソルバーを走査
			(*itr)->PreSolve(); //拘束解決の事前処理

		for (int i = 0; i < SolveNum; i++)
			for (std::list<std::shared_ptr<Solver>>::iterator itr = this->listspSolverObj[this->FrameFlag].begin(); itr != this->listspSolverObj[this->FrameFlag].end(); itr++) //現在のフレームのソルバーを走査
				(*itr)->SolveVelocity(); //速度成分を拘束解決

		for (int i = 0; i < 1; i++)
			for (std::list<std::shared_ptr<Solver>>::iterator itr = this->listspSolverObj[this->FrameFlag].begin(); itr != this->listspSolverObj[this->FrameFlag].end(); itr++) //現在のフレームのソルバーを走査
				(*itr)->SolvePosition(); //位置成分を拘束解決

		return;
	}

	//全ての剛体を移動
	void World::Move() 
	{
		std::for_each(this->listspBodyObj.begin(), this->listspBodyObj.end(), [&](const std::shared_ptr<Body> &spBodyObj)
		{
			spBodyObj->Move(dt);
		});
		
		return;
	}
}