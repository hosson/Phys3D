#include "WorldTransMtrxStack.h"

namespace ho
{
	//ワールド変換行列の更新開始
	void WorldTransMtrxStack::UpdateFrame(D3DXFRAME_WORLD *pFrame) 
	{
		//スタック初期化
		while (!stackMatrix.empty())
			stackMatrix.pop();

		listpDrawFrame.clear(); //描画フレームリスト初期化
	
		stackMatrix.push(&WorldTransMatrix); //ワールド変換行列をスタックに積む

		CalcFrameWorldMatrix(pFrame); //ルートフレームからワールド変換行列を連続計算

		return;
	}

	//フレームのワールド行列計算用の再帰関数
	void WorldTransMtrxStack::CalcFrameWorldMatrix(D3DXFRAME_WORLD *pFrame) 
	{
		//現在のスタック先頭にあるワールド変換行列を参照
		D3DXMATRIX *pStackMtrx = stackMatrix.top();

		//引数のフレームに対応するワールド変換行列を計算
		D3DXMatrixMultiply(&pFrame->WorldTransMatrix, &pFrame->TransformationMatrix, pStackMtrx);

		//フレームにメッシュコンテナがあれば、このフレームをリストに追加する
		if (pFrame->pMeshContainer)
			listpDrawFrame.push_back(pFrame);

		//子フレームがあればスタックを積んで、子フレームのワールド座標変換の計算へ
		if (pFrame->pFrameFirstChild)
		{
			stackMatrix.push(&pFrame->WorldTransMatrix); //スタックに積む
			CalcFrameWorldMatrix((D3DXFRAME_WORLD *)pFrame->pFrameFirstChild); //子フレームの計算
			stackMatrix.pop(); //子フレームの計算が終わったのでスタックを1つ外す
		}

		//兄弟フレームがある場合は現在のスタックを使って計算する
		if (pFrame->pFrameSibling)
			CalcFrameWorldMatrix((D3DXFRAME_WORLD *)pFrame->pFrameSibling);

		return;
	}
}
