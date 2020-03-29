#include "Shader.h"
#include "Error.h"

namespace ho
{
	namespace D3D11
	{
		//�R���X�g���N�^
		Shader_Base::Shader_Base(const std::shared_ptr<D3D11> &spD3D11Obj)
			: spD3D11Obj(spD3D11Obj)
		{
		}

		//�f�X�g���N�^
		Shader_Base::~Shader_Base()
		{}

		//1�t���[�����̕`��
		void Shader_Base::Draw()
		{
			return;
		}

		//�t�@�C������V�F�[�_���R���p�C������
		void Shader_Base::CompileShaderFromFile(const tstring &strFilename, LPCSTR pEntryPoint, LPCSTR pShaderModel, ho::ComPtr<ID3DBlob *> &cpBlobObj)
		{
			HRESULT hr;
			unsigned int Flags1 = D3DCOMPILE_ENABLE_STRICTNESS;
			ho::ComPtr<ID3DBlob *> cpErrorBlobObj;

#ifdef _DEBUG
			Flags1 |= D3DCOMPILE_DEBUG;
#else
			Flags1 |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

			hr = D3DCompileFromFile(
				strFilename.c_str(),
				NULL,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				pEntryPoint,
				pShaderModel,
				Flags1,
				0,
				cpBlobObj.ToCreator(),
				cpErrorBlobObj.ToCreator()
				);
			if (FAILED(hr))
			{
				if (cpErrorBlobObj.GetpInterface() != 0)
				{
					ERR(true, ho::tstring(ho::CharToWChar((char *)cpErrorBlobObj->GetBufferPointer())), __WFILE__, __LINE__);
				}
				ERR(true, D3D11::GetErrorStr(hr), __WFILE__, __LINE__);
			}

			return;
		}
	}
}