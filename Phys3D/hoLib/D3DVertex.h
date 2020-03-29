//Direct3DのDrawPrimitive系用頂点データ

#pragma once
#include <Windows.h>
#include "Direct3D.h"
#include "Vector.h"

namespace ho {
	//3次元の頂点フォーマット構造体
	class D3DVERTEX
	{
	public:
		//コンストラクタ
		D3DVERTEX() {}
		D3DVERTEX(float x, float y, float z, float nx, float ny, float nz, DWORD diff, DWORD spec, float tu, float tv)
		{ SetVertex(x, y, z, nx, ny, nz, diff, spec, tu, tv); }

		float x, y, z; //頂点座標
		float nx, ny, nz; //法線ベクトル
		DWORD diff; //ディフューズ色
		DWORD spec; //スペキュラ色
		float tu, tv; //テクスチャ座標

		const static WORD IndexSquare[6]; //4角形の頂点インデックス
		const static DWORD FVF; //頂点フォーマット
		const static D3DVERTEXELEMENT9 ve[6]; //頂点情報宣言

		void SetVertex(float x, float y, float z, float nx, float ny, float nz, DWORD diff, DWORD spec, float tu, float tv); //頂点情報をセット
		void SetVertex(::D3DXVECTOR3 *pvecVt, ::D3DXVECTOR3 *pvecNormal, DWORD diff, DWORD spec, float tu, float tv); //頂点情報をセット（D3DXVECTOR3使用）
		void SetNormalLineVecFromD3DXVECTOR3(::D3DXVECTOR3 vecN) { nx = vecN.x; ny = vecN.y; nz = vecN.z; } //D3DXVECTOR3型から法線ベクトルを設定

		//演算子
		operator D3DXVECTOR3() { return D3DXVECTOR3(x, y, z); } //D3DXVECTOR3型で得る
		operator VECTOR3DOUBLE() { return VECTOR3DOUBLE(x, y, z); }

		//図形作成
		static void CreateLine(D3DVERTEX *pOut, D3DXVECTOR3 *pvecBegin, D3DXVECTOR3 *pvecEnd, D3DXVECTOR3 *pvecNormal, float Width, DWORD diff, DWORD spec); //始点、終点、法線ベクトル、幅から4頂点の線を作成
	};

	//2D用頂点フォーマット構造体
	class D3DVERTEX_2D
	{
	public:
		float x, y, z; //頂点座標
		float rhw; //常に1.0
		DWORD diff;
		float tu, tv; //テクスチャ座標

		void SetVertex(float x, float y, float z, DWORD diff, float tu, float tv); //2Dの頂点情報をセット
		void SetCoordinate(D3DXVECTOR2 &vec) { this->x = vec.x; this->y = vec.y; }
		void SetCoordinate(VECTOR2DOUBLE &vec) { this->x = (float)vec.x; this->y = (float)vec.y; }
		static void SetVertexFromRECT(D3DVERTEX_2D *lpVt, RECT *lpRect, float z, DWORD Color); //RECT構造体から4つの頂点情報を設定
		static void Offset(D3DVERTEX_2D *pVt, int Number); //座標を-0.5fずらす

		const static DWORD FVF; //頂点フォーマット
		const static D3DVERTEXELEMENT9 ve[4]; //頂点情報宣言

		//演算子
		operator VECTOR2DOUBLE() { return VECTOR2DOUBLE(x, y); }
	};

}