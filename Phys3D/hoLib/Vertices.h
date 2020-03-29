//Direct3D用の頂点バッファとインデックスバッファをラッピングしたクラス

#pragma once

#include <d3d9.h>
#pragma comment (lib, "d3d9.lib")
#include <vector>
#include "D3DVertex.h"

namespace ho {
	class Vertices
	{
	public:
		Vertices(); //コンストラクタ
		~Vertices(); //デストラクタ

		void Add(ho::D3DVERTEX *pD3DVERTEX, int Vertices, WORD *pIndex, int Indexes, UINT Primitives); //頂点とインデックスを追加
		void CreateBuffer(LPDIRECT3DDEVICE9 pD3DDevice, DWORD Usage = 0, D3DPOOL Pool = D3DPOOL_MANAGED); //頂点バッファをインデックスバッファを生成
		void Clear(); //追加中の頂点とインデックスを削除
		void Release(); //頂点バッファをインデックスバッファを解放
		void Draw(LPDIRECT3DDEVICE9 pD3DDevice, D3DXMATRIX *pmtrx); //描画
		float GetMaxRadius(); //作成済み頂点バッファから最大半径を計算して取得する
	private:
		IDirect3DVertexBuffer9 *pVertexBuffer; //頂点バッファへのポインタ
		IDirect3DIndexBuffer9 *pIndexBuffer; //インデックスバッファへのポインタ

		std::vector<ho::D3DVERTEX> vectorD3DVERTEX; //頂点配列
		std::vector<WORD> vectorIndex; //頂点インデックス
		UINT Primitives; //ポリゴン数
	};
}

/*Direct3Dの頂点バッファをインデックスバッファをラッピングしている。
D3DVERTEXクラスの頂点フォーマットを使用する。

コンストラクタで生成した時点ではまだバッファは作られない。
Add()メソッドで頂点を自由に追加し終わった時点で、CreateBuffer()メソッドを行うと、
頂点バッファとインデックスバッファが生成され、Draw()メソッドによる描画が可能となる。
Clear()メソッドで追加中の内部頂点情報を削除、Release()メソッドで各バッファを解放する。
デストラクタでは解放が自動的に行われる。

Add()メソッドによる頂点インデックスの追加が一度も行われなかった場合には、
DrawIndexedPrimitiveではなくDrawPrimitiveにより頂点インデックス無しの描画が行われる。
*/