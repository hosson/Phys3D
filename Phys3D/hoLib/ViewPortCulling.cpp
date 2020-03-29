#include "ViewPortCulling.h"
#include "D3DVertex.h"

namespace ho
{
	//コンストラクタ
	ViewPortCulling::ViewPortCulling(LPDIRECT3DDEVICE9 pD3DDevice)
	{
		this->pD3DDevice = pD3DDevice;
	}

	//デストラクタ
	ViewPortCulling::~ViewPortCulling()
	{
	}

	//視錐台を更新
	void ViewPortCulling::Update(float DrawDistance)
	{
		D3DVIEWPORT9 ViewPort; //ビューポート
		pD3DDevice->GetViewport(&ViewPort); //デバイスに設定されているビューポートを取得

		//ViewPortから4点の座標を得る
		float x1 = (float)ViewPort.X;
		float y1 = (float)ViewPort.Y;
		float x2 = float(ViewPort.X + ViewPort.Width);
		float y2 = float(ViewPort.Y + ViewPort.Height);

		//視錐台の8点の座標
		//この時点ではスクリーン上の座標が格納される
		vecNear[0] = D3DXVECTOR3(x1, y1, ViewPort.MinZ); //左下→左上
		vecNear[1] = D3DXVECTOR3(x2, y1, ViewPort.MinZ); //右下→右上
		vecNear[2] = D3DXVECTOR3(x1, y2, ViewPort.MinZ); //左上→左下
		vecNear[3] = D3DXVECTOR3(x2, y2, ViewPort.MinZ); //右上→右下
		vecFar[0] = D3DXVECTOR3(x1, y1, ViewPort.MaxZ * DrawDistance); //左下→左上
		vecFar[1] = D3DXVECTOR3(x2, y1, ViewPort.MaxZ * DrawDistance); //右下→右上
		vecFar[2] = D3DXVECTOR3(x1, y2, ViewPort.MaxZ * DrawDistance); //左上→左下
		vecFar[3] = D3DXVECTOR3(x2, y2, ViewPort.MaxZ * DrawDistance); //右上→右下

		D3DXMATRIX mtrxProj; //投射行列
		pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtrxProj); //デバイスから投射行列を取得

		D3DXMATRIX mtrxView; //ビュー行列
		pD3DDevice->GetTransform(D3DTS_VIEW, &mtrxView); //デバイスからビュー行列を取得

		D3DXMATRIX mtrxWorld; //ワールド行列
		D3DXMatrixIdentity(&mtrxWorld);

		//スクリーン上の座標をワールド空間上の座標に変換
		for (int i = 0; i < 4; i++)
		{
			D3DXVec3Unproject(&vecNear[i], &vecNear[i], &ViewPort, &mtrxProj, &mtrxView, &mtrxWorld);
			D3DXVec3Unproject(&vecFar[i], &vecFar[i], &ViewPort, &mtrxProj, &mtrxView, &mtrxWorld);
		}

		CalcNormal(); //視錐台の8点の座標から法線ベクトルを計算

		return;
	}

	//任意のビュー行列と射影行列により視錐台を更新
	void ViewPortCulling::Update(D3DXMATRIX *pmtrxView, D3DXMATRIX *pmtrxProj, float DrawDistance) 
	{
		D3DXMATRIX mtrxInvView, mtrxInvProj; //ビュー行列と射影行列の逆行列
		D3DXMatrixInverse(&mtrxInvView, NULL, pmtrxView); //ビュー行列の逆行列を計算
		D3DXMatrixInverse(&mtrxInvProj, NULL, pmtrxProj); //射影行列の逆行列を計算

		vecNear[0] = D3DXVECTOR3(-1.0f, 1.0f, 0.0f); //左上
		vecNear[1] = D3DXVECTOR3(1.0f, 1.0f, 0.0f); //右上
		vecNear[2] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); //左下
		vecNear[3] = D3DXVECTOR3(1.0f, -1.0f, 0.0f); //右下

		vecFar[0] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f); //左上
		vecFar[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f); //右上
		vecFar[2] = D3DXVECTOR3(-1.0f, -1.0f, 1.0f); //左下
		vecFar[3] = D3DXVECTOR3(1.0f, -1.0f, 1.0f); //右下

		D3DXMATRIX mtrx = mtrxInvProj * mtrxInvView; //射影逆行列とビュー逆行列を積算

		//射影空間内の座標をワールド空間内の座標に変換
		for (int i = 0; i < 4; i++)
		{
			D3DXVec3TransformCoord(&vecNear[i], &vecNear[i], &mtrx);
			D3DXVec3TransformCoord(&vecFar[i], &vecFar[i], &mtrx);
		}

		CalcNormal(); //視錐台の8点の座標から法線ベクトルを計算

		return;
	}

	//視錐台の8点の座標から法線ベクトルを計算
	void ViewPortCulling::CalcNormal()
	{
		//奥方向の法線ベクトル
		D3DXVec3Cross(&vecNFar, &(vecFar[1] - vecFar[0]), &(vecFar[0] - vecFar[2]));
		D3DXVec3Normalize(&vecNFar, &vecNFar);

		//手前方向の法線ベクトル
		D3DXVec3Cross(&vecNNear, &(vecNear[1] - vecNear[0]), &(vecNear[2] - vecNear[0]));
		D3DXVec3Normalize(&vecNNear, &vecNNear);

		//左方向の法線ベクトル
		D3DXVec3Cross(&vecNLeft, &(vecNear[2] - vecNear[0]), &(vecFar[0] - vecNear[0]));
		D3DXVec3Normalize(&vecNLeft, &vecNLeft);

		//右方向の法線ベクトル
		D3DXVec3Cross(&vecNRight, &(vecNear[0] - vecNear[2]), &(vecFar[1] - vecNear[1]));
		D3DXVec3Normalize(&vecNRight, &vecNRight);

		//上方向の法線ベクトル
		D3DXVec3Cross(&vecNTop, &(vecNear[0] - vecNear[1]), &(vecFar[0] - vecNear[0]));
		D3DXVec3Normalize(&vecNTop, &vecNTop);

		//下方向の法線ベクトル
		D3DXVec3Cross(&vecNBottom, &(vecNear[1] - vecNear[0]), &(vecFar[2] - vecNear[2]));
		D3DXVec3Normalize(&vecNBottom, &vecNBottom);

		return;
	}


	//カリング判定
	bool ViewPortCulling::Judge(::D3DXVECTOR3 &vecWorldPos, float Radius) 
	{
		//視錐台の法線ベクトルと内外判定
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[0]), &vecNNear) - Radius > 0) //手前の辺が法線ベクトル（視界の外向き）と同じ方向（正の数）の場合
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecFar[0]), &vecNFar) - Radius > 0) //奥辺が法線ベクトル（視界の外向き）と同じ方向（正の数）の場合
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[0]), &vecNLeft) - Radius > 0) //左辺が法線ベクトル（視界の外向き）と同じ方向（正の数）の場合
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[1]), &vecNRight) - Radius > 0) //右辺が法線ベクトル（視界の外向き）と同じ方向（正の数）の場合
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[0]), &vecNTop) - Radius > 0) //上辺が法線ベクトル（視界の外向き）と同じ方向（正の数）の場合
			return false;
		if (D3DXVec3Dot(&(vecWorldPos - vecNear[2]), &vecNBottom) - Radius > 0) //下辺が法線ベクトル（視界の外向き）と同じ方向（正の数）の場合
			return false;

		return true; //カリングされない（境界内に入るので描画対象となる）場合
	}

	//現在の視錐台カリングの範囲をワイヤーフレームで描画
	void ViewPortCulling::DrawViewPortWireFrame(DWORD Color) 
	{
		/*この関数は視錐台カリングの確認用にのみ使う。
		まず本来のカメラ位置をデバイスに設定しUpdateViewPortCulling()を呼び出す。
		そして、視錐台を眺めたい位置にカメラを移動し、通常通りの視錐台カリング判定を行いながらこの関数も呼び出す。*/

		//ViewPortCulling構造体の情報を頂点情報に変換
		D3DVERTEX vt[8];
		for (int i = 0; i < 4; i++)
			vt[i].SetVertex(&vecFar[i], &D3DXVECTOR3(0, 0, 0), Color, Color, 0, 0);
		for (int i = 0; i < 4; i++)
			vt[4 + i].SetVertex(&vecNear[i], &D3DXVECTOR3(0, 0, 0), Color, Color, 0, 0);

		WORD Index[24] = { 0, 1, 1, 3, 3, 2, 2, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 7, 7, 6, 6, 4};

		pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, 12, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX)); //描画

		return;
	}
}