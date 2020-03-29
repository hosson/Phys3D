//物体クラス
#pragma once
#include <memory>
#include <list>
#include "Phys3D\Body.h"
#include <DirectXMath.h>
#include "ShaderSimple_base.h"

class AppCore;
class Element;

using namespace DirectX;

class ElementManager
{
public:
	ElementManager(const std::weak_ptr<AppCore> &wpAppCoreObj); //コンストラクタ
	~ElementManager(); //デストラクタ

	//アクセッサ
	const std::list<std::shared_ptr<Element>> &getlistspElementObj() const { return listspElementObj; }
private:
	std::weak_ptr<AppCore> wpAppCoreObj;
	std::list<std::shared_ptr<Element>> listspElementObj; //物体リスト
};

class Element
{
public:
	Element(const std::shared_ptr<Phys3D::Body> &spBodyObj, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vectorVtIndex, const XMFLOAT4 &vecColor); //コンストラクタ
	~Element(); //デストラクタ

	//アクセッサ
	const std::shared_ptr<Phys3D::Body> &getspBodyObj() const { return spBodyObj; }
	const std::vector<ShaderSimple_base::CustomVertex> &getvectorvecVertex() const { return vectorvecVertex; }
private:
	std::shared_ptr<Phys3D::Body> spBodyObj; //物理エンジン内の物体オブジェクトへのポインタ
	std::vector<ShaderSimple_base::CustomVertex> vectorvecVertex; //頂点配列
};