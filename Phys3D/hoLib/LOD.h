//LOD(Level of Detail)クラス

#pragma once
#include <vector>
#include "Common.h"
#include "tstring.h"

namespace ho
{
	template <class T> class LOD
	{
	public:
		//コンストラクタ
		LOD(int Objects)
		{
			this->Objects = Objects;
			vectorpMeshObj.resize(Objects, NULL);
			vectorDistance.resize(Objects, 0);
		}

		//デストラクタ
		~LOD() {}

		//メッシュオブジェクトを登録
		bool RegistMeshObj(T *pMeshObj, int Index, float Distance = 0)
		{
			if (Index < 0 || Index >= Objects) //登録可能なインデックスの範囲外の場合
				return FALSE;
		
			vectorpMeshObj.at(Index) = pMeshObj;

			if (Distance != 0) //判定用距離が指定されていた場合
				vectorDistance.at(Index) = Distance;

			return TRUE;
		}

		//距離に応じて適切なメッシュオブジェクトを得る
		T *GetpMeshObjFromDistance(float Distance)
		{
			for (int i = 0; i < Objects; i++)
			{
				if (Distance < vectorDistance.at(i))
					return vectorpMeshObj.at(i);
			}

			return vectorpMeshObj.at(Objects - 1); //距離の範囲外の場合は一番遠く用のオブジェクトを返す
		}
	private:
		int Objects; //メッシュオブジェクトの数
		std::vector<T *> vectorpMeshObj; //メッシュオブジェクトへのポインタ配列	
		std::vector<float> vectorDistance; //判定用距離配列
	};
}

/*ディテールの異なる複数のメッシュオブジェクトへのポインタを持つ。
1つの物体に対してこのLODクラスのインスタンスを1つ作り、RegistMeshObj()関数で
メッシュオブジェクトを登録していく。描画時には、GetpMeshObjFromDistance()関数へ
カメラ位置からオブジェクトまでの距離を渡し、距離に応じたディテールの
メッシュオブジェクトを得て描画する。

このクラスはメッシュオブジェクトのポインタを持つだけで、実体を作り出したり
管理したり削除する機能は今のところ持たない。なので、デストラクタを呼び出しても
登録されたメッシュオブジェクト自体は削除されないのでメモリリーク等に対する
注意が必要である。*/
