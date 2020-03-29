#pragma once
#include "D3D11.h"
//各シェーダの基底クラス
//バーテックスシェーダ、ジオメトリシェーダ、ピクセルシェーダ、頂点バッファ、定数バッファをひとまとめにしたクラス

namespace ho
{
	namespace D3D11
	{
		class Shader_Base
		{
		public:
			Shader_Base(const std::shared_ptr<D3D11> &spD3D11Obj); //コンストラクタ
			virtual ~Shader_Base(); //デストラクタ

			virtual void Draw(); //1フレーム毎の描画
		protected:
			void CompileShaderFromFile(const tstring &strFilename, LPCSTR pEntryPoint, LPCSTR pShaderModel, ho::ComPtr<ID3DBlob *> &cpBlobObj); //ファイルからシェーダをコンパイルする

			std::shared_ptr<D3D11> spD3D11Obj;
		};
	}
}