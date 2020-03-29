//ノート（音程）や調性のクラス

#pragma once
#include <Windows.h>

namespace ho {

//ノート
class NoteNumber
{
	public:
		NoteNumber(int Num); //コンストラクタ（ノートナンバーで指定）
		NoteNumber(int Octave, int Scale); //コンストラクタ（オクターブと音階（0～11）で指定）
		~NoteNumber() {} //デストラクタ

		double GetPitch(); //周波数を得る

		//アクセッサ
		int GetNum() { return Num; }
		void SetNum(int Num) { this->Num = Num; }
	private:
		int Num; //0～127までのノートナンバー
		static const double APitch; //基準音Aの周波数（Hz）
	};

	/*MIDI規格と同じ0～127までのノートナンバーを表す。*/

	//調性
	class Tonal
	{
	public:
		Tonal(int Key); //コンストラクタ
		BOOL Check(int NoteNum); //ノートナンバーが調性の構成音かどうかを判定
		void ChangeRelative(int Value); //相対転調
		void ChangeAbsolute(int Value); //絶対転調
		int GetNoteAlongScale(int NoteNum, int Degree); //ノートナンバーを音階に沿って上昇・下降させる
		const TCHAR *GetpName(); //現在の調性名を取得
	private:
		int Key; //調性（0=C dur, 1=Cis dur, 2=D dur…11=H Dur）
		static const BOOL KeyTable[12]; //調性の構成音判定用テーブル
		static const TCHAR Name[12][8]; //調性名
	};

	/*オブジェクト毎に一つの調性を持ち、ノートナンバーで調性の構成音を判定したりする。*/

}