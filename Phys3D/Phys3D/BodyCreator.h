//物体生成クラス
#pragma once
#include <memory>
#include "../hoLib/num/vector.h"
#include "../hoLib/num/Quaternion.h"
#include "../hoLib/num/Matrix.h"
#include <vector>
#include "MassPoint.h"

using namespace ho::num;

namespace Phys3D
{
	class Body;

	class BodyCreator
	{
	public:
		BodyCreator() {} //コンストラクタ
		~BodyCreator() {} //デストラクタ

		std::shared_ptr<Body> CreateBody(vec3d vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecOmega, std::vector<MASSPOINT> vectorMassPoint, std::vector<vec3d> vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const double &e, const double &mu, const bool &Fixed); //物体を生成する
		std::shared_ptr<Body> CreateBody(vec3d vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecOmega, const double &Density, std::vector<vec3d> vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const double &e, const double &mu, const bool &Fixed); //物体を生成する
	private:
		Matrix GetmtrxInertia(const std::vector<MASSPOINT> &vectorMassPoint, const bool &Fixed); //質点データから慣性モーメントテンソルを計算する
		Matrix GetmtrxInertia(const double &Density, const std::vector<vec3d> &vectorvecVertex, const std::vector<int> &vectorVtFaceIndex, const bool &Fixed); //密度とメッシュ形状から慣性モーメントテンソルを計算する
		std::vector<int> GetvectorVtEdgeIndex(const std::vector<int> &vectorVtFaceIndex); //面の頂点インデックスから辺の頂点インデックスを抽出
	};
}

/*Body オブジェクトを生成するためのクラス。
Body オブジェクト生成時のパラメータには様々なパターンがあるため、
その差を吸収し、Body クラスのコンストラクタを1つにまとめる役割がある。
*/