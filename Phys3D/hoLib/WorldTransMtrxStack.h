#pragma once

#include <d3dx9.h>
#include <stack>
#include <list>
#include "MyAllocateHierarchy.h"

namespace ho
{
	class WorldTransMtrxStack
	{
	public:
		WorldTransMtrxStack(D3DXMATRIX *pWorldTransMatrix) { this->WorldTransMatrix = *pWorldTransMatrix; } //コンストラクタ

		void UpdateFrame(D3DXFRAME_WORLD *pFrame); //ワールド変換行列の更新開始

		//アクセッサ
		std::list<D3DXFRAME_WORLD *> *GetplistpDrawFrame() { return &listpDrawFrame; }
	private:
		D3DXMATRIX WorldTransMatrix; //オブジェクト中心のローカルからワールドへ変換する行列
		std::stack<D3DXMATRIX *> stackMatrix; //ワールド行列のスタック
		std::list<D3DXFRAME_WORLD *> listpDrawFrame; //描画フレームリスト

		void CalcFrameWorldMatrix(D3DXFRAME_WORLD *pFrame); //フレームのワールド行列計算用の再帰関数
	};
}
