#include "Body.h"
#include "BodyCreator.h"
#include "../hoLib/Error.h"
#include "Mesh.h"

namespace Phys3D
{
	//物体を生成する
	std::shared_ptr<Body> BodyCreator::CreateBody(vec3d vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecOmega, std::vector<MASSPOINT> vectorMassPoint, std::vector<vec3d> vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const double &e, const double &mu, const bool &Fixed)
	{
		//総重量を計算
		double Mass = 0;
		if (Fixed) //固定物の場合
		{
			Mass = DBL_MAX;
		} else {
			for (std::vector<MASSPOINT>::iterator itr = vectorMassPoint.begin(); itr != vectorMassPoint.end(); itr++)
				Mass += itr->Mass;
		}

		//重心を計算
		vec3d vecLocalCenter(0, 0, 0); //ローカル座標での重心
		for (std::vector<MASSPOINT>::const_iterator itr = vectorMassPoint.begin(); itr != vectorMassPoint.end(); itr++)
			vecLocalCenter += itr->vecPos * (itr->Mass / Mass);
		vecPos += vecLocalCenter; //物体の座標を重心位置に変更

		//質点の位置を重心から見た位置に変更
		for (std::vector<MASSPOINT>::iterator itr = vectorMassPoint.begin(); itr != vectorMassPoint.end(); itr++)
			itr->vecPos = itr->vecPos - vecLocalCenter;

		//頂点情報を重心から見た座標に変更
		for (std::vector<vec3d>::iterator itr = vectorvecVertex.begin(); itr != vectorvecVertex.end(); itr++)
			*itr -= vecLocalCenter;

		std::vector<int> vectorVtEdgeIndex = GetvectorVtEdgeIndex(vectorVtFaceIndex); //辺の頂点インデックスを抽出

		std::shared_ptr<Mesh> spMeshObj = ho::sp_init<Mesh>(new Mesh(vectorvecVertex, vectorVtEdgeIndex, vectorVtFaceIndex)); //メッシュを作成

		Matrix mtrxInertia = GetmtrxInertia(vectorMassPoint, Fixed); //質点データから慣性モーメントテンソルを計算する

		vec3d vecL = vecOmega * mtrxInertia; //角速度ω × 慣性モーメントテンソルI によって 角運動量L を求める

		return std::shared_ptr<Body>(new Body(vecPos, vecLinearVelocity, qtAngle, vecL, Mass, mtrxInertia, spMeshObj, e, mu, Fixed));
	}

	//物体を生成する
	std::shared_ptr<Body> BodyCreator::CreateBody(vec3d vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecOmega, const double &Density, std::vector<vec3d> vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const double &e, const double &mu, const bool &Fixed)
	{
		//総重量を計算
		double Mass = 0;
		double TotalVolume = 0;
		if (Fixed) //固定物の場合
		{
			Mass = DBL_MAX;
		} else {
			for (std::vector<int>::const_iterator itr = vectorVtFaceIndex.begin(); itr != vectorVtFaceIndex.end(); itr += 3) //全ての面を走査
				TotalVolume += abs(vec3d::GetDot(vectorvecVertex.at(*itr), vec3d::GetCross(vectorvecVertex.at(*(itr + 2)), vectorvecVertex.at(*(itr + 1)))) / 6.0); //四面体の体積を加算
			Mass = TotalVolume * Density; //質量 = 体積 * 密度
		}

		//重心を計算
		vec3d vecLocalCenter(0, 0, 0); //ローカル座標での重心
		for (std::vector<int>::const_iterator itr = vectorVtFaceIndex.begin(); itr != vectorVtFaceIndex.end(); itr += 3) //全ての面を走査
		{
			double Volume = abs(vec3d::GetDot(vectorvecVertex.at(*itr), vec3d::GetCross(vectorvecVertex.at(*(itr + 2)), vectorvecVertex.at(*(itr + 1)))) / 6.0); //四面体の体積
			vec3d vecCentroid = (vectorvecVertex.at(*itr) + vectorvecVertex.at(*(itr + 1)) + vectorvecVertex.at(*(itr + 2))) * 0.25; //四面体の重心
			vecLocalCenter += vecCentroid * (Volume / TotalVolume);
		}
		vecPos += vecLocalCenter; //物体の座標を重心位置に変更

		//頂点情報を重心から見た座標に変更
		for (std::vector<vec3d>::iterator itr = vectorvecVertex.begin(); itr != vectorvecVertex.end(); itr++)
			*itr -= vecLocalCenter;

		std::vector<int> vectorVtEdgeIndex = GetvectorVtEdgeIndex(vectorVtFaceIndex); //辺の頂点インデックスを抽出

		std::shared_ptr<Mesh> spMeshObj = ho::sp_init<Mesh>(new Mesh(vectorvecVertex, vectorVtEdgeIndex, vectorVtFaceIndex)); //メッシュを作成

		Matrix mtrxInertia = GetmtrxInertia(Density, vectorvecVertex, vectorVtFaceIndex, Fixed); //密度とメッシュ形状から慣性モーメントテンソルを計算する

		vec3d vecL = vecOmega * mtrxInertia; //角速度ω × 慣性モーメントテンソルI によって 角運動量L を求める

		return std::shared_ptr<Body>(new Body(vecPos, vecLinearVelocity, qtAngle, vecL, Mass, mtrxInertia, spMeshObj, e, mu, Fixed));
	}

	//面の頂点インデックスから辺の頂点インデックスを抽出
	std::vector<int> BodyCreator::GetvectorVtEdgeIndex(const std::vector<int> &vectorVtFaceIndex)
	{
		std::vector<int> vectorVtEdgeIndex; //辺の頂点インデックス

		//現在存在する辺の頂点インデックスの中に同じものがあるかどうかチェック
		auto fCheckVtEdgeIndexOverlap = [&](int Index1, int Index2)->bool
		{
			for (unsigned int i = 0; i < vectorVtEdgeIndex.size(); i += 2)
				if ((Index1 == vectorVtEdgeIndex.at(i) && Index2 == vectorVtEdgeIndex.at(i + 1)) || (Index1 == vectorVtEdgeIndex.at(i + 1) && Index2 == vectorVtEdgeIndex.at(i)))
					return true; //重複していた場合

			return false; //重複していない場合
		};


		//面の頂点インデックスから辺の頂点インデックスを抽出
		for (unsigned int i = 0; i < vectorVtFaceIndex.size(); i += 3)
		{
			if (!fCheckVtEdgeIndexOverlap(vectorVtFaceIndex.at(i), vectorVtFaceIndex.at(i + 1))) //重複していない場合
			{
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i));
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i + 1));
			}
			if (!fCheckVtEdgeIndexOverlap(vectorVtFaceIndex.at(i + 1), vectorVtFaceIndex.at(i + 2))) //重複していない場合
			{
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i + 1));
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i + 2));
			}
			if (!fCheckVtEdgeIndexOverlap(vectorVtFaceIndex.at(i + 2), vectorVtFaceIndex.at(i))) //重複していない場合
			{
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i + 2));
				vectorVtEdgeIndex.push_back(vectorVtFaceIndex.at(i));
			}
		}

		return vectorVtEdgeIndex;
	}

	//質点データから慣性モーメントテンソルを計算する
	Matrix BodyCreator::GetmtrxInertia(const std::vector<MASSPOINT> &vectorMassPoint, const bool &Fixed)
	{
		Matrix mtrx;

		mtrx.Resize(3, 3); //行列を3×3のサイズに変更
		mtrx.InitZero(); //全ての要素を0で初期化

		if (Fixed) //固定物体の場合（質量は無限大とする）
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					mtrx(i, j) = DBL_MAX;
		} else {
			for (std::vector<MASSPOINT>::const_iterator itr = vectorMassPoint.begin(); itr != vectorMassPoint.end(); itr++) //質点を走査
			{
				mtrx(0, 0) += itr->Mass * (itr->vecPos.y * itr->vecPos.y + itr->vecPos.z * itr->vecPos.z);
				mtrx(1, 0) -= itr->Mass * itr->vecPos.y * itr->vecPos.x;
				mtrx(2, 0) -= itr->Mass * itr->vecPos.z * itr->vecPos.x;
				mtrx(0, 1) -= itr->Mass * itr->vecPos.x * itr->vecPos.y;
				mtrx(1, 1) += itr->Mass * (itr->vecPos.x * itr->vecPos.x + itr->vecPos.z * itr->vecPos.z);
				mtrx(2, 1) -= itr->Mass * itr->vecPos.z * itr->vecPos.y;
				mtrx(0, 2) -= itr->Mass * itr->vecPos.x * itr->vecPos.z;
				mtrx(1, 2) -= itr->Mass * itr->vecPos.y * itr->vecPos.z;
				mtrx(2, 2) += itr->Mass * (itr->vecPos.x * itr->vecPos.x + itr->vecPos.y * itr->vecPos.y);
			}
		}

		return mtrx;
	}

	//密度とメッシュ形状から慣性モーメントテンソルを計算する
	Matrix BodyCreator::GetmtrxInertia(const double &Density, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const bool &Fixed)
	{
		Matrix mtrx;

		mtrx.Resize(3, 3); //行列を3×3のサイズに変更
		mtrx.InitZero(); //全ての要素を0で初期化

		if (Fixed) //固定物体の場合（質量は無限大とする）
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					mtrx(i, j) = DBL_MAX;
		} else {
			for (std::vector<int>::const_iterator itr = vectorVtFaceIndex.begin(); itr != vectorVtFaceIndex.end(); itr += 3) //全ての面を走査
			{
				Matrix mtrxFace(3, 3); //面1つに対応する四面体の慣性テンソル
				mtrxFace.InitZero();

				const vec3d v1 = vectorvecVertex.at(*itr);
				const vec3d v2 = vectorvecVertex.at(*(itr + 1));
				const vec3d v3 = vectorvecVertex.at(*(itr + 2));

				const double Volume = abs(vec3d::GetDot(v1, vec3d::GetCross(v3, v2)) / 6.0);
				const double Mass = Volume * Density;

				mtrxFace(0, 0) = Mass * (v1.y * v1.y + v1.y * v2.y + v2.y * v2.y + v2.y * v3.y + v3.y * v3.y + v3.y * v1.y + v1.z * v1.z + v1.z * v2.z + v2.z * v2.z + v2.z * v3.z + v3.z * v3.z* +v3.z * v1.z) / 10.0;
				mtrxFace(1, 0) = Mass * (-2.0 * v1.x * v1.y - v1.x * v2.y - v1.x * v3.y - v2.x * v1.y - 2.0 * v2.x * v2.y - v2.x * v3.y - v3.x * v1.y - v3.x * v2.y - 2.0 * v3.x * v3.y) / 20.0;
				mtrxFace(2, 0) = Mass * (-2.0 * v1.x * v1.z - v1.x * v2.z - v1.x * v3.z - v2.x * v1.z - 2.0 * v2.x * v2.z - v2.x * v3.z - v3.x * v1.z - v3.x * v2.z - 2.0 * v3.x * v3.z) / 20.0;
				mtrxFace(0, 1) = Mass * (-2.0 * v1.x * v1.y - v1.x * v2.y - v1.x * v3.y - v2.x * v1.y - 2.0 * v2.x * v2.y - v2.x * v3.y - v3.x * v1.y - v3.x * v2.y - 2.0 * v3.x * v3.y) / 20.0;
				mtrxFace(1, 1) = Mass * (v1.z * v1.z + v1.z * v2.z + v2.z * v2.z + v2.z * v3.z + v3.z * v3.z + v3.z * v1.z + v1.x * v1.x + v1.x * v2.x + v2.x * v2.x + v2.x * v3.x + v3.x * v3.x + v3.x * v1.x) / 10.0;
				mtrxFace(2, 1) = Mass * (-2.0 * v1.y * v1.z - v1.y * v2.z - v1.y * v3.z - v2.y * v1.z - 2.0 * v2.y * v2.z - v2.y * v3.z - v3.y * v1.z - v3.y * v2.z - 2.0 * v3.y * v3.z) / 20.0;
				mtrxFace(0, 2) = Mass * (-2.0 * v1.x * v1.z - v1.x * v2.z - v1.x * v3.z - v2.x * v1.z - 2.0 * v2.x * v2.z - v2.x * v3.z - v3.x * v1.z - v3.x * v2.z - 2.0 * v3.x * v3.z) / 20.0;
				mtrxFace(1, 2) = Mass * (-2.0 * v1.y * v1.z - v1.z * v2.z - v1.y * v3.z - v2.y * v1.z - 2.0 * v2.y * v2.z - v2.y * v3.z - v3.y * v1.z - v3.y * v2.z - 2.0 * v3.y * v3.z) / 20.0;
				mtrxFace(2, 2) = Mass * (v1.x * v1.x + v1.x * v2.x + v2.x * v2.x + v2.x * v3.x + v3.x * v3.x + v1.x * v3.x + v1.y * v1.y + v1.y * v2.y + v2.y * v2.y + v2.y * v3.y + v3.y * v3.y + v3.y * v1.y) / 10.0;

				mtrx = mtrx + mtrxFace;
			}
		}

		return mtrx;
	}
}