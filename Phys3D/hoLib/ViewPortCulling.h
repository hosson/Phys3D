//視錐台カリングクラス
#pragma once
#include <d3d9.h>
#include <d3dx9.h>

namespace ho
{
	class ViewPortCulling
	{
	public:
		ViewPortCulling(LPDIRECT3DDEVICE9 pD3DDevice); //コンストラクタ
		~ViewPortCulling(); //デストラクタ

		void Update(float DrawDistance = 1.0f); //視錐台を更新
		void Update(::D3DXMATRIX *pmtrxView, ::D3DXMATRIX *pmtrxProj, float DrawDistance = 1.0f); //任意のビュー行列と射影行列により視錐台を更新
		bool Judge(::D3DXVECTOR3 &vecWorldPos, float Radius); //カリング判定
		void DrawViewPortWireFrame(DWORD Color); //現在の視錐台カリングの範囲をワイヤーフレームで描画

		//アクセッサ
		const D3DXVECTOR3 GetvecNearLeftTop() const { return vecNear[0]; }
		const D3DXVECTOR3 GetvecNearRightTop() const { return vecNear[1]; }
		const D3DXVECTOR3 GetvecNearLeftBottom() const { return vecNear[2]; }
		const D3DXVECTOR3 GetvecNearRightBottom() const { return vecNear[3]; }
		const D3DXVECTOR3 GetvecFarLeftTop() const { return vecFar[0]; }
		const D3DXVECTOR3 GetvecFarRightTop() const { return vecFar[1]; }
		const D3DXVECTOR3 GetvecFarLeftBottom() const { return vecFar[2]; }
		const D3DXVECTOR3 GetvecFarRightBottom() const { return vecFar[3]; }
		const D3DXVECTOR3 &GetvecNFar() const { return vecNFar; }
		const D3DXVECTOR3 &GetvecNNear() const { return vecNNear; }
		const D3DXVECTOR3 &GetvecNLeft() const { return vecNLeft; }
		const D3DXVECTOR3 &GetvecNRight() const { return vecNRight; }
		const D3DXVECTOR3 &GetvecNTop() const { return vecNTop; }
		const D3DXVECTOR3 &GetvecNBottom() const { return vecNBottom; }
	private:
		LPDIRECT3DDEVICE9 pD3DDevice;
		D3DXVECTOR3 vecNear[4], vecFar[4]; //視錐台の8点の座標
		D3DXVECTOR3 vecNFar, vecNNear, vecNLeft, vecNRight, vecNTop, vecNBottom; //法線ベクトル

		void CalcNormal(); //視錐台の8点の座標から法線ベクトルを計算
	};
}

/*
1つのプログラムで1つだけオブジェクトを作成する。
各フレームの最初にデバイスに座標変換行列が設定された後で Update() メソッドを一度だけ呼び出す。
引数は0.0f～1.0fの範囲を取り、0に近いほど近距離しか描画されなくなる。
これで内部の視錐台の位置が設定されるので、後は描画ごとに Judge() メソッドを呼び出して判定する。
*/
