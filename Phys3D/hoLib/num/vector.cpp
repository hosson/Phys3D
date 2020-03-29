#include "vector.h"
#include "../Error.h"

namespace ho
{
	namespace num
	{
		//XY平面上のvec3d型に変換する
		vec3d vec2d::Getvec3d_XY(const double &z) const
		{
			return vec3d(x, y, z);
		}

		//XZ平面上のvec3d型に変換する
		vec3d vec2d::Getvec3d_XZ(const double &y) const
		{
			return vec3d(x, y, y);
		}

		
		//垂直な方向のベクトルを返す
		vec2d vec2d::GetPerpendicular(const e2LR::e LR) const
		{
		switch (LR)
		{
		case e2LR::left: //左方向を得る場合
			return vec2d(-y, x);
			break;
		case e2LR::right: //右方向を得る場合
			return vec2d(y, -x);
			break;
		default:
			ERR(false, TEXT("引数 LR に有効範囲外の値が渡されました。"), __WFILE__, __LINE__); //エラー出力
			break;
		}

		return vec2d(0, 0); //一応0を返す
		}
		

		//2つのベクトルの成す角度を求める
		bool vec2d::GetAngle(double &out, const vec2d &vec1, const vec2d &vec2)
		{
			double Denominator = sqrt(vec1.x * vec1.x + vec1.y * vec1.y) * sqrt(vec2.x * vec2.x + vec2.y * vec2.y); //分母を先に計算する

			if (Denominator) //分母が0以外の場合
			{
				double ToFunction = GetInnerProduct(vec1, vec2) / Denominator; //acos関数に入れる値

				//acosに入れる値を1.0～-1.0の範囲に収める
				if (ToFunction < -1.0)
					ToFunction = -1.0 - fmod(ToFunction, 1.0);
				if (ToFunction > 1.0)
					ToFunction = 1.0 - fmod(ToFunction, 1.0);

				out = acos(ToFunction);
				return true;
			}

			return false; //片方まはた両方のベクトルの大きさが 0 なので角度が求まらなかった
		}

		/*
		//2つの直線の交点を求める
		bool ho::vec2d::GetCrossPoint(const vec2d &vecLine1Begin, const vec2d &vecLine1End, const vec2d &vecLine2Begin, const vec2d &vecLine2End, ho::Enable<ho::vec2d> &vecCrossPos, bool &Inner)
		{
		double Height[2]; //直線1を基準にした、直線2の始点と終点の高さ

		Height[0] = vec2d::GetCrossProduct((vecLine1End - vecLine1Begin).GetNormalize(), vecLine2Begin - vecLine1Begin);
		Height[1] = vec2d::GetCrossProduct((vecLine1End - vecLine1Begin).GetNormalize(), vecLine2End - vecLine1Begin);

		if (Height[0] == Height[1]) //高さが同じ場合（平行の場合）
		{
		if (Height[0] == 0.0) //直線2が直線1と重なっている場合
		{
		vecCrossPos = ho::Enable<ho::vec2d>((vecLine2Begin + vecLine2End) * 0.5); //直線2の中心位置を返す
		Inner = true;

		return true;
		} else {
		vecCrossPos = ho::Enable<ho::vec2d>();
		Inner = false;

		return false;
		}
		}


		int Sign[2] = {ho::GetSign(Height[0]), ho::GetSign(Height[1])}; //高さの符号
		if (Sign[0] * Sign[1] == 1) //符号が同じ場合
		{
		//交点座標を計算
		vecCrossPos = ho::Enable<ho::vec2d>(vecLine2Begin + (vecLine2End - vecLine2Begin) * (Height[0] / (Height[0] - Height[1])));

		Inner = false; //交点は線分の外側にある
		} else {
		//交点座標を計算
		vecCrossPos = ho::Enable<ho::vec2d>(vecLine2Begin + (vecLine2End - vecLine2Begin) * (abs(Height[0]) / abs(Height[0] - Height[1])));

		//交点が線分1の内側にあるかどうかを判定
		Inner = false;
		double DistanceLine1 = ho::vec2d::GetDistance(vecLine1Begin, vecLine1End); //線分1の始点と終点の距離
		if (ho::vec2d::GetDistance(vecLine1Begin, vecCrossPos()) <= DistanceLine1 && ho::vec2d::GetDistance(vecLine1End, vecCrossPos()) <= DistanceLine1) //交点が線分1の範囲内の場合
		{
		double DistanceLine2 = ho::vec2d::GetDistance(vecLine2Begin, vecLine2End); //線分2の始点と終点の距離
		if (ho::vec2d::GetDistance(vecLine2Begin, vecCrossPos()) <= DistanceLine2 && ho::vec2d::GetDistance(vecLine2End, vecCrossPos()) <= DistanceLine2) //交点が線分2の範囲内の場合
		{
		Inner = true; //交点は線分1と2の範囲内
		}
		}
		}

		return true;
		}
		*/



		vec3d vec3d::operator *(const Matrix &mtrx) const
		{
			//ベクトルを行列に変換
			Matrix mtrxVec(3, 1);
			mtrxVec(0, 0) = this->x;
			mtrxVec(1, 0) = this->y;
			mtrxVec(2, 0) = this->z;

			Matrix mtrxResult = mtrxVec * mtrx;

			return vec3d(mtrxResult(0, 0), mtrxResult(1, 0), mtrxResult(2, 0));
		}

		//2つのベクトルの成す角の角度を返す
		bool vec3d::GetAngle(double &out, const vec3d &vec1, const vec3d &vec2)
		{
			double Div = vec1.GetPower() * vec2.GetPower(); //除算に使用する値
			if (Div != 0.0) //除算できる場合
			{
				double ToFunction = double(vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z) / Div; //acos関数に入れる値
				//acosに入れる値を1.0～-1.0の範囲に収める
				if (ToFunction < -1.0)
					ToFunction = -1.0 - fmod(ToFunction, 1.0);
				if (ToFunction > 1.0)
					ToFunction = 1.0 - fmod(ToFunction, 1.0);

				out = acos(ToFunction); //アークコサインの結果を計算
				return true;
			}

			return false; //片方または両方のベクトルの大きさが 0 の場合
		}

		//2つのベクトル間の成す角の角度と回転軸ベクトルを得る
		bool vec3d::GetAngleAndAxis(double &Angle, vec3d &vecAxis, const vec3d &vec1, const vec3d &vec2)
		{
			if (!GetAngle(Angle, vec1, vec2)) //角度を求め、求まらなかった場合
				return false;

			vecAxis = GetCross(vec1, vec2); //外積で回転軸ベクトルを求める
			return true;
		}

		/*
		//2つのベクトルからなる直線同士の一番接近する2点を求める
		//pSrcには直線の通る2点を2つの直線分、合計4点の先頭のアドレスを入れる
		bool ho::vec3d::GetCrossPointsFromLines(std::vector<vec3d> &vecOut, vec3d pIn[4])
		{
		vecOut.resize(2);

		PLANE plnBase(pIn[0], pIn[1], pIn[0] + (pIn[3] - pIn[2])); //2つの直線を含む平面を作成
		vec3d vecNormalLine = plnBase.GetNormalLine(); //平面の法線ベクトルを取得

		int Result = 0; //直線と平面の交点が見つかった回数

		PLANE plnLine2(pIn[2], pIn[3], pIn[2] + vecNormalLine); //直線2側の平面を作成
		Result += plnLine2.GetCrossPointFromLine(vecOut.at(0), pIn[0], pIn[1]); //直線1と直線2側の平面との交点を計算

		PLANE plnLine1(pIn[0], pIn[1], pIn[0] + vecNormalLine); //直線1側の平面を作成
		Result += plnLine1.GetCrossPointFromLine(vecOut.at(1), pIn[2], pIn[3]); //直線2と直線1側の平面との交点を計算

		if (Result < 2) //成功回数が2回以下だった場合
		return false; //求まらなかった

		return true;
		}
		*/
	}
}

