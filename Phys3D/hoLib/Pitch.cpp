#include "Pitch.h"
#include <math.h>
#include "Math.h"

namespace ho {

	const double NoteNumber::APitch = 442.0; //基準音Aの周波数（Hz）

	//コンストラクタ（ノートナンバーで指定）
	NoteNumber::NoteNumber(int Num) 
	{
		this->Num = Num;
	}

	//コンストラクタ（オクターブと音階（0～11）で指定）
	NoteNumber::NoteNumber(int Octave, int Scale) 
	{
		this->Num = (Octave + 2) * 12 + Scale;
	}

	//周波数を得る
	double NoteNumber::GetPitch() 
	{
		return APitch * pow(2.0, (Num - 69.0) / 12.0);
	}







	const BOOL Tonal::KeyTable[12] = {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1};
	const TCHAR Tonal::Name[12][8] = { TEXT("C dur"), TEXT("Des dur"), TEXT("D dur"), TEXT("Es dur"), TEXT("E dur"), TEXT("F dur"), TEXT("Fis dur"), TEXT("G dur"), TEXT("As dur"), TEXT("A dur"), TEXT("B dur"), TEXT("H dur") };

	//コンストラクタ
	Tonal::Tonal(int Key)
	{
		this->Key = Key % 12;
	}

	BOOL Tonal::Check(int NoteNum) //ノートナンバーが調性の構成音かどうかを判定
	{
		return KeyTable[(NoteNum - Key) % 12];
	}

	void Tonal::ChangeRelative(int Value) //相対転調
	{
		Key = (Key + Value) % 12;

		return;
	}

	void Tonal::ChangeAbsolute(int Value) //絶対転調
	{
		Key = Value % 12;

		return;
	}

	//ノートナンバーを音階に沿って上昇・下降させる
	int Tonal::GetNoteAlongScale(int NoteNum, int Degree) 
	{
		if (Degree >= -1 && Degree <= 1) //-1度、0度、1度の場合
			return Degree; //無効とする（変化しない）

		int Sign = ho::GetSign(Degree); //上昇か下降かを符号で得る

		for (int i = 0; i < abs(Degree) - 1; i++)
		{
			int j = 1;
			while (!Check(NoteNum + j * Sign)) //ノートナンバー+j が調性の構成音ではない場合
				j++;
			NoteNum += j * Sign;
		}

		return NoteNum;
	}

	//現在の調性名を取得
	const TCHAR *Tonal::GetpName() 
	{
		return Name[Key];
	}

}