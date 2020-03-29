#include "Element.h"
#include "AppCore.h"
#include "Phys3D\Phys3D.h"
#include "hoLib\num\vector.h"
#include "Phys3D\MassPoint.h"
#include "hoLib\num\Quaternion.h"
#include "hoLib\Random.h"

using namespace ho::num;

//コンストラクタ
ElementManager::ElementManager(const std::weak_ptr<AppCore> &wpAppCoreObj)
	: wpAppCoreObj(wpAppCoreObj)
{
	ho::xorShift xorShiftObj;

	const std::shared_ptr<Phys3D::Phys3D> &spPhys3DObj = wpAppCoreObj.lock()->getspPhys3DObj();

	//物体の生成：床
	{
		std::vector<Phys3D::MASSPOINT> vectorMassPoint; //質点配列
		std::vector<vec3d> vectorvecVertex; //頂点配列
		std::vector<int> vectorVtIndex; //頂点インデックス

		//質点を設定
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(-0.5f, 1.0f, 0.5f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(0.5f, 1.0f, 0.5f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(-0.5f, 1.0f, -0.5f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(0.5f, 1.0f, -0.5f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(-1.0f, -1.0f, 1.0f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(1.0f, -1.0f, 1.0f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(-1.0f, -1.0f, -1.0f), 10.0));
		vectorMassPoint.push_back(Phys3D::MASSPOINT(vec3d(1.0f, -1.0f, -1.0f), 10.0));

		//頂点配列
		vectorvecVertex.push_back(vec3d(-50.0f, 1.0f, 50.0f));
		vectorvecVertex.push_back(vec3d(50.0f, 1.0f, 50.0f));
		vectorvecVertex.push_back(vec3d(-50.0f, 1.0f, -50.0f));
		vectorvecVertex.push_back(vec3d(50.0f, 1.0f, -50.0f));
		vectorvecVertex.push_back(vec3d(-50.0f, -1.0f, 50.0f));
		vectorvecVertex.push_back(vec3d(50.0f, -1.0f, 50.0f));
		vectorvecVertex.push_back(vec3d(-50.0f, -1.0f, -50.0f));
		vectorvecVertex.push_back(vec3d(50.0f, -1.0f, -50.0f));

		//頂点インデックス
		{
			//上
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(3);
			//下
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(6);
			//前
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(6);
			vectorVtIndex.push_back(6);
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(7);
			//奥
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(4);
			//左
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(6);
			//右
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(5);
		}

		//物理エンジン内に物体を生成して、そのポインタを得る
		std::shared_ptr<Phys3D::Body> spBodyObj = spPhys3DObj->CreateBody(
			vec3d(0, 0.0, 0),							//位置
			vec3d(0, 0, 0),							//速度
			Quaternion(vec3d(0.0f, 0.0, 1.0), 0.0),//回転角度（姿勢）
			vec3d(0, 0.0, 0),						//角速度
			vectorMassPoint,						//質点配列
			vectorvecVertex,						//頂点配列
			vectorVtIndex,							//頂点インデックス
			0.2,									//反発係数
			0.5,									//摩擦係数
			true);

		//エレメントを生成してリストに追加
		this->listspElementObj.push_back(std::shared_ptr<Element>(new Element(spBodyObj, vectorvecVertex, vectorVtIndex, XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f))));
	}

	/*
	//物体の生成：レンガ
	{
		const double size = 1.0f;

		std::vector<Phys3D::MASSPOINT> vectorMassPoint; //質点配列
		std::vector<vec3d> vectorvecVertex; //頂点配列
		std::vector<int> vectorVtIndex; //頂点インデックス

		//頂点配列
		vectorvecVertex.push_back(vec3d(-size * 2.0, size, size));
		vectorvecVertex.push_back(vec3d(size * 2.0, size, size));
		vectorvecVertex.push_back(vec3d(-size * 2.0, size, -size));
		vectorvecVertex.push_back(vec3d(size * 2.0, size, -size));
		vectorvecVertex.push_back(vec3d(-size * 2.0, -size, size));
		vectorvecVertex.push_back(vec3d(size * 2.0, -size, size));
		vectorvecVertex.push_back(vec3d(-size * 2.0, -size, -size));
		vectorvecVertex.push_back(vec3d(size * 2.0, -size, -size));

		//頂点インデックス
		{
			//上
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(3);
			//下
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(6);
			//前
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(6);
			vectorVtIndex.push_back(6);
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(7);
			//奥
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(5);
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(4);
			//左
			vectorVtIndex.push_back(0);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(4);
			vectorVtIndex.push_back(2);
			vectorVtIndex.push_back(6);
			//右
			vectorVtIndex.push_back(3);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(7);
			vectorVtIndex.push_back(1);
			vectorVtIndex.push_back(5);
		}

		const int iMax = 10, jMax = 10;
		for (int i = 0; i < iMax; i++)
		{
			for (int j = 0; j < jMax; j++)
			{
				//物理エンジン内に物体を生成して、そのポインタを得る
				std::shared_ptr<Phys3D::Body> spBodyObj = spPhys3DObj->CreateBody(
					vec3d((jMax * -0.5 + j + (i % 2) * 0.5) * size * 4.0, 2.0 + i * size * 2.0, 0),//位置
					vec3d(0, 0, 0),								//速度
					Quaternion(vec3d(0.0f, 0.0, 1.0), 0.0),		//回転角度（姿勢）
					vec3d(0, 0.0, 0.0),							//角速度
					1.0,							//質点配列
					vectorvecVertex,							//頂点配列
					vectorVtIndex,								//頂点インデックス
					0.2,										//反発係数
					0.5,										//摩擦係数
					false);

				//エレメントを生成してリストに追加
				this->listspElementObj.push_back(std::shared_ptr<Element>(new Element(spBodyObj, vectorvecVertex, vectorVtIndex, XMFLOAT4((float)xorShiftObj.get(0.0, 1.0), (float)xorShiftObj.get(0.0, 1.0), (float)xorShiftObj.get(0.0, 1.0), (float)xorShiftObj.get(1.0, 1.0)))));
			}
		}
	}
	*/


	
	//物体の生成：積み上げる箱
	{
	const double size = 1.0f;

	std::vector<Phys3D::MASSPOINT> vectorMassPoint; //質点配列
	std::vector<vec3d> vectorvecVertex; //頂点配列
	std::vector<int> vectorVtIndex; //頂点インデックス

	//頂点配列
	vectorvecVertex.push_back(vec3d(-size, size, size));
	vectorvecVertex.push_back(vec3d(size, size, size));
	vectorvecVertex.push_back(vec3d(-size, size, -size));
	vectorvecVertex.push_back(vec3d(size, size, -size));
	vectorvecVertex.push_back(vec3d(-size, -size, size));
	vectorvecVertex.push_back(vec3d(size, -size, size));
	vectorvecVertex.push_back(vec3d(-size, -size, -size));
	vectorvecVertex.push_back(vec3d(size, -size, -size));

	//頂点インデックス
	{
	//上
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(3);
	//下
	vectorVtIndex.push_back(5);
	vectorVtIndex.push_back(4);
	vectorVtIndex.push_back(7);
	vectorVtIndex.push_back(7);
	vectorVtIndex.push_back(4);
	vectorVtIndex.push_back(6);
	//前
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(6);
	vectorVtIndex.push_back(6);
	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(7);
	//奥
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(5);
	vectorVtIndex.push_back(5);
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(4);
	//左
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(4);
	vectorVtIndex.push_back(4);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(6);
	//右
	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(7);
	vectorVtIndex.push_back(7);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(5);
	}

	for (int i = 0; i < 7; i++)
	//for (int i = 7; i >= 0; i--)
	{
	//物理エンジン内に物体を生成して、そのポインタを得る
	std::shared_ptr<Phys3D::Body> spBodyObj = spPhys3DObj->CreateBody(
	vec3d(0.0 + i * 0.0000, 2.0 + i * size * 2.0, 0),						//位置
	vec3d(0, 0, 0),								//速度
	Quaternion(vec3d(0.0f, 0.0, 1.0), 0.0),		//回転角度（姿勢）
	vec3d(0, 0.0, 0.0),							//角速度
	1.0,							//質点配列
	vectorvecVertex,							//頂点配列
	vectorVtIndex,								//頂点インデックス
	0.2,										//反発係数
	0.5,										//摩擦係数
	false);

	//エレメントを生成してリストに追加
	this->listspElementObj.push_back(std::shared_ptr<Element>(new Element(spBodyObj, vectorvecVertex, vectorVtIndex, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f))));
	}
	}
	

/*
	//物体の生成
	{
	const double size = 1.0f;

	std::vector<vec3d> vectorvecVertex; //頂点配列
	std::vector<int> vectorVtIndex; //頂点インデックス

	//頂点配列
	vectorvecVertex.push_back(vec3d(-size, size, 0));
	vectorvecVertex.push_back(vec3d(0, size, size));
	vectorvecVertex.push_back(vec3d(size, size, 0));
	vectorvecVertex.push_back(vec3d(0, size, -size));
	vectorvecVertex.push_back(vec3d(0, -size, 0));

	//頂点インデックス
	{
	//上
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(3);

	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(4);

	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(1);
	vectorVtIndex.push_back(4);

	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(2);
	vectorVtIndex.push_back(4);

	vectorVtIndex.push_back(0);
	vectorVtIndex.push_back(3);
	vectorVtIndex.push_back(4);
	}

	//物理エンジン内に物体を生成して、そのポインタを得る
	std::shared_ptr<Phys3D::Body> spBodyObj = spPhys3DObj->CreateBody(
	vec3d(0.0, size * 2.0, 0),						//位置
	vec3d(0, 0, 0),								//速度
	Quaternion(vec3d(0.0f, 0.0, 1.0), -0.1),		//回転角度（姿勢）
	vec3d(0, 40.0, 0.0),							//角速度
	1.0,							//質点配列
	vectorvecVertex,							//頂点配列
	vectorVtIndex,								//頂点インデックス
	0.8,										//反発係数
	0.1,										//摩擦係数
	false);

	//エレメントを生成してリストに追加
	this->listspElementObj.push_back(std::shared_ptr<Element>(new Element(spBodyObj, vectorvecVertex, vectorVtIndex, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f))));
	}
	*/

}

//デストラクタ
ElementManager::~ElementManager()
{}





//コンストラクタ
Element::Element(const std::shared_ptr<Phys3D::Body> &spBodyObj, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vectorVtIndex, const XMFLOAT4 &vecColor)
	: spBodyObj(spBodyObj)
{
	for (unsigned int i = 0; i < vectorVtIndex.size(); i += 3) //ポリゴン単位で頂点インデックスを走査
	{
		//外積により法線ベクトルを計算
		XMVECTOR vecNormal = XMVector3Normalize(XMVector3Cross(XMVectorSubtract(XMLoadFloat3(&(XMFLOAT3)vectorvecVertex.at(vectorVtIndex.at(i + 1))), XMLoadFloat3(&(XMFLOAT3)vectorvecVertex.at(vectorVtIndex.at(i)))), XMVectorSubtract(XMLoadFloat3(&(XMFLOAT3)vectorvecVertex.at(vectorVtIndex.at(i + 2))), XMLoadFloat3(&(XMFLOAT3)vectorvecVertex.at(vectorVtIndex.at(i))))));

		for (int j = 0; j < 3; j++) //ポリゴン内の3つの頂点を走査
		{
			ShaderSimple_base::CustomVertex cv;
			cv.position = vectorvecVertex.at(vectorVtIndex.at(i + j)); //vec3d から XMFLOAT3 に変換しながらコピー
			cv.Color = vecColor; //色
			XMStoreFloat3(&cv.Normal, vecNormal);

			this->vectorvecVertex.push_back(cv);
		}
	}
}

//デストラクタ
Element::~Element()
{}