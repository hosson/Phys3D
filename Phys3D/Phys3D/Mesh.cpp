#include "Mesh.h"
#include "../hoLib/Common.h"
#include "../hoLib/Error.h"
#include "../hoLib/num/constant.h"
using namespace ho::num;

namespace Phys3D
{
	//コンストラクタ
	Mesh::Mesh()
	{
		this->wpThisObj = this->sp_this;
	}

	//コンストラクタ
	Mesh::Mesh(const std::vector<vec3d> &vectorvecVt, const std::vector<int> &vectorVtEdgeIndex, const std::vector<int> &vectorVtFaceIndex)
	{
		this->wpThisObj = this->sp_this;

		//頂点オブジェクトを作成
		for (unsigned int i = 0; i < vectorvecVt.size(); i++)
			this->vectorspVertexObj.push_back(ho::sp_init<Vertex>(new Vertex(vectorvecVt.at(i), i)));

		//辺オブジェクトを作成
		for (unsigned int i = 0; i < vectorVtEdgeIndex.size(); i += 2)
			this->vectorspEdgeObj.push_back(ho::sp_init<Edge>(new Edge(this->vectorspVertexObj.at(vectorVtEdgeIndex.at(i)), vectorspVertexObj.at(vectorVtEdgeIndex.at(i + 1)), i / 2)));

		//面オブジェクトを作成
		for (unsigned int i = 0; i < vectorVtFaceIndex.size(); i += 3)
			this->vectorspFaceObj.push_back(ho::sp_init<Face>(new Face(this->wpThisObj, this->vectorspVertexObj.at(vectorVtFaceIndex.at(i)), vectorspVertexObj.at(vectorVtFaceIndex.at(i + 1)), vectorspVertexObj.at(vectorVtFaceIndex.at(i + 2)), i / 3)));

		Merge(); //頂点と辺と面の共通関係を形成
	}

	//デストラクタ
	Mesh::~Mesh()
	{
	}

	//頂点と辺と面の共通関係を形成
	void Mesh::Merge()
	{
		//辺と面の共通関係を形成
		for (unsigned int i = 0; i < this->vectorspEdgeObj.size(); i++) //辺オブジェクトを走査
		{
			for (unsigned int j = 0; j < this->vectorspFaceObj.size(); j++) //面オブジェクトを走査
			{
				if (this->vectorspFaceObj.at(j)->SearchVertices(this->vectorspEdgeObj.at(i)->getwpVertexObj1(), this->vectorspEdgeObj.at(i)->getwpVertexObj2())) //辺と面で共通の頂点を持っているかを調べ、持っている場合
				{
					this->vectorspEdgeObj.at(i)->AddwpFaceObj(this->vectorspFaceObj.at(j)); //辺に面を追加
					this->vectorspFaceObj.at(j)->AddwpEdgeObj(this->vectorspEdgeObj.at(i)); //面に辺を追加
				}
			}
		}

		return;
	}

	//全ての頂点のワールド座標を現在の位置と姿勢のものに更新
	void Mesh::UpdateVertexWorldPos(const Quaternion &qt, const vec3d &vecWorldPos)
	{
		for (unsigned int i = 0; i < this->vectorspVertexObj.size(); i++)
			this->vectorspVertexObj.at(i)->UpdatevecPos(qt, vecWorldPos);
		for (std::vector<std::shared_ptr<Face>>::iterator itr = this->vectorspFaceObj.begin(); itr != this->vectorspFaceObj.end(); itr++)
			(*itr)->CalcvecWorldNormal(); //ワールド座標系の法線ベクトルを更新

		return;
	}

	//境界球半径を取得
	double Mesh::GetBoundingRadius()
	{
		double MaxRadius = 0.0;
		for (unsigned int i = 0; i < this->vectorspVertexObj.size(); i++)
			if (vectorspVertexObj.at(i)->getvecLocalPos().GetPower() > MaxRadius)
				MaxRadius = vectorspVertexObj.at(i)->getvecLocalPos().GetPower();

		return MaxRadius;
	}

	//3つの頂点情報から、頂点、辺、面を追加する
	void Mesh::AddFace(const std::vector<vec3d> &vectorvecVt)
	{
		//頂点オブジェクトを作成
		const int Vertices = this->vectorspVertexObj.size();
		for (unsigned int i = 0; i < 3; i++)
			this->vectorspVertexObj.push_back(ho::sp_init<Vertex>(new Vertex(vectorvecVt.at(i), Vertices + i)));

		//辺オブジェクトを作成
		const int Edges = this->vectorspEdgeObj.size();
		for (unsigned int i = 0; i < 3; i++)
			this->vectorspEdgeObj.push_back(ho::sp_init<Edge>(new Edge(this->vectorspVertexObj.at(i), this->vectorspVertexObj.at((i + 1) % 3), Edges + i)));

		//面オブジェクトを作成
		const int Faces = this->vectorspFaceObj.size();
		this->vectorspFaceObj.push_back(ho::sp_init<Face>(new Face(this->wpThisObj, this->vectorspVertexObj.at(Vertices), this->vectorspVertexObj.at(Vertices + 1), this->vectorspVertexObj.at(Vertices + 2), Faces)));

		return;
	}




	//コンストラクタ
	Vertex::Vertex(const vec3d &vecLocalPos, const int &ID)
		: vecLocalPos(vecLocalPos), ID(ID)
	{
		this->wpThisObj = this->sp_this;
	}

	//コピーコンストラクタ
	Vertex::Vertex(const Vertex &obj)
	{
		this->vecLocalPos = obj.vecLocalPos;
		this->vecWorldPos = obj.vecWorldPos;
		this->ID = obj.ID;
	}

	//デストラクタ
	Vertex::~Vertex()
	{
	}

	//辺オブジェクトを追加
	bool Vertex::AddwpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj, const boost::optional<vec3d> &vecN)
	{
		if (this->vectorwpEdgeObj.size() < 2) //辺オブジェクトが2個より少ない場合
		{
			this->vectorwpEdgeObj.push_back(wpEdgeObj); //追加
		} else {  //辺オブジェクトが2個以上の場合
			if (vecN) //角度ソートの基準となる面の法線ベクトルが指定されている場合
			{
				vec3d vec[2];
				std::vector<double> vectorAngle(this->vectorwpEdgeObj.size()); //既存の辺との角度
				vec3d vecCross;

				//各辺と追加された辺との時計回りの角度を計算
				int i = 0;
				for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end(); itr++) //既存の辺を走査
				{
					if (auto p = itr->lock())
					{
						auto spEdgeObj = wpEdgeObj.lock();

						if (auto p2 = p->GetwpOppositeVertexObj(this->wpThisObj).lock())
							vec[0] = p2->getvecLocalPos() - this->vecLocalPos; //この頂点から走査中の辺のもう一方の頂点へのベクトル
						if (auto p2 = spEdgeObj->GetwpOppositeVertexObj(this->wpThisObj).lock())
							vec[1] = p2->getvecLocalPos() - this->vecLocalPos; //この頂点から追加される辺のもう一方の頂点へのベクトル
						double Angle;
						vec3d::GetAngle(Angle, vec[0], vec[1]); //ベクトル間の角度を計算
						vecCross = vec3d::GetCross(vec[0], vec[1]); //外積を計算
						if (vec3d::GetDot(vecN.get(), vecCross) < 0) //外積の結果が正の数の場合
							Angle = PI * 2.0 - Angle; //角度は反時計回りで求まっているので、時計回りの角度に変換する
						vectorAngle.at(i++) = Angle;
					}
				}

				//一番角度の大きいもの（追加される辺の一つ時計回りの方向の隣のもの）を探す
				double MaxAngle = 0;
				unsigned int MaxIndex = -1;
				for (unsigned int i = 0; i < vectorAngle.size(); i++)
				{
					if (MaxAngle < vectorAngle.at(i))
					{
						MaxAngle = vectorAngle.at(i);
						MaxIndex = i;
					}
				}

				if (MaxIndex == -1) //インデックスが正しく得られなかった場合
				{
					ERR(true, TEXT("インデックスが正しく求まりませんでした。"), __WFILE__, __LINE__);
					return false;
				}

				//一番角度の大きい辺の手前に挿入
				std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin();
				itr += MaxIndex;
				this->vectorwpEdgeObj.insert(itr, wpEdgeObj);
			}
		}

		return true;
	}

	//面オブジェクトを追加
	void Vertex::AddwpFaceObj(const std::weak_ptr<Face> &wpFaceObj)
	{
		this->vectorwpFaceObj.push_back(wpFaceObj);

		return;
	}

	//座標をワールド座標に変換し、ローカル座標を退避領域にコピー
	void Vertex::UpdatevecPos(const Quaternion &qt, const vec3d &vecWorldPos)
	{
		qt.TransformVector(this->vecWorldPos, this->vecLocalPos); //回転
		this->vecWorldPos += vecWorldPos; //平行移動

		return;
	}

	//座標をローカル座標に戻す
	void Vertex::RestoreLocalPos()
	{
		//vecPos = vecBuffer;

		return;
	}

	//辺オブジェクトを削除
	void Vertex::ErasewpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj)
	{
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end();)
		{
			if ((*itr).lock() == wpEdgeObj.lock())
				itr = this->vectorwpEdgeObj.erase(itr);
			else
				itr++;
		}

		return;
	}

	//接続された任意の辺の反時計回り方向の隣に接続されている辺を取得
	std::weak_ptr<Edge> Vertex::GetwpCounterClockwiseEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj)
	{
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end(); itr++)
		{
			if (itr->lock() == wpEdgeObj.lock())
			{
				if (itr == this->vectorwpEdgeObj.begin())
				{
					itr = this->vectorwpEdgeObj.end();
					itr--;
					return *itr;
				} else {
					itr--;
					return *itr;
				}
			}
		}

		return std::weak_ptr<Edge>();
	}

	//2つの頂点を1つに吸收する。wpEraseObjが消えその内容がwpAbsorbObjに追加される。
	void Vertex::Merge(const std::weak_ptr<Vertex> &wpAbsorbObj, const std::weak_ptr<Vertex> &wpEraseObj)
	{
		bool Add;

		const std::shared_ptr<Vertex> spAbsorbObj = wpAbsorbObj.lock();
		const std::shared_ptr<Vertex> spEraseObj = wpEraseObj.lock();

		//辺オブジェクトの吸收
		for (std::vector<std::weak_ptr<Edge>>::const_iterator itr1 = spEraseObj->getvectorwpEdgeObj().begin(); itr1 != spEraseObj->getvectorwpEdgeObj().end(); itr1++) //消去される頂点側に接続されている辺オブジェクトを走査
		{
			Add = true; //追加するかどうか
			for (std::vector<std::weak_ptr<Edge>>::const_iterator itr2 = spAbsorbObj->getvectorwpEdgeObj().begin(); itr2 != spAbsorbObj->getvectorwpEdgeObj().end(); itr2++) //吸收される頂点側に接続されている辺オブジェクトを走査
			{
				if (itr1->lock() == itr2->lock()) //吸收される側の頂点にすでに接続されている場合
				{
					Add = false;
					break;
				}
			}
			if (Add) //吸收する場合
			{
				if (auto p = itr1->lock())
				{
					p->ExchangewpVertexObj(wpAbsorbObj, wpEraseObj); //辺の接続先の頂点を入れ替える
					spAbsorbObj->getvectorwpEdgeObj().push_back(p); //吸收される頂点側にも辺を追加
				}
			}
		}

		//面オブジェクトの吸收
		for (std::vector<std::weak_ptr<Face>>::const_iterator itr1 = spEraseObj->getvectorwpFaceObj().begin(); itr1 != spEraseObj->getvectorwpFaceObj().end(); itr1++) //消去される頂点側に接続されている辺オブジェクトを走査
		{
			Add = true; //追加するかどうか
			for (std::vector<std::weak_ptr<Face>>::const_iterator itr2 = spAbsorbObj->getvectorwpFaceObj().begin(); itr2 != spAbsorbObj->getvectorwpFaceObj().end(); itr2++) //吸收される頂点側に接続されている辺オブジェクトを走査
			{
				if (itr1->lock() == itr2->lock()) //吸收される側の頂点にすでに接続されている場合
				{
					Add = false;
					break;
				}
			}
			if (Add) //吸收する場合
			{
				if (auto p = itr1->lock())
				{
					p->ErasewpVertexObj(wpEraseObj); //面から削除される頂点を削除
					p->AddwpVertexObj(wpAbsorbObj); //面へ追加される頂点を追加
					spAbsorbObj->getvectorwpFaceObj().push_back(p); //吸收される頂点側にも辺を追加
				}
			}
		}

		return;
	}














	//コンストラクタ
	Edge::Edge(const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2, const int &ID, const boost::optional<vec3d> &vecN)
	{
		this->wpThisObj = this->sp_this;
		this->ID = ID;

		this->wpVertexObj[0] = wpVertexObj1;
		this->wpVertexObj[1] = wpVertexObj2;

		const std::shared_ptr<Vertex> spVertexObj1 = wpVertexObj1.lock();
		const std::shared_ptr<Vertex> spVertexObj2 = wpVertexObj2.lock();

		spVertexObj1->AddwpEdgeObj(wpThisObj, vecN);
		spVertexObj2->AddwpEdgeObj(wpThisObj, vecN);

	}

	/*
	//コピーコンストラクタ
	Edge::Edge(const Edge &obj)
	{
	}
	*/

	//デストラクタ
	Edge::~Edge()
	{
	}

	//面を追加する
	void Edge::AddwpFaceObj(const std::weak_ptr<Face> &wpFaceObj)
	{
		this->vectorwpFaceObj.push_back(wpFaceObj);

		return;
	}

	//接続先の頂点オブジェクトを入れ替える
	void Edge::ExchangewpVertexObj(const std::weak_ptr<Vertex> &wpAddVertexObj, const std::weak_ptr<Vertex> &wpEraseVertexObj)
	{
		for (int i = 0; i < 2; i++)
			if (this->wpVertexObj[i].lock() == wpEraseVertexObj.lock())
				this->wpVertexObj[i] = wpAddVertexObj;

		return;
	}

	//もう一方に接続された頂点を取得
	std::weak_ptr<Vertex> Edge::GetwpOppositeVertexObj(const std::weak_ptr<Vertex> &wpVertexObj)
	{
		if (this->wpVertexObj[0].lock() == wpVertexObj.lock())
			return this->wpVertexObj[1];
		else if (this->wpVertexObj[1].lock() == wpVertexObj.lock())
			return this->wpVertexObj[0];

		return std::weak_ptr<Vertex>();
	}












	//コンストラクタ
	Face::Face()
	{
		this->ID = 0;
	}

	//コンストラクタ
	Face::Face(const std::weak_ptr<Mesh> &wpMeshObj, const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2, const std::weak_ptr<Vertex> &wpVertexObj3, const int &ID)
	{
		this->wpMeshObj = wpMeshObj;
		this->vectorwpVertexObj.push_back(wpVertexObj1);
		this->vectorwpVertexObj.push_back(wpVertexObj2);
		this->vectorwpVertexObj.push_back(wpVertexObj3);
		this->ID = ID;

		wpVertexObj1.lock()->AddwpFaceObj(this->sp_this);
		wpVertexObj2.lock()->AddwpFaceObj(this->sp_this);
		wpVertexObj3.lock()->AddwpFaceObj(this->sp_this);

		this->CalcvecLocalNormal();
	}

	/*
	//コンストラクタ（既に作成された頂点と辺から作成）
	Face::Face(const std::list<std::weak_ptr<Vertex>> &listwpVertexObj, const std::list<std::weak_ptr<Edge>> &listwpEdgeObj, const int &ID)
	{
		this->vectorwpVertexObj.resize(listwpVertexObj.size());
		int i = 0;
		for (std::list<std::weak_ptr<Vertex>>::const_iterator itr = listwpVertexObj.begin(); itr != listwpVertexObj.end(); itr++)
			this->vectorwpVertexObj.at(i++) = *itr;

		this->vectorwpEdgeObj.resize(listwpEdgeObj.size());
		i = 0;
		for (std::list<std::weak_ptr<Edge>>::const_iterator itr = listwpEdgeObj.begin(); itr != listwpEdgeObj.end(); itr++)
			this->vectorwpEdgeObj.at(i++) = *itr;

		this->ID = ID;
	}

	//コンストラクタ（既に作成された頂点と辺の配列から三角形を作成）
	Face::Face(const std::vector<std::weak_ptr<Vertex>> &vectorwpVertexObj, const std::vector<std::weak_ptr<Edge>> &vectorwpEdgeObj, const int &ID)
	{
		this->vectorwpVertexObj.resize(3);
		this->vectorwpEdgeObj.resize(3);
		for (int i = 0; i < 3; i++)
		{
			this->vectorwpVertexObj.at(i) = vectorwpVertexObj.at(i);
			this->vectorwpEdgeObj.at(i) = vectorwpEdgeObj.at(i);
		}

		this->ID = ID;
	}
	*/

	/*
	//コピーコンストラクタ
	Face::Face(const Face &obj)
	{
	}
	*/

	//デストラクタ
	Face::~Face()
	{
	}

	//辺を追加する
	void Face::AddwpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj)
	{
		this->vectorwpEdgeObj.push_back(wpEdgeObj);

		return;
	}

	//頂点を追加する
	void Face::AddwpVertexObj(const std::weak_ptr<Vertex> &wpVertexObj)
	{
		this->vectorwpVertexObj.push_back(wpVertexObj);

		return;
	}

	//登録されている頂点を削除
	void Face::ErasewpVertexObj(const std::weak_ptr<Vertex> &wpVertexObj)
	{
		for (std::vector<std::weak_ptr<Vertex>>::iterator itr =this-> vectorwpVertexObj.begin(); itr != this->vectorwpVertexObj.end();)
		{
			if (itr->lock() == wpVertexObj.lock())
				itr = this->vectorwpVertexObj.erase(itr);
			else
				itr++;
		}

		return;
	}

	//2つの頂点オブジェクトを持っているかどうかを判定
	bool Face::SearchVertices(const std::weak_ptr<Vertex> &wpVertexObj1, const std::weak_ptr<Vertex> &wpVertexObj2)
	{
		bool Find[2] = { false, false };

		for (unsigned int i = 0; i < this->vectorwpVertexObj.size(); i++)
		{
			if (wpVertexObj1.lock() == this->vectorwpVertexObj.at(i).lock())
				Find[0] = true;
			if (wpVertexObj2.lock() == this->vectorwpVertexObj.at(i).lock())
				Find[1] = true;
		}

		if (Find[0] == true && Find[1] == true)
			return true;

		return false;
	}

	/*
	//複製する（頂点や辺も作りなおす）
	std::shared_ptr<Face> Face::Duplicate()
	{
		
		std::shared_ptr<Face> spNewFaceObj(new Face);

		//頂点と辺のオブジェクトをコピー＆作成
		spNewFaceObj->getvectorwpVertexObj().resize(this->vectorwpVertexObj.size());
		spNewFaceObj->getvectorwpEdgeObj().resize(this->vectorwpEdgeObj.size());
		for (unsigned int i = 0; i < this->vectorwpVertexObj.size(); i++)
		{
			spNewFaceObj->getpvectorwpVertexObj().at(i) = new Vertex(*vectorpVertexObj.at(i));
			spNewFaceObj->getpvectorwpVertexObj().at(i)->AddpFaceObj(pNewFaceObj);
		}
		for (unsigned int i = 0; i < vectorpEdgeObj.size(); i++)
		{
			pNewFaceObj->GetpvectorpEdgeObj()->at(i) = new Edge(pNewFaceObj->GetpvectorpVertexObj()->at(i), pNewFaceObj->GetpvectorpVertexObj()->at((i + 1) % vectorpEdgeObj.size()), i);
			pNewFaceObj->GetpvectorpEdgeObj()->at(i)->AddpFaceObj(pNewFaceObj);
		}

		//辺のIDを元の辺のIDと同じにする
		for (unsigned int i = 0; i < pNewFaceObj->GetpvectorpEdgeObj()->size(); i++)
		{
			for (unsigned int j = 0; j < vectorpEdgeObj.size(); j++)
			{
				if ((vectorpEdgeObj.at(j)->GetppVertexObj()[0]->GetID() == pNewFaceObj->GetpvectorpEdgeObj()->at(i)->GetppVertexObj()[0]->GetID() &&
					vectorpEdgeObj.at(j)->GetppVertexObj()[1]->GetID() == pNewFaceObj->GetpvectorpEdgeObj()->at(i)->GetppVertexObj()[1]->GetID()) ||
					(vectorpEdgeObj.at(j)->GetppVertexObj()[0]->GetID() == pNewFaceObj->GetpvectorpEdgeObj()->at(i)->GetppVertexObj()[1]->GetID() &&
					vectorpEdgeObj.at(j)->GetppVertexObj()[1]->GetID() == pNewFaceObj->GetpvectorpEdgeObj()->at(i)->GetppVertexObj()[0]->GetID()))
				{
					pNewFaceObj->GetpvectorpEdgeObj()->at(i)->SetID(vectorpEdgeObj.at(j)->GetID());
				}
			}
		}

		return pNewFaceObj;
		
	}
	*/

	//登録されている辺を削除
	void Face::ErasewpEdgeObj(const std::weak_ptr<Edge> &wpEdgeObj)
	{
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end();)
		{
			if (itr->lock() == wpEdgeObj.lock())
				itr = vectorwpEdgeObj.erase(itr);
			else
				itr++;
		}

		return;
	}

	//面積を計算する
	double Face::CalcArea()
	{
		double Len[3];
		Len[0] = vec3d::GetDistance(this->vectorwpVertexObj.at(0).lock()->getvecLocalPos(), this->vectorwpVertexObj.at(1).lock()->getvecLocalPos());
		Len[1] = vec3d::GetDistance(this->vectorwpVertexObj.at(1).lock()->getvecLocalPos(), this->vectorwpVertexObj.at(2).lock()->getvecLocalPos());
		Len[2] = vec3d::GetDistance(this->vectorwpVertexObj.at(2).lock()->getvecLocalPos(), this->vectorwpVertexObj.at(0).lock()->getvecLocalPos());

		double s = 0.5 * (Len[0] + Len[1] + Len[2]);

		double ToSqrt = s * (s - Len[0]) * (s - Len[1]) * (s - Len[2]); //sqrt関数に入れる前段階の値

		if (ToSqrt < 0) //sqrtに負の値を入れると正常に求まらないため
			return 0;

		return sqrt(ToSqrt);
	}

	//ある中心座標との4点の体積を求める
	double Face::CalcVolume(const vec3d &vecCenter)
	{
		return abs(vec3d::GetDot(this->vectorwpVertexObj.at(0).lock()->getvecLocalPos() - vecCenter, vec3d::GetCross(this->vectorwpVertexObj.at(2).lock()->getvecLocalPos() - vecCenter, this->vectorwpVertexObj.at(1).lock()->getvecLocalPos() - vecCenter)) / 6.0);
	}

	//ある中心座標との4点の平均（重心）を求める
	vec3d Face::CalcCentroid(const vec3d &vecCenter)
	{
		return (vecCenter + this->vectorwpVertexObj.at(0).lock()->getvecLocalPos() + this->vectorwpVertexObj.at(1).lock()->getvecLocalPos() + this->vectorwpVertexObj.at(2).lock()->getvecLocalPos()) * 0.25;
	}

	//任意の座標から任意の方向へ向かう直線との交点を求め、求まった場合は任意の座標からの距離を返す
	bool Face::CalcDistance(double &OutDistance, const vec3d &vecPos, const vec3d &vecDirection)
	{
		vec3d vecN = getvecLocalNormal(); //面の法線ベクトルを計算
		double Dot = vec3d::GetDot(vecN, vecDirection);
		if (Dot == 0.0) //任意の方向ベクトルと面の法線ベクトルが平行な場合
			return false; //交わらない

		double Distance = vec3d::GetDot(-vecN, (vecPos - this->vectorwpVertexObj.at(0).lock()->getvecLocalPos())) / Dot; //任意の座標から交点までの距離を求める

		vec3d vecCrossPoint = vecPos + vecDirection * Distance; //交点座標を求める

		//交点座標が三角形の内側にあるかどうかを調べる
		bool Inner = true;
		vec3d vecCross;
		for (int i = 0; i < 3; i++)
		{
			vecCross = vec3d::GetCross(vecCrossPoint - this->vectorwpVertexObj.at(i).lock()->getvecLocalPos(), this->vectorwpVertexObj.at((i + 1) % 3).lock()->getvecLocalPos() - this->vectorwpVertexObj.at(i).lock()->getvecLocalPos()); //三角形の辺と、ある一点カラ交点へ向かうベクトルとの外積
			if (vec3d::GetDot(vecCross, vecN) > 0) //内積が正方向、つまり三角形の外側の場合
			{
				Inner = false;
				break;
			}
		}

		if (!Inner) //交点が三角形の外側だった場合
			return false;

		OutDistance = Distance;

		return true;
	}
	
	//2つの頂点のインデックスから辺オブジェクトを得る
	std::weak_ptr<Edge> Face::GetwpEdgeObj(const int VertexIndex1, const int VertexIndex2)
	{
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end(); itr++) //辺オブジェクトを走査
			if ((this->vectorwpVertexObj.at(VertexIndex1).lock() == itr->lock()->getwpVertexObj1().lock() && this->vectorwpVertexObj.at(VertexIndex2).lock() == itr->lock()->getwpVertexObj2().lock() ||
				(this->vectorwpVertexObj.at(VertexIndex1).lock() == itr->lock()->getwpVertexObj1().lock() && this->vectorwpVertexObj.at(VertexIndex2).lock() == itr->lock()->getwpVertexObj1().lock()))) //引数で送られてきたインデックスの頂点を持つ辺だった場合
				return *itr;

		return std::weak_ptr<Edge>();
	}

	//所属する頂点と辺のオブジェクトを削除
	void Face::DeleteVertexAndEdge()
	{
		for (std::vector<std::weak_ptr<Vertex>>::iterator itr = this->vectorwpVertexObj.begin(); itr != this->vectorwpVertexObj.end(); itr++)
			itr->lock().reset();
		for (std::vector<std::weak_ptr<Edge>>::iterator itr = this->vectorwpEdgeObj.begin(); itr != this->vectorwpEdgeObj.end(); itr++)
			itr->lock().reset();

		this->vectorwpVertexObj.clear();
		this->vectorwpEdgeObj.clear();

		return;
	}

	//ローカル座標の法線ベクトルを計算する
	void Face::CalcvecLocalNormal()
	{
		this->vecLocalNormal = vec3d::GetCross(this->vectorwpVertexObj.at(1).lock()->getvecLocalPos() - this->vectorwpVertexObj.at(0).lock()->getvecLocalPos(), this->vectorwpVertexObj.at(2).lock()->getvecLocalPos() - this->vectorwpVertexObj.at(0).lock()->getvecLocalPos()).GetNormalize();

		return;
	}
	//ワールド座標の法線ベクトルを計算する
	void Face::CalcvecWorldNormal()
	{
		this->vecWorldNormal = vec3d::GetCross(this->vectorwpVertexObj.at(1).lock()->getvecWorldPos() - this->vectorwpVertexObj.at(0).lock()->getvecWorldPos(), this->vectorwpVertexObj.at(2).lock()->getvecWorldPos() - this->vectorwpVertexObj.at(0).lock()->getvecWorldPos()).GetNormalize();

		return;
	}
}