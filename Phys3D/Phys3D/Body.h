//剛体クラス
#pragma once
#include <list>
#include <vector>
#include "../hoLib/num/vector.h"
#include "../hoLib/num/Quaternion.h"
#include "../hoLib/num/Matrix.h"
#include <memory>

using namespace ho::num;

namespace Phys3D
{
	class World;
	class Mesh;
	class Contact;

	class Body
	{
	public:
		Body(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const double &Mass, const Matrix &mtrxInertia, const std::shared_ptr<Mesh> &spMeshObj, const double &e, const double &mu, const bool &Fixed); //コンストラクタ
		~Body(); //デストラクタ

		Matrix GetCoordTransMatrix() const; //グラフィックス用の座標変換行列を取得する

		//void GetAllvecVtWorld(std::vector<vec3d> *pvectorvecVtWorld); //すべての頂点座標をワールド座標で取得
		void Move(const double &dt); //移動
		void ApplyGravity(const vec3d &vecGravity); //重力を付加する
		void ApplyForce(const vec3d &vecForce, const vec3d &vecLocalPos); //力積を加える
		void ApplyCorrect(const vec3d &vecForce, const vec3d &vecLocalPos); //位置を動かす力を加える
		void GetlistVtFaceiiFromVtEdgeii(std::list<int> *plistVtFaceii, int VtEdgeii); //ある一辺が含まれる面をすべて列挙する（辺の頂点インデックス配列のインデックスの辺にが含まれる面の頂点インデックス配列中のインデックスを列挙）
		//void GetvectorvecFaceNRotation(std::vector<vec3d> *pvectorvecFaceN); //すべての面の現在の姿勢での法線ベクトルを取得する
		void CalcmtrxInertiaNowInv(); //現在時刻（現在姿勢）での慣性テンソルの逆行列を計算する
		void UpdateMeshVertexWorldPos(); //メッシュのワールド座標を更新する
		void AddpContactObj(Contact *pContactObj); //接触オブジェクトを追加
		bool ErasepContactObj(Contact *pContactObj); //接触オブジェクトを削除

		static vec3d GetRelativeVelocity(const std::weak_ptr<Body> &wpBodyObj1, const std::weak_ptr<Body> &wpBodyObj2, const vec3d &vecWorldPos); //ワールド座標上の一点での剛体間の相対速度を計算

		//アクセッサ
		void setwpWorldObj(const std::weak_ptr<World> &wp) { this->wpWorldObj = wp; }
		const vec3d &getvecPos() const { return vecPos; }
		const vec3d &getvecLinearVelocity() const { return vecLinearVelocity; }
		const Quaternion &getqtAngle() const { return qtAngle; }
		const vec3d &getvecL() const { return vecL; }
		const vec3d &getvecOmega() const { return vecOmega; }
		const double &getMass() const { return Mass; }
		const double &getMassInv() const { return MassInv; }
		const Matrix &getmtrxInertia() const { return mtrxInertia; }
		const Matrix &getmtrxInertiaInv() const { return mtrxInertiaInv; }
		const Matrix &getmtrxInertiaNowInv() const { return mtrxInertiaNowInv; }
		const double &getBoundingRadius() const { return BoundingRadius; }
		const bool &getFixed() const { return Fixed; }
		double CalcInverseInertia(vec3d *pvecAxis);
		std::vector<vec3d> *GetpvectorvecVt() { return &vectorvecVt; }
		std::vector<int> *GetpvectorVtFaceIndex() { return &vectorVtFaceIndex; }
		std::vector<int> *GetpvectorVtEdgeIndex() { return &vectorVtEdgeIndex; }
		int GetFaces() { return Faces; }
		const std::shared_ptr<Mesh> &getspMeshObj() const { return spMeshObj; }
		const double &gete() const { return  e; }
		const double &getmu() const { return mu; }
		//bool GetSleep() { return Sleep; }
	private:
		std::weak_ptr<World> wpWorldObj;
		vec3d vecPos; //ワールド座標上の位置
		vec3d vecLinearVelocity; //平行移動速度
		Quaternion qtAngle; //回転位置
		vec3d vecL; //角運動量
		vec3d vecOmega; //角速度
		double Mass; //重量[Kg]
		double MassInv; //重量の逆数
		Matrix mtrxInertia; //慣性モーメントテンソル
		Matrix mtrxInertiaInv; //慣性モーメントテンソルの逆行列
		Matrix mtrxInertiaNowInv; //現在時刻（現在姿勢）での慣性テンソルの逆行列
		std::shared_ptr<Mesh> spMeshObj; //メッシュオブジェクト
		std::vector<vec3d> vectorvecVt; //頂点座標配列
		std::vector<int> vectorVtFaceIndex; //面の頂点インデックス配列
		std::vector<int> vectorVtEdgeIndex; //辺の頂点インデックス配列
		int Faces; //面の数
		//std::vector<vec3d> vectorvecFaceN; //各面の法線ベクトルの配列
		double e; //反発係数
		double mu; //摩擦係数
		bool Fixed; //固定物かどうか
		double BoundingRadius; //衝突判定用の最長半径
		std::list<Contact *> listpContactObj; //関わっている接触オブジェクトのリスト
		int LastUpdateFrame; //メッシュのワールド頂点座標を最後に更新したフレーム数
	};

}