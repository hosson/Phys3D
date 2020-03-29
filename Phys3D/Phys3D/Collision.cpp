#include "Collision.h"
#include "../hoLib/Common.h"
#include "Body.h"
#include "Contact.h"
#include "../hoLib/hoLib.h"
#include "../hoLib/Debug.h"
#include "Mesh.h"
#include "../hoLib/num/Quaternion.h"
#include "../hoLib/num/constant.h"
#include "../hoLib/num/numerical.hpp"

using namespace ho::num;

namespace Phys3D
{
	//コンストラクタ
	Collision::Collision()
	{
	}

	//デストラクタ
	Collision::~Collision()
	{
	}

	//全ての物体から接触情報を取得
	void Collision::DetectContact(std::list<CONTACT> &listContact, const std::list<std::shared_ptr<Body>> &listspBodyObj)
	{
		std::list<PAIR> listPair; //ペアリスト
		GetPair(listPair, listspBodyObj); //接触可能性のあるペアを取得する

		for (std::list<PAIR>::iterator itr = listPair.begin(); itr != listPair.end(); itr++) //全てのペアを走査
		{
			CONTACT Contact(*itr); //ペアをセットして接触情報を生成

			if (CollisionJudge(Contact)) //物体どうしの衝突判定を行い、接触して接触情報が取得された場合
				listContact.push_back(Contact);
		}

		return;
	}

	//接触可能性のあるペアを取得する
	void Collision::GetPair(std::list<PAIR> &listPair, const std::list<std::shared_ptr<Body>> &listspBodyObj)
	{
		std::list<std::shared_ptr<Body>>::const_iterator itr2;
		for (std::list<std::shared_ptr<Body>>::const_iterator itr1 = listspBodyObj.begin(); itr1 != listspBodyObj.end(); itr1++)
		{
			itr2 = itr1;
			itr2++;

			for (; itr2 != listspBodyObj.end(); itr2++)
				if (!(*itr1)->getFixed() || !(*itr2)->getFixed()) //どちらかが固定物でない場合
					if (((*itr2)->getvecPos() - (*itr1)->getvecPos()).GetPower() <= (*itr1)->getBoundingRadius() + (*itr2)->getBoundingRadius()) //半径同士の判定で衝突可能性がある場合
						listPair.push_back(PAIR(*itr1, *itr2));
		}

		return;
	}

	//剛体同士の衝突判定
	bool Collision::CollisionJudge(CONTACT &Contact)
	{
		std::vector<std::shared_ptr<Mesh>> vectorspMeshObj(2); //めり込みメッシュ

		//メッシュのワールド頂点座標を更新
		for (int i = 0; i < 2; i++)
			Contact.Pair.wpBodyObj[i].lock()->UpdateMeshVertexWorldPos();

		if (!GetSinkMesh(vectorspMeshObj, Contact.Pair.wpBodyObj[0].lock()->getspMeshObj(), Contact.Pair.wpBodyObj[1].lock()->getspMeshObj())) //2つのメッシュを衝突判定し衝突部分のメッシュを得て、得られなかった場合
			return false;

		//めり込みメッシュから接触情報を計算
		Contact.vecN = CalcvecNFromSinkMesh(vectorspMeshObj); //衝突メッシュより接触面の法線ベクトルを計算
		Contact.vecWorldPos = CalcHitPos(vectorspMeshObj);//衝突メッシュより接触位置を計算
		Contact.SinkDistance = CalcSinkDistance(vectorspMeshObj, Contact.vecWorldPos, Contact.vecN); //接触メッシュと接触位置と接触面の法線ベクトルよりめり込み距離を計算
		//Contact.vectorvecSubPos.clear(); //接触が継続していた時のために、配列をクリア
		//GetSubPos(Contact.vectorvecSubPos, vectorspMeshObj, Contact.vecWorldPos, Contact.vecN); //サブソルバー用の接触位置を取得

		return true;
	}

	//2つのメッシュを衝突判定し衝突部分のメッシュを得る
	bool Collision::GetSinkMesh(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const std::weak_ptr<Mesh> &wpMeshObj1, const std::weak_ptr<Mesh> &wpMeshObj2)
	{	
		std::shared_ptr<Mesh> spMeshObj[2] = { wpMeshObj1.lock(), wpMeshObj2.lock() };

		std::vector<std::list<vec3d>> vectorlistvecCrossPoint[2]; //両方のメッシュの面ごとの交点
		vectorlistvecCrossPoint[0].resize(spMeshObj[0]->getvectorspFaceObj().size());
		vectorlistvecCrossPoint[1].resize(spMeshObj[1]->getvectorspFaceObj().size());

		bool Collision1 = false, Collision2 = false; //接触があったかどうか
		std::vector<double> vectorVtHeight; //走査中のメッシュの頂点の、相手メッシュの面に対する高さ
		for (int i = 0; i < 2; i++) //メッシュを走査
		{
			//走査中のメッシュのすべての頂点の、相手メッシュの面に対する高さを計算する
			vectorVtHeight.resize(spMeshObj[i]->getvectorspVertexObj().size() * spMeshObj[1 - i]->getvectorspFaceObj().size()); //頂点数×面の数
			for (unsigned int j = 0; j < spMeshObj[i]->getvectorspVertexObj().size(); j++) //頂点を走査
				for (unsigned int k = 0; k < spMeshObj[1 - i]->getvectorspFaceObj().size(); k++) //面を走査
					vectorVtHeight.at(j * spMeshObj[1 - i]->getvectorspFaceObj().size() + k) = vec3d::GetDot(spMeshObj[i]->getvectorspVertexObj().at(j)->getvecWorldPos() - spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvectorwpVertexObj().at(0).lock()->getvecWorldPos(), spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvecWorldNormal());

			//頂点が相手のメッシュの内側にあるかどうかを検出
			for (unsigned int j = 0; j < spMeshObj[i]->getvectorspVertexObj(). size(); j++) //頂点を走査
			{
				bool VtInner = true; //頂点が面の内側にあるかどうか
				for (unsigned int k = 0; k < spMeshObj[1 - i]->getvectorspFaceObj().size(); k++) //面を走査
				{
					if (vectorVtHeight.at(j * spMeshObj[1 - i]->getvectorspFaceObj().size() + k) > 0) //面の外側だった場合
					{
						VtInner = false;
						break;
					}
				}
				if (VtInner) //内側だった場合
				{
					Collision1 = true;
					for (std::vector<std::weak_ptr<Face>>::iterator itr = spMeshObj[i]->getvectorspVertexObj().at(j)->getvectorwpFaceObj().begin(); itr != spMeshObj[i]->getvectorspVertexObj().at(j)->getvectorwpFaceObj().end(); itr++) //頂点が存在する面を走査
						vectorlistvecCrossPoint[i].at((*itr).lock()->getID()).push_back(spMeshObj[i]->getvectorspVertexObj().at(j)->getvecWorldPos()); //面に交点として追加する
				}
			}

			//走査中メッシュの辺と相手メッシュの面を判定
			for (unsigned int j = 0; j < spMeshObj[i]->getvectorspEdgeObj().size(); j++) //辺を走査
			{
				int ID[2] = { spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj1().lock()->getID(), spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj2().lock()->getID() }; //辺に接続された2つの頂点のIDを取得

				for (unsigned int k = 0; k < spMeshObj[1 - i]->getvectorspFaceObj().size(); k++) //面を走査
				{
					//交点を求める
					vec3d vecCrossPoint; //交点
					double Height[2] = { vectorVtHeight.at(ID[0] * spMeshObj[1 - i]->getvectorspFaceObj().size() + k), vectorVtHeight.at(ID[1] * spMeshObj[1 - i]->getvectorspFaceObj().size() + k) }; //辺の2つの頂点の高さ
					if ((Height[0] > 0 && Height[1] > 0) || (Height[0] < 0 && Height[1] < 0)) //頂点が平面の片側に寄っている場合
						continue;
					if (Height[0] == 0 && Height[1] == 0) //両方の頂点が面の内部にある場合
						vecCrossPoint = (spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj1().lock()->getvecWorldPos() + spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj2().lock()->getvecWorldPos()) * 0.5;
					else
						vecCrossPoint = spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj1().lock()->getvecWorldPos() + (spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj2().lock()->getvecWorldPos() - spMeshObj[i]->getvectorspEdgeObj().at(j)->getwpVertexObj1().lock()->getvecWorldPos()) * (abs(Height[0]) / (abs(Height[0]) + abs(Height[1])));

					//交点が面の内側にあるかどうかを調べる
					bool Inner = true;
					int lNext;
					for (int l = 0; l < 3; l++) //面の辺を走査
					{
						lNext = (l + 1) % 3;
						if (0 < vec3d::GetDot(vec3d::GetCross(vecCrossPoint - spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvectorwpVertexObj().at(l).lock()->getvecWorldPos(), spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvectorwpVertexObj().at(lNext).lock()->getvecWorldPos() - spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvectorwpVertexObj().at(l).lock()->getvecWorldPos()), spMeshObj[1 - i]->getvectorspFaceObj().at(k)->getvecWorldNormal())) //面の辺の外側にあった場合
						{
							Inner = false;
							break;
						}
					}

					if (Inner) //交点が面の内側だった場合
					{
						Collision2 = true;
						vectorlistvecCrossPoint[1 - i].at(k).push_back(vecCrossPoint); //相手の面に交点を追加
						for (std::vector<std::weak_ptr<Face>>::const_iterator itr = spMeshObj[i]->getvectorspEdgeObj().at(j)->getvectorwpFaceObj().begin(); itr != spMeshObj[i]->getvectorspEdgeObj().at(j)->getvectorwpFaceObj().end(); itr++) //辺が含まれる面を走査
							vectorlistvecCrossPoint[i].at((*itr).lock()->getID()).push_back(vecCrossPoint); //自分のメッシュの面に交点を追加
					}
				}
			}
		}

		if (!Collision2) //接触していなかった場合
			return false; //Collision1のみがtrueの場合は、閉じてない図形の反対側に頂点がめり込んでいるのみなので、接触しているとは言えない

		//交点情報を元にめり込みメッシュを構成する面を取得する
		for (int i = 0; i < 2; i++) //メッシュを走査
		{
			vectorspMeshObj[i] = ho::sp_init<Mesh>(new Mesh()); //めり込みメッシュオブジェクトを生成

			for (unsigned int j = 0; j < spMeshObj[i]->getvectorspFaceObj().size(); j++) //面を走査
				GetSinkMeshFace(vectorspMeshObj[i], spMeshObj[i]->getvectorspFaceObj().at(j), vectorlistvecCrossPoint[i].at(j)); //交点から面を取得
		}

		return true;
	}

	//衝突線分で分割し、めり込み部分のメッシュを構成する面を得る
	void Collision::GetSinkMeshFace(const std::shared_ptr<Mesh> &spMeshObj, const std::shared_ptr<Face> &spFaceObj, std::list<vec3d> &listvecCrossPoint)
	//void Collision::GetSinkMeshFace(std::list<Face *> *plistpFaceObj, Face *pFaceObj, std::list<vec3d> *plistvecCrossPoint)
	{
		if (listvecCrossPoint.size() < 3) //衝突交点が3つ未満の場合
			return; //三角形が作れないのでここで終了

		//★重複している座標をカットし、残った交点が3つに満たない場合はreturnする（未実装）
		CreateOutLine(listvecCrossPoint, spFaceObj->getvecWorldNormal()); //頂点情報群の外側の辺を作成して接続して多角形を作る
		DividePolygonIntoTriangles(spMeshObj, listvecCrossPoint); //多角形を複数の三角形の面に分割
		
		return;
	}

	//頂点リストから外側の辺を構成する頂点だけを残し、残りを削除する
	void Collision::CreateOutLine(std::list<vec3d> &listvecCrossPoint, const vec3d &vecFaceN)
	{
		//外側を循環するように並び替える
		for (std::list<vec3d>::iterator itr1 = listvecCrossPoint.begin(); itr1 != listvecCrossPoint.end();)
		{
			std::list<vec3d>::iterator itr2 = itr1;
			itr2++;

			bool Find = false; //itr1から外側に続く次の頂点が見つかったかどうか
			while (itr2 != listvecCrossPoint.end())
			{
				std::list<vec3d>::iterator itr3 = listvecCrossPoint.begin();

				bool Result = true;
				vec3d vec1To2 = (*itr2 - *itr1).GetNormalize(); //itr1→itr2 の頂点へ向かう方向ベクトル

				while (itr3 != listvecCrossPoint.end())
				{
					if (itr3 != itr2 && itr3 != itr1)
					{
						if (vec3d::GetDot(vec3d::GetCross(*itr3 - *itr1, vec1To2), vecFaceN) > 0) //itr1～itr2の直線よりもitr3が外側にある場合
						{
							Result = false;
							break;
						}
					}

					itr3++;
				}

				if (Result) //itr2がitr1から外側に続く次の頂点だった場合
				{
					Find = true;

					std::list<vec3d>::iterator itr = itr1;
					itr++;
					listvecCrossPoint.insert(itr, *itr2);
					listvecCrossPoint.erase(itr2);
					break;
				}

				itr2++;
			}

			if (!Find) //外側に続く頂点が見つからなかった場合
			{
				if (itr1 == listvecCrossPoint.begin()) //並び替えられた頂点が存在しない場合
				{
					itr1 = listvecCrossPoint.erase(itr1);
				} else {
					//この後は外側のラインに含まれない頂点なので削除
					itr1++;
					for (std::list<vec3d>::iterator itr = itr1; itr != listvecCrossPoint.end();)
						itr = listvecCrossPoint.erase(itr);
					break; //この後は並び替えられた最初の頂点につながるしかないので終わる
				}
			} else {
				itr1++;
			}
		}
		
		return;
	}

	//多角形を複数の三角形の面に分割
	void Collision::DividePolygonIntoTriangles(const std::shared_ptr<Mesh> &spMeshObj, const std::list<vec3d> &listvecCrossPoint)
	//void Collision::DividePolygonIntoTriangles(std::list<Face *> *plistpFaceObj, std::list<vec3d> *plistvecCrossPoint)
	{
		std::vector<vec3d> vectorvec(3);

		std::list<vec3d>::const_iterator itrBegin, itr1, itr2;
		itrBegin = listvecCrossPoint.begin();
		itr1 = itrBegin;
		itr1++;
		itr2 = itr1;
		itr2++;

		int ID = 0;
		while (itr2 != listvecCrossPoint.end())
		{
			vectorvec.at(0) = *itrBegin;
			vectorvec.at(1) = *itr1;
			vectorvec.at(2) = *itr2;
			spMeshObj->AddFace(vectorvec); //めり込みメッシュに三角形の面を追加

			itr1 = itr2;
			itr2++;
		}
		
		return;
	}

	//接触メッシュより法線ベクトルを計算する
	vec3d Collision::CalcvecNFromSinkMesh(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj)
	{
		vec3d vecMeshN[2] = { vec3d(0, 0, 0), vec3d(0, 0, 0) }; //それぞれのメッシュの面で構成される法線ベクトル

		for (int i = 0; i < 2; i++) //物体を走査
		{
			for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++) //接触メッシュの面を走査
				vecMeshN[i] += (*itr)->getvecLocalNormal() * (*itr)->CalcArea(); //面の法線ベクトルに面の面積を重みとして掛けたものを加算する
			vecMeshN[i] = vecMeshN[i].GetNormalize(); //正規化
		}

		return (-vecMeshN[0] + vecMeshN[1]).GetNormalize();
	}

	//接触メッシュより接触位置を計算する
	vec3d Collision::CalcHitPos(std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj)
	{
		//メッシュの座標の平均値の中心を求める
		int SumNum = 0;
		vec3d vecAverageCenter(0, 0, 0); //メッシュの頂点座標の平均位置
		for (int i = 0; i < 2; i++) //メッシュの物体を走査
		{
			for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++)
			{
				vecAverageCenter += (*itr)->getvectorwpVertexObj().at(0).lock()->getvecLocalPos();
				SumNum++;
			}
		}
		vecAverageCenter /= SumNum;

		vec3d vecCentroid(0, 0, 0); //重心
		double TotalVolume = 0; //体積の合計
		double Vol;
		vec3d Cen;
		for (int i = 0; i < 2; i++) //メッシュの物体を走査
		{
			for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++)
			{
				Vol = (*itr)->CalcVolume(vecAverageCenter); //面と中心座標から体積を計算
				Cen = (*itr)->CalcCentroid(vecAverageCenter); //面と中心座標から重心を計算
				vecCentroid += Cen * Vol;
				TotalVolume += Vol;
			}
		}

		if (TotalVolume == 0) //体積の合計が0の場合
		{
			//三角形の面積で重心を決める
			double TotalArea = 0; //面積の合計
			double Area;
			vecCentroid = vec3d(0, 0, 0);
			for (int i = 0; i < 2; i++) //メッシュの物体を走査
			{
				for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++)
				{
					Area = (*itr)->CalcArea(); //面の面積を計算
					Cen = (*itr)->CalcCentroid(vecAverageCenter); //面と中心座標から重心を計算
					vecCentroid += Cen * Area;
					TotalArea += Area;
				}
			}
			if (TotalArea != 0)
				vecCentroid /= TotalArea;
		} else {
			vecCentroid /= TotalVolume;
		}

		return vecCentroid;
	}

	//接触メッシュと衝突位置と衝突法線より、めり込み距離を計算する
	double Collision::CalcSinkDistance(const std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const vec3d &vecHitPos, const vec3d &vecN)
	{
		double TotalDistance = 0, Distance;
		for (int i = 0; i < 2; i++) //メッシュの物体を走査
			for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(i)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(i)->getvectorspFaceObj().end(); itr++)
				if ((*itr)->CalcDistance(Distance, vecHitPos, vecN)) //接触位置と接触法線から、面との衝突判定をし、衝突していた場合
					TotalDistance += abs(Distance); //距離を加算

		return TotalDistance;
	}

	//サブソルバー用の接触位置を取得
	void Collision::GetSubPos(std::vector<vec3d> &vectorvecSubPos, const std::vector<std::shared_ptr<Mesh>> &vectorspMeshObj, const vec3d &vecHitPos, const vec3d &vecN)
	{
		const int Num = 3; //生成する位置数
		const double Ratio = 0.8; //中心から接触位置までの距離比率（1.0が壁）

		const vec3d vecAxis1 = [&]()->vec3d
		{
			if (vecN != vec3d(1.0, 0.0, 0.0) && vecN != vec3d(-1.0, 0.0, 0.0))
				return vec3d(1.0, 0.0, 0.0);
			return vec3d(0.0, 1.0, 0.0);
		}(); //法線と垂直なベクトルを得るための軸

		vec3d vecT = vec3d::GetCross(vecN, vecAxis1).GetNormalize(); //法線と垂直なベクトル

		double Distance;

		for (int i = 0; i < Num; i++) //位置数の数だけ走査
		{
			Quaternion qt = Quaternion(vecN, PI2 * (double(i) / Num));
			vec3d vecDir; //方向ベクトル
			qt.TransformVector(vecDir, vecT);

			for (int j = 0; j < 2; j++) //メッシュの物体を走査
			{
				for (std::vector<std::shared_ptr<Face>>::const_iterator itr = vectorspMeshObj.at(j)->getvectorspFaceObj().begin(); itr != vectorspMeshObj.at(j)->getvectorspFaceObj().end(); itr++)
				{
					if ((*itr)->CalcDistance(Distance, vecHitPos, vecDir)) //接触位置と方向ベクトルから、面との衝突判定をし、衝突していた場合
					{
						Distance = abs(Distance);
						vectorvecSubPos.push_back(vecHitPos + vecDir * Distance * Ratio); //サブソルバー位置を配列に追加
						goto BREAK1;
					}
				}
			}
		BREAK1:;
		}

		return;
	}
}
