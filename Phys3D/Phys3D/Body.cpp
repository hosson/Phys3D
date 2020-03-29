#include "Body.h"
#include "Contact.h"
#include "Mesh.h"
#include "World.h"
#include "../hoLib/num/Matrix.h"
#include "../hoLib/Error.h"

using namespace ho::num;

namespace Phys3D
{
	//コンストラクタ
	Body::Body(const vec3d &vecPos, const vec3d &vecLinearVelocity, const Quaternion &qtAngle, const vec3d &vecL, const double &Mass, const Matrix &mtrxInertia, const std::shared_ptr<Mesh> &spMeshObj, const double &e, const double &mu, const bool &Fixed)
		: vecPos(vecPos), vecLinearVelocity(vecLinearVelocity), qtAngle(qtAngle), vecL(vecL), Mass(Mass), mtrxInertia(mtrxInertia), spMeshObj(spMeshObj), e(e), mu(mu), Fixed(Fixed), LastUpdateFrame(-1)
	{
		if (this->Fixed) //固定物の場合
			this->MassInv = 0;
		else
			this->MassInv = 1.0 / this->Mass;

		//慣性モーメントテンソルの逆行列を求める
		if (this->Fixed) //固定物の場合
		{
			this->mtrxInertiaInv.Resize(3, 3);
			this->mtrxInertiaInv.InitZero(); //全成分を0
		} else {
			this->mtrxInertiaInv = this->mtrxInertia.CalcInverseMatrix(); //逆行列を計算
		}
		if (this->mtrxInertiaInv.GetxSize() == 0)
			ERR(true, TEXT("逆行列が求まらない慣性モーメントテンソルが指定されました。"), __WFILE__, __LINE__);

		CalcmtrxInertiaNowInv(); //現在時刻（現在姿勢）での慣性テンソルの逆行列を計算する
		this->vecOmega = this->vecL * mtrxInertiaNowInv;//角運動量ベクトルと慣性テンソルの逆行列を積算して、角速度ωを求めておく。

		this->BoundingRadius = spMeshObj->GetBoundingRadius();
	}
	
	//デストラクタ
	Body::~Body() 
	{}

	//グラフィックス用の座標変換行列を取得する
	Matrix Body::GetCoordTransMatrix() const
	{
		Matrix mtrx = this->qtAngle.GetMatrix4x4(); //現在姿勢の回転行列を取得

		//平行移動成分を設定
		mtrx(0, 3) = this->vecPos.x;
		mtrx(1, 3) = this->vecPos.y;
		mtrx(2, 3) = this->vecPos.z;

		return mtrx;
	}

	/*
	//すべての頂点座標をワールド座標で取得
	void Body::GetAllvecVtWorld(std::vector<vec3d> *pvectorvecVtWorld)
	{
		*pvectorvecVtWorld = vectorvecVt; //ローカル座標をコピー

		for (unsigned int i = 0; i < pvectorvecVtWorld->size(); i++)
		{
			qtAngle.TransformVector(&pvectorvecVtWorld->at(i)); //ワールド座標に回転変換
			pvectorvecVtWorld->at(i) += vecPos; //平行移動
		}
		
		return;
	}
	*/

	//移動
	void Body::Move(const double &dt) 
	{
		vecOmega = vecL * mtrxInertiaNowInv;//角運動量ベクトルと慣性テンソルの逆行列を積算して、角速度ωを求める

		vecPos += vecLinearVelocity * dt;

		/*
		{
			//行列で同じ回転をする場合の記述
			vec3d vecOmegaDt = vecOmega * dt;
			Matrix mtrxOmega(vecOmegaDt.GetNormalize(), vecOmegaDt.GetPower());
			Matrix mtrx = this->qtAngle.GetMatrix3x3();
			mtrx = mtrxOmega * mtrx; 
			this->qtAngle = Quaternion(mtrx);
			qtAngle.Normalize(); //クォータニオンを正規化
		}*/
		
		
		//qtAngle += 0.5 * qtAngle * dt * Quaternion(0, vecOmega.x, vecOmega.y, vecOmega.z); //角速度ωに応じて姿勢を変化させる
		qtAngle += 0.5 * Quaternion(0, vecOmega.x, vecOmega.y, vecOmega.z) * dt * qtAngle; //角速度ωに応じて姿勢を変化させる
		qtAngle.Normalize(); //クォータニオンを正規化
	
		CalcmtrxInertiaNowInv(); //姿勢（クォータニオン）が変化したので、現在時刻（現在姿勢）での慣性テンソルの逆行列を計算しておく

		return;
	}

	//現在時刻（現在姿勢）での慣性テンソルの逆行列を計算する
	void Body::CalcmtrxInertiaNowInv()
	{
		if (Fixed) //固定物の場合
		{
			this->mtrxInertiaNowInv.Resize(3, 3);
			this->mtrxInertiaNowInv.InitZero();
		} else {
			//現在の姿勢から回転行列とその転置行列を求める
			Matrix mtrxRotation = qtAngle.GetMatrix3x3(); //現在の姿勢から3X3の回転行列を取得
			Matrix mtrxTransposedRotation = mtrxRotation.CalcTransposedMatrix(); //回転行列の転置行列

			//現在の姿勢での慣性モーメントテンソルの逆行列を求める
			this->mtrxInertiaNowInv = mtrxRotation * mtrxInertiaInv * mtrxTransposedRotation;
		}

		/*
		if (Fixed) //固定物の場合
		{
			mtrxInverseInertiaTensorT.Resize(3, 3);
			mtrxInverseInertiaTensorT.InitZero(); //全ての成分が0
		} else {
			//★要改良！　G14ページを読むと、最後の慣性テンソルの逆行列の計算を省けることが書いてある。先に基本姿勢の慣性テンソルの逆行列を計算しておく。

			//現在の姿勢から回転行列とその転置行列を求める
			ho::Matrix mtrxRotation; //回転行列
			qtAngle.CalcMatrix(&mtrxRotation); //現在の姿勢から3X3の回転行列を取得
			ho::Matrix mtrxTransposedRotation = mtrxRotation.CalcTransposedMatrix(); //回転行列の転置行列
			//ho::Matrix mtrxInverseRotation = mtrxRotation.CalcInverseMatrix(); //回転行列の逆行列（回転行列においては、逆行列=転置行列となるので、↑の行の計算の速い転置行列を使う）

			//基本姿勢の慣性テンソルから、現在の姿勢の慣性テンソルを求める
			ho::Matrix mtrxInertiaTensorNow = mtrxRotation * mtrxInertiaTensor * mtrxTransposedRotation;

			ho::Matrix mtrxInverseInertiaTensorNow; //現在姿勢の慣性テンソルの逆行列
			mtrxInverseInertiaTensorT = mtrxInertiaTensorNow.CalcInverseMatrix(); //逆行列を計算
		}*/
	
		return;
	}

	//ワールド座標上の一点での剛体間の相対速度を計算
	vec3d Body::GetRelativeVelocity(const std::weak_ptr<Body> &wpBodyObj1, const std::weak_ptr<Body> &wpBodyObj2, const vec3d &vecWorldPos)
	{
		const std::shared_ptr<Body> spBodyObj1 = wpBodyObj1.lock();
		const std::shared_ptr<Body> spBodyObj2 = wpBodyObj2.lock();

		//ある回転軸の角速度と重心からのベクトルの外積でその回転軸に対する角速度が出る

		vec3d vecLocalPos[2]; //ワールド座標上の一点をローカル座標に変換した座標
		vecLocalPos[0] = vecWorldPos - spBodyObj1->getvecPos();
		vecLocalPos[1] = vecWorldPos - spBodyObj2->getvecPos();
		vec3d vecOmega[2]; //角速度
		vecOmega[0] = spBodyObj1->getvecOmega();
		vecOmega[1] = spBodyObj2->getvecOmega();

		vec3d vecV[2]; //速度ベクトル
		vecV[0] = spBodyObj1->getvecLinearVelocity() + vec3d::GetCross(vecOmega[0], vecLocalPos[0]);//
		vecV[1] = spBodyObj2->getvecLinearVelocity() + vec3d::GetCross(vecOmega[1], vecLocalPos[1]);//

		/*
		//ローカル座標と角速度を右手系座標に変換
		for (int i = 0; i < 2; i++)
		{
			vecLocalPos[i] = vecLocalPos[i].GetExchangeLeftRight();
			vecOmega[i] = vecOmega[i].GetExchangeLeftRight();
		}

		vec3d vecV[2]; //速度ベクトル
		vecV[0] = pBodyObj1->GetvecParallelV() + vec3d::GetCrossProduct(vecLocalPos[0], vecOmega[0]).GetExchangeLeftRight();
		vecV[1] = pBodyObj2->GetvecParallelV() + vec3d::GetCrossProduct(vecLocalPos[1], vecOmega[1]).GetExchangeLeftRight();
		*/

		return vecV[0] - vecV[1];
	}

	//重力を付加する
	void Body::ApplyGravity(const vec3d &vecGravity)
	{
		if (Fixed)
		{
			this->vecLinearVelocity = vec3d(0, 0, 0);
			this->vecL = vec3d(0, 0, 0);
		} else {
			this->vecLinearVelocity += vecGravity;
		}

		return;
	}

	//力積を加える
	void Body::ApplyForce(const vec3d &vecForce, const vec3d &vecLocalPos) 
	{
		if (Fixed)
			return;

		this->vecLinearVelocity += vecForce * this->MassInv; //並進加速度は位置に関係なく加算される
		this->vecL += vec3d::GetCross(vecLocalPos, vecForce); //角加速度は相対位置と力のベクトルとの外積で求まる
		this->vecOmega = this->vecL * this->mtrxInertiaNowInv;//角運動量ベクトルと慣性テンソルの逆行列を積算して、角速度ωを求める

		return;
	}

	//位置を動かす力を加える
	void Body::ApplyCorrect(const vec3d &vecForce, const vec3d &vecLocalPos) 
	{
		if (Fixed)
			return;

		this->vecPos += vecForce * this->MassInv; //並進加速度は位置に関係なく加算される

		vec3d vecLocalL = vec3d::GetCross(vecLocalPos, vecForce); //姿勢変化に影響する角運動量は相対位置と力のベクトルとの外積で求まる
		vec3d vecOmega2 = vecLocalL * this->mtrxInertiaNowInv;//変化する角運動量ベクトルと慣性テンソルの逆行列を積算して、変化する角度ωを求める

		/* //これは行列で同じ回転をする場合
		ho::Matrix mtrx, mtrxOmega(&vecOmega2.Normalize(), vecOmega2.GetPower());
		qtAngle.CalcMatrix(&mtrx);
		mtrx = mtrxOmega * mtrx;
		qtAngle = ho::Quaternion(&mtrx);
		*/

		qtAngle += 0.5 * Quaternion(0, vecOmega2.x, vecOmega2.y, vecOmega2.z) * qtAngle; //角速度ωに応じて姿勢を変化させる
		qtAngle.Normalize(); //クォータニオンを正規化

		CalcmtrxInertiaNowInv(); //姿勢（クォータニオン）が変化したので、現在時刻（現在姿勢）での慣性テンソルの逆行列を計算しておく

		return;
	}


	//ある一辺が含まれる面をすべて列挙する（辺の頂点インデックス配列のインデックスの辺にが含まれる面の頂点インデックス配列中のインデックスを列挙）
	void Body::GetlistVtFaceiiFromVtEdgeii(std::list<int> *plistVtFaceii, int VtEdgeii)
	{
		plistVtFaceii->clear();

		for (unsigned int i = 0; i < vectorVtFaceIndex.size(); i+= 3) //すべての面を走査
		{
			//辺が含まれていた場合
			if ((vectorVtFaceIndex.at(i) == vectorVtEdgeIndex.at(VtEdgeii) && vectorVtFaceIndex.at(i + 1) == vectorVtEdgeIndex.at(VtEdgeii + 1)) ||
				(vectorVtFaceIndex.at(i) == vectorVtEdgeIndex.at(VtEdgeii + 1) && vectorVtFaceIndex.at(i + 1) == vectorVtEdgeIndex.at(VtEdgeii)) ||
				(vectorVtFaceIndex.at(i + 1) == vectorVtEdgeIndex.at(VtEdgeii) && vectorVtFaceIndex.at(i + 2) == vectorVtEdgeIndex.at(VtEdgeii + 1)) ||
				(vectorVtFaceIndex.at(i + 1) == vectorVtEdgeIndex.at(VtEdgeii + 1) && vectorVtFaceIndex.at(i + 2) == vectorVtEdgeIndex.at(VtEdgeii)) ||
				(vectorVtFaceIndex.at(i + 2) == vectorVtEdgeIndex.at(VtEdgeii) && vectorVtFaceIndex.at(i) == vectorVtEdgeIndex.at(VtEdgeii + 1)) ||
				(vectorVtFaceIndex.at(i + 2) == vectorVtEdgeIndex.at(VtEdgeii + 1) && vectorVtFaceIndex.at(i) == vectorVtEdgeIndex.at(VtEdgeii)))
			{
				plistVtFaceii->push_back(i);
			}
		}

		return;
	}

	/*
	//すべての面の現在の姿勢での法線ベクトルを取得する
	void Body::GetvectorvecFaceNRotation(std::vector<vec3d> *pvectorvecFaceN) 
	{
		*pvectorvecFaceN = vectorvecFaceN; //新しい配列にコピー

		for (unsigned int i = 0; i < pvectorvecFaceN->size(); i++)
			qtAngle.TransformVector(&pvectorvecFaceN->at(i)); //法線ベクトルを現在の姿勢に回転

		return;
	}
	*/

	//メッシュのワールド座標を更新する
	void Body::UpdateMeshVertexWorldPos()
	{
		//if (this->LastUpdateFrame < this->wpWorldObj.lock()->getFrame()) //現在のフレームではまだ更新されていない場合
			this->spMeshObj->UpdateVertexWorldPos(this->qtAngle, this->vecPos); //メッシュの座標を更新

		return;
	}

	//接触オブジェクトを追加
	void Body::AddpContactObj(Contact *pContactObj)
	{
		/*
		listpContactObj.push_back(pContactObj);
		*/

		return;
	}

	//接触オブジェクトを削除
	bool Body::ErasepContactObj(Contact *pContactObj)
	{
		/*
		for (std::list<Contact *>::iterator itr = listpContactObj.begin(); itr != listpContactObj.end(); itr++)
		{
			if (*itr == pContactObj)
			{
				listpContactObj.erase(itr);
				return TRUE; //削除できた
			}
		}

		return FALSE; //削除対象のオブジェクトが見つからなかった
		*/

		return true;
	}
}