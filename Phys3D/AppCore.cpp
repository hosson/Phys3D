#include "AppCore.h"
#include "App.h"
#include "hoLib\D3D11.h"
#include "hoLib\Error.h"
#include "ShaderSimple.h"
#include "Shader2DTexture.h"
#include <vector>
#include "hoLib\Profiler.h"
#include "hoLib\MultiThread.h"
#include "Phys3D\Phys3D.h"
#include "Element.h"

//コンストラクタ
AppCore::AppCore(const std::shared_ptr<App> &spAppObj)
	: spAppObj(spAppObj)
{
}

//デストラクタ
AppCore::~AppCore()
{
}

//初期化
void AppCore::Init(const std::weak_ptr<AppCore> &wpThisObj)
{
	this->wpThisObj = wpThisObj;

	this->spShaderSimpleObj.reset(new ShaderSimple(spAppObj->getspD3D11Obj())); //シンプルシェーダオブジェクト生成
	this->spShader2DTextureObj.reset(new Shader2DTexture(spAppObj->getspD3D11Obj())); //2Dテクスチャ描画シェーダオブジェクト生成

	this->spProfilerObj.reset(new ho::Profiler(10)); //プロファイラオブジェクト生成
	this->spMultiThreadObj.reset(new ho::MultiThread(4)); //マルチスレッドオブジェクト生成

	this->spPhys3DObj = ho::sp_init<Phys3D::Phys3D>(new Phys3D::Phys3D(1.0 / 60.0, 50)); //物理演算オブジェクト生成
	this->spElementManagerObj.reset(new ElementManager(wpThisObj)); //物体管理オブジェクト生成

	return;
}

//更新
void AppCore::Update()
{
	this->spPhys3DObj->Update(); //物理演算オブジェクトの1フレーム毎の処理

	return;
}

//描画
void AppCore::Draw()
{
	std::shared_ptr<ho::D3D11::D3D11> spDirect3D11Obj = this->spAppObj->getspD3D11Obj();

	spDirect3D11Obj->DrawFrameBegin(); //1フレームの描画開始

	//シェーダの描画
	{
		//エレメントリストを取得
		const std::list<std::shared_ptr<Element>> &listspElementObj = this->spElementManagerObj->getlistspElementObj();

		for (auto itr = listspElementObj.begin(); itr != listspElementObj.end(); itr++) //エレメントを走査
		{
			const std::shared_ptr<Phys3D::Body> &spBodyObj = (*itr)->getspBodyObj();

			XMMATRIX mtrxWorld = spBodyObj->GetCoordTransMatrix(); //描画用の座標変換行列を取得

			this->spShaderSimpleObj->DrawUp(
				(ShaderSimple_base::CustomVertex *const)&(*itr)->getvectorvecVertex().at(0),
				(*itr)->getvectorvecVertex().size(),
				NULL,
				0,
				mtrxWorld);
		}

		//this->spShaderSimpleObj->Draw();
		//this->spShader2DTextureObj->Draw();
	}

	spDirect3D11Obj->DrawFrameEnd(); //1フレームの描画終了

	return;
}

//現在の物体から描画用の頂点情報を生成
void AppCore::CreateVertices(std::vector<DirectX::XMFLOAT3> &vectorvecVertex)
{
	/*
	const std::list<std::shared_ptr<Element>> &listspElementObj = this->spElementManagerObj->getlistspElementObj();

	std::for_each(listspElementObj.cbegin(), listspElementObj.cend(), [&](const std::shared_ptr<Element> &p)
	{
		if (auto p2 = p->getwpElementObj().lock())
		{
			const std::vector<vec2d> &vectorvecShapeWorld = p2->getvectorvecShapeWorld(); //ワールド座標での形状データを取得

			for (unsigned int i = 1; i < vectorvecShapeWorld.size() - 1; i++) //形状データを三角形状に走査
			{
				vectorvecVertex.push_back(vectorvecShapeWorld.at(0).Getvec3d_XY(0));
				vectorvecVertex.push_back(vectorvecShapeWorld.at(i).Getvec3d_XY(0));
				vectorvecVertex.push_back(vectorvecShapeWorld.at(i + 1).Getvec3d_XY(0));
			}
		}
	});
	*/

	return;
}