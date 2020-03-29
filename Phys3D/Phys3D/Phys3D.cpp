#include "Phys3D.h"
#include "World.h"
#include "BodyCreator.h"
#include "../hoLib/num/vector.h"
#include "Collision.h"

using namespace ho::num;

namespace Phys3D
{
	//コンストラクタ
	Phys3D::Phys3D(const double &dt, const int SolveNum)
	: dt(dt), SolveNum(SolveNum)
	{
		this->spBodyCreatorObj = std::shared_ptr<BodyCreator>(new BodyCreator()); //物体生成オブジェクトを生成
		this->spCollisionObj = std::make_shared<Collision>(); //衝突検出オブジェクトを生成
		this->spWorldObj = ho::sp_init<World>(new World(this->sp_this, dt, vec3d(0, -9.8, 0))); //世界クラスオブジェクトを作成
		this->WarmStart = true;
	}

	//デストラクタ
	Phys3D::~Phys3D() 
	{}

	//1フレーム毎の処理
	void Phys3D::Update()
	{
		this->spWorldObj->Update(); //世界クラスオブジェクトの1フレーム毎の処理

		return;
	}

	//物体を生成する
	std::shared_ptr<Body> Phys3D::CreateBody(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const std::vector<MASSPOINT> &vectorMassPoint, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vecVtIndex, const double &e, const double &mu, const bool &Fixed)
	{
		std::shared_ptr<Body> spBodyObj = this->spBodyCreatorObj->CreateBody(vecPos, vecLinearVelocity, qtAngle, vecL, vectorMassPoint, vectorvecVertex, vecVtIndex, e, mu, Fixed);
		this->spWorldObj->AddspBodyObj(spBodyObj);

		return spBodyObj;
	}

	//物体を生成する
	std::shared_ptr<Body> Phys3D::CreateBody(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const double &Density, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vecVtIndex, const double &e, const double &mu, const bool &Fixed)
	{
		std::shared_ptr<Body> spBodyObj = this->spBodyCreatorObj->CreateBody(vecPos, vecLinearVelocity, qtAngle, vecL, Density, vectorvecVertex, vecVtIndex, e, mu, Fixed);
		this->spWorldObj->AddspBodyObj(spBodyObj);

		return spBodyObj;
	}
}
