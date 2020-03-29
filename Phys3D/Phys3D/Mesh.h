#pragma once
#include <list>
#include <vector>
#include "../hoLib/num/vector.h"
#include "../hoLib/num/Matrix.h"
#include "../hoLib/num/Quaternion.h"
#include <memory>
#include <boost/optional.hpp>
#include "../hoLib/sp_init.hpp"

namespace Phys3D
{
	using namespace ::ho::num;

	class Vertex;
	class Edge;
	class Face;

	//メッシュクラス
	class Mesh : public ho::sp_base<Mesh>
	{
	public:
		Mesh(); //コンストラクタ
		Mesh(const std::vector<vec3d> &vectorvecVt, const std::vector<int> &vectorVtEdgeIndex, const std::vector<int> &vectorVtFaceIndex); //コンストラクタ
		~Mesh(); //デストラクタ

		void UpdateVertexWorldPos(const Quaternion &qt, const vec3d &vecWorldPos); //全ての頂点のワールド座標を現在の位置と姿勢のものに更新
		double GetBoundingRadius(); //境界球半径を取得

		void AddFace(const std::vector<vec3d> &vectorvecVt); //3つの頂点情報から、頂点、辺、面を追加する

		//アクセッサ
		const std::vector<std::shared_ptr<Vertex>> &getvectorspVertexObj() const { return vectorspVertexObj; }
		const std::vector<std::shared_ptr<Edge>> &getvectorspEdgeObj() const { return vectorspEdgeObj; }
		const std::vector<std::shared_ptr<Face>> &getvectorspFaceObj() const { return vectorspFaceObj; }
	private:
		std::weak_ptr<Mesh> wpThisObj;
		std::vector<std::shared_ptr<Vertex>> vectorspVertexObj;
		std::vector<std::shared_ptr<Edge>> vectorspEdgeObj;
		std::vector<std::shared_ptr<Face>> vectorspFaceObj;

		void Merge(); //頂点と辺と面の共通関係を形成
	};

	//頂点クラス
	class Vertex : public ho::sp_base<Vertex>
	{
	public:
		Vertex(const vec3d &vecLocalPos, const int &ID); //コンストラクタ
		Vertex(const Vertex &obj); //コピーコンストラクタ
		~Vertex(); //デストラクタ

		bool AddwpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj, const boost::optional<vec3d> &vecN = boost::optional<vec3d>(boost::none)); //辺オブジェクトを追加
		void AddwpFaceObj(const std::weak_ptr<Face> &wpFaceObj); //面オブジェクトを追加
		void UpdatevecPos(const Quaternion &qt, const vec3d &vecWorldPos); //座標をワールド座標に変換し、ローカル座標を退避領域にコピー
		void RestoreLocalPos(); //座標をローカル座標に戻す
		void ErasewpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj); //辺オブジェクトを削除
		std::weak_ptr<Edge> GetwpCounterClockwiseEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj); //接続された任意の辺の反時計回り方向の隣に接続されている辺を取得

		static void Merge(const std::weak_ptr<Vertex> &wpAbsorbObj, const std::weak_ptr<Vertex> &wpEraseObj); //2つの頂点を1つに吸收する。wpEraseObjが消えその内容がwpAbsorbObjに追加される。

		//アクセッサ
		const vec3d &getvecLocalPos() const { return vecLocalPos; }
		const vec3d &getvecWorldPos() const { return vecWorldPos; }
		const int &getID() const { return ID; }
		std::vector<std::weak_ptr<Edge>> &getvectorwpEdgeObj() { return vectorwpEdgeObj; }
		std::vector<std::weak_ptr<Face>> &getvectorwpFaceObj() { return vectorwpFaceObj; }
	private:
		std::weak_ptr<Vertex> wpThisObj;
		vec3d vecLocalPos; //ローカル座標
		vec3d vecWorldPos; //ワールド座標
		int ID;
		std::vector<std::weak_ptr<Face>> vectorwpFaceObj;
		std::vector<std::weak_ptr<Edge>> vectorwpEdgeObj;
	};

	//辺クラス
	class Edge : public ho::sp_base<Edge>
	{
	public:
		Edge(const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2, const int &ID, const boost::optional<vec3d> &vecN = boost::optional<vec3d>(boost::none)); //コンストラクタ
		//Edge(const Edge &obj); //コピーコンストラクタ
		~Edge(); //デストラクタ

		void Init(); //初期化

		void AddwpFaceObj(const std::weak_ptr<Face> &wpFaceObj); //面を追加する
		void ExchangewpVertexObj(const std::weak_ptr<Vertex> &wpAddVertexObj, const std::weak_ptr<Vertex> &wpEraseVertexObj); //接続先の頂点オブジェクトを入れ替える
		std::weak_ptr<Vertex> GetwpOppositeVertexObj(const std::weak_ptr<Vertex> &wpVertexObj); //もう一方に接続された頂点を取得

		//アクセッサ
		const std::weak_ptr<Vertex> &getwpVertexObj1() const { return wpVertexObj[0]; }
		const std::weak_ptr<Vertex> &getwpVertexObj2() const { return wpVertexObj[1]; }
		const std::vector<std::weak_ptr<Face>> &getvectorwpFaceObj() const { return vectorwpFaceObj; }
		const int &getID() const { return ID; }
		void setID(const int &id) { this->ID = id; }
	private:
		std::weak_ptr<Edge> wpThisObj;
		std::weak_ptr<Vertex> wpVertexObj[2];
		std::vector<std::weak_ptr<Face>> vectorwpFaceObj;
		int ID;
	};

	//面クラス
	class Face : public ho::sp_base<Face>
	{
	public:
		Face(); //コンストラクタ
		Face(const std::weak_ptr<Mesh> &wpMeshObj, const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2, const std::weak_ptr<Vertex> &wpVertexObj3, const int &ID); //コンストラクタ
		//Face(const std::list<std::weak_ptr<Vertex>> &listwpVertexObj, const std::list<std::weak_ptr<Edge>> &listwpEdgeObj, const int &ID); //コンストラクタ（既に作成された頂点と辺から作成）
		//Face(const std::vector<std::weak_ptr<Vertex>> &vectorwpVertexObj, const std::vector<std::weak_ptr<Edge>> &vectorwpEdgeObj, const int &ID); //コンストラクタ（既に作成された頂点と辺の配列から三角形を作成）
		//Face(const Face &obj); //コピーコンストラクタ
		~Face(); //デストラクタ

		void Init(const std::weak_ptr<Face> &wpFaceObj); //初期化

		void AddwpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj); //辺を追加する
		void AddwpVertexObj(const std::weak_ptr<Vertex> &wpVertexObj); //頂点を追加する
		void ErasewpVertexObj(const std::weak_ptr<Vertex> &wpVertexObj); //登録されている頂点を削除
		bool SearchVertices(const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2); //2つの頂点オブジェクトを持っているかどうかを判定
		//std::shared_ptr<Face> Duplicate(); //複製する（頂点や辺も作りなおす）
		void ErasewpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj); //登録されている辺を削除
		double CalcArea(); //面積を計算する
		double CalcVolume(const vec3d &vecCenter); //ある中心座標との4点の体積を求める
		vec3d CalcCentroid(const vec3d &vecCenter); //ある中心座標との4点の平均（重心）を求める
		bool CalcDistance(double &OutDistance, const vec3d &vecPos, const vec3d &vecDirection); //任意の座標から任意の方向へ向かう直線との交点を求め、求まった場合は任意の座標からの距離を返す
		std::weak_ptr<Edge> GetwpEdgeObj(const int VertexIndex1, const int VertexIndex2); //2つの頂点のインデックスから辺オブジェクトを得る
		void DeleteVertexAndEdge(); //所属する頂点と辺のオブジェクトを削除
		void CalcvecLocalNormal(); //ローカル座標の法線ベクトルを計算する
		void CalcvecWorldNormal(); //ワールド座標の法線ベクトルを計算する

		//アクセッサ
		const std::weak_ptr<Mesh> &getwpMeshObj() const { return wpMeshObj; }
		std::vector<std::weak_ptr<Vertex>> &getvectorwpVertexObj() { return vectorwpVertexObj; }
		std::vector<std::weak_ptr<Edge>> &getvectorwpEdgeObj() { return vectorwpEdgeObj; }
		const int &getID() const { return ID; }
		const vec3d &getvecLocalNormal() const { return vecLocalNormal; }
		const vec3d &getvecWorldNormal() const { return vecWorldNormal; }
	private:
		std::weak_ptr<Mesh> wpMeshObj;
		std::vector<std::weak_ptr<Vertex>> vectorwpVertexObj;
		std::vector<std::weak_ptr<Edge>> vectorwpEdgeObj;
		int ID; //メッシュから見た通し番号
		vec3d vecLocalNormal, vecWorldNormal; //法線ベクトル
	};
}