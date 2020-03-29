//更新と描画を司る、アプリケーションの核となるクラス
#pragma once

#include <Windows.h>
#include <vector>
#include <boost\shared_ptr.hpp>
#include <DirectXMath.h>

class App;
class ShaderSimple;
class Shader2DTexture;
class ElementManager;

namespace ho
{
	class Profiler;
	class MultiThread;
}

namespace Phys3D
{
	class Phys3D;
}

class AppCore
{
public:
	AppCore(const std::shared_ptr<App> &spAppObj); //コンストラクタ
	~AppCore(); //デストラクタ

	void Init(const std::weak_ptr<AppCore> &wpThisObj); //初期化

	void Update(); //更新
	void Draw(); //描画

	//アクセッサ
	const std::weak_ptr<AppCore> &getwpThisObj() const { return wpThisObj; }
	const std::shared_ptr<Phys3D::Phys3D> &getspPhys3DObj() const { return spPhys3DObj; }
private:
	std::weak_ptr<AppCore> wpThisObj;
	const std::shared_ptr<App> &spAppObj; //アプリケーションオブジェクト
	std::shared_ptr<ShaderSimple> spShaderSimpleObj; //シンプルシェーダオブジェクト
	std::shared_ptr<Shader2DTexture> spShader2DTextureObj; //2Dテクスチャを描画するシェーダオブジェクト
	std::shared_ptr<ho::Profiler> spProfilerObj; //プロファイラオブジェクト
	std::shared_ptr<ho::MultiThread> spMultiThreadObj; //マルチスレッドオブジェクト
	std::shared_ptr<Phys3D::Phys3D> spPhys3DObj; //物理演算オブジェクト
	std::shared_ptr<ElementManager> spElementManagerObj; //物体管理オブジェクト

	void CreateVertices(std::vector<DirectX::XMFLOAT3> &vectorvecVertex); //現在の物体から描画用の頂点情報を生成
};