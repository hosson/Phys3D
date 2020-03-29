//衝突判定クラス
#pragma once
#include <list>
#include <vector>
#include "../hoLib/num/vector.h"
#include "Collision_base.h"
#include "Contact.h"

using namespace ho::num;

namespace Phys3D
{

	class Mesh;
	class Face;
	class Edge;
	class Vertex;
	class Body;

	class Collision
	{
	public:
		struct CrossPoint //面と辺との交点情報
		{
			vec3d vecPos; //交差座標
			Face *pFaceObj;
			Edge *pEdgeObj;
			Vertex *pVertexObj;
		};
		struct TriangleCollision //3角形の面どうしの衝突判定の結果
		{
			CrossPoint CrossPt[2]; //交点情報
			Face *pFaceObj[2]; //衝突判定された2つの面
		};
		struct CommonPoint //接触交点と頂点オブジェクトの共通の点情報
		{
			Vertex *pVertexObj; //頂点オブジェクト
			CrossPoint CrossPoint; //接触交点
		};
	public:
		Collision(); //コンストラクタ
		~Collision(); //デストラクタ

		void DetectContact(std::list<CONTACT> &listContact, const std::list<std::shared_ptr<Body>> &listspBodyObj); //全ての物体から接触情報を取得
		bool CollisionJudge(CONTACT &Contact); //剛体同士の衝突判定
	private:
		void GetPair(std::list<PAIR> &listPair, const std::list<std::shared_ptr<Body>> &listspBodyObj); //接触可能性のあるペアを取得する
		bool GetSinkMesh(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const std::weak_ptr<Mesh> &wpMeshObj1, const std::weak_ptr<Mesh> &wpMeshObj2); //2つのメッシュを衝突判定し衝突部分のメッシュを得る
		void GetSinkMeshFace(const std::shared_ptr<Mesh> &spMeshObj, const std::shared_ptr<Face> &spFaceObj, std::list<vec3d> &listvecCrossPoint); //衝突線分で分割し、めり込み部分のメッシュを構成する面を得る
		void CreateOutLine(std::list<vec3d> &listvecCrossPoint, const vec3d &vecFaceN); //頂点リストから外側の辺を構成する頂点だけを残し、残りを削除する
		void DividePolygonIntoTriangles(const std::shared_ptr<Mesh> &spMeshObj, const std::list<vec3d> &listvecCrossPointj); //多角形を複数の三角形の面に分割
		vec3d CalcvecNFromSinkMesh(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj); //接触メッシュより法線ベクトルを計算する
		vec3d CalcHitPos(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj); //接触メッシュより接触位置を計算する
		double CalcSinkDistance(const std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const vec3d &vecHitPos, const vec3d &vecN); //接触メッシュと衝突位置と衝突法線より、めり込み距離を計算する
		void GetSubPos(std::vector<vec3d> &vectorvecSubPos, const std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const vec3d &vecHitPos, const vec3d &vecM); //サブソルバー用の接触位置を取得
	};
}
