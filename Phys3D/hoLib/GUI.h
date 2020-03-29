//GUIクラス

#pragma once
#include <d3d9.h>
#include "CharTexture.h"
#include "DrawString.h"
#include "Math.h"
#include "Rect.h"
#include <map>
#include "UserInput.h"
#include <vector>
#include <list>
#include "Common.h"
#include "Direction.h"
#include <queue>
#include "ComPtr.hpp"

namespace ho
{
	namespace GUI
	{
		//前方宣言
		class GUI;
		class Element;
		class ScrollBar;

		//メッセージ
		enum eMessage
		{
			DRAW,					//描画
			GET_PTR,				//エレメントオブジェクトのポインタを取得（4Byte、void **ポインタを格納するダブルポインタ）
			MOUSE_LCLICK,			//マウスの左クリック確定（8Byte、ho::PT<int>マウス座標）
			MOUSE_LDRAG_BEGIN,		//マウスの左ボタンでドラッグ開始（8Byte、ho::PT<int>クリック時の座標）
			MOUSE_LDRAG_MOVE,		//マウスの左ボタンでドラッグ移動（16Byte、ho::PT<int>クリック時の座標、ho::PT<int>現在のマウス座標）
			MOUSE_LDRAG_END,		//マウスの左ボタンでドラッグ終了（16Byte、ho::PT<int>クリック時の座標、ho::PT<int>現在のマウス座標）
			MOUSE_HOVER_BEGIN,		//マウスホバー開始
			MOUSE_HOVER_MOVE,		//マウスホバー中に移動（8Byte、ho::PT<int> *移動中のマウス座標）
			MOUSE_HOVER_END,		//マウスホバー終了
			GET_TOPID_FROMPOS,		//任意の座標上の一番上側のエレメントのIDを取得（12Byte、ho::PT<int>任意座標、int *IDを格納するポインタ）
			SETPOS_CENTER,			//クライアント領域の中央に移動
			SHOW,					//表示、非表示の切り替え（4Byte、BOOL）
			SHOW_SWITCH,			//表示⇔非表示 を交互に気鋭変える
			GET_SHOW,				//表示、非表示の状態を取得する（4Byte、 *BOOL 結果を得るポインタ）
			SETTEXT,				//ウィンドウやラベル等の文字列を設定（4Byte、TCHAR *）
			POSITION,				//エレメントの位置情報（rcPosition）を変更する（16Byte、RectWH *新しい位置情報）
			MOVE,					//エレメントの左上座標を変更する（8Byte、ho::PT<int> *新しい座標）
			SETFOREGROUND,			//親エレメントのリスト上で一番最後（画面上では上側）に並び替える
			GET_CLIENT_RECT,		//エレメントのクライアント領域の絶対位置を取得（4Byte、RectWH *クライアント領域を書き込むポインタ）
			CB_PUSH,				//コンボボックスの最後に要素を追加（4Byte、TCHAR *）
			CB_GETSELECT,			//コンボボックスの選択された要素のインデックスを得る（4Byte, 結果を得るint型へのポインタ）
			CB_SETSELECT,			//コンボボックスの要素を選択する（4Byte、int 選択する要素数）
			SB_SETMAX,				//スクロールバーの最大値を設定（4Byte、int）
			SB_SETPOS,				//スクロールバーの位置を設定（4Byte, int）
			SB_SETRANGE,			//スクロールバーの表示領域を設定（4Byte, int）
			SB_GETPOS,				//スクロールバーの現在位置を取得(4Byte、*int）
			CHECKBOX_SETCHECK,		//チェックボックスのチェックを変更（4Byte、BOOL）
			GAUGESLIDER_SETPOS,		//ゲージスライダーの値を変更（8Byte、double *新しい値）
			IMG_SETTEXTURE,			//イメージにテクスチャを設定（4Byte、ComPtr<LPDIRECT3DTEXTURE9>）
			IMG_RELEASETEXTURE,		//テクスチャのComポインタを解放する
			EL_DELETE,				//エレメントを削除する
		};

		//ウィンドウのスタイル
		namespace eWindowStyle
		{
			enum e
			{
				TITLE = 0x00000001,			//タイトルバーを使用する
				RESIZE = 0x00000002,		//リサイズを許可する
				CLOSEBUTTON = 0x00000004,	//閉じるボタンを使用する
				MINIBUTTON = 0x00000008,	//最小化ボタンを使用する
				MAXBUTTON = 0x00000010		//最大化ボタンを使用する
			};
		}


		//メッセージクラス
		struct MESSAGE 
		{
		public:
			//コンストラクタ
			MESSAGE() {};
			MESSAGE(GUI *pGUIObj, eMessage Message, int TargetID, int Bytes, LPVOID pParam) { this->pGUIObj = pGUIObj; this->Message = Message; this->TargetID = TargetID; this->Bytes = Bytes; this->pParam = pParam; }

			GUI *pGUIObj; //GUIオブジェクトへのポインタ
			eMessage Message; //メッセージの種類
			int TargetID; //宛先となるID（再帰処理で全体に送る場合は-1）
			int Bytes; //付随情報のバイト数
			LPVOID pParam; //付随情報へのポインタ
		};


		enum eHV {Horizonal, Vertical}; //水平方向、垂直方向

		class GUI
		{
		public:
			struct MOUSEDRAG //マウスドラッグ情報構造体
			{
				int DragElementID; //クリックされたエレメントのID
				ho::PT<int> ptClickPos; //クリックされた位置
				BOOL Enable; //ドラッグ中かどうか
			};
			struct MOUSEHOVER //マウスホバー情報構造体
			{
				int HoverElementID; //ホバー中のエレメントのID
				BOOL Enable; //ホバー中かどうか
			};
		public:
			GUI(LPDIRECT3DDEVICE9 pD3DDevice, DrawString *pDrawStringObj, RectWH pRectWH, LONG FontHeight, UserInput *pUserInputObj); //コンストラクタ
			~GUI(); //デストラクタ

			void SendMsg(MESSAGE Message); //メッセージをすぐに送る
			void PostMsg(MESSAGE Message); //メッセージをあとで送る（現在処理中のメッセージが終わったあと）
			void DefaultMessageProcedure(MESSAGE Message); //デフォルトメッセージプロシージャ
			void FrameProcess(); //1フレーム毎の処理
			void Draw(); //描画
			Element *GetpElementObj(int ID); //IDからエレメントを取得

			BOOL CreateWnd(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption, BOOL Show, eWindowStyle::e WindowStyle); //ウィンドウ作成
			BOOL CreateButton(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption); //ボタン作成
			BOOL CreateLabel(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pStr); //ラベル作成
			BOOL CreateComboBox(ho::RectWH rcPosition, int ID, int ParentID, int z); //コンボボックス作成
			BOOL CreateCheckBox(ho::RectWH rcPosition, int ID, int ParentID, int z, BOOL Check, TCHAR *pStr); //チェックボックス作成
			BOOL CreateGaugeSlider(ho::RectWH rcPosition, int ID, int ParentID, int z, eHV hv, double Max, double Pos); //ゲージスライダー作成
			BOOL CreateImage(ho::RectWH rcPosition, int ID, int ParentID, int z, ComPtr<LPDIRECT3DTEXTURE9> cpTexture); //イメージ作成

			int GetNullSystemElementID(); //登録可能なシステムエレメントIDを取得
			int RegistSystemElementObj(Element *pElementObj); //作成したシステムエレメントを登録し、IDが返る
			void DeleteSystemElementObj(int ID); //指定されたIDのシステムエレメントを管理リストから削除する

			void CalcTriangleVertex(ho::D3DVERTEX_2D *pVertex, ho::PT<int> ptCenter, int Radius, ho::Direction8 Angle); //三角形の3頂点の座標を計算する
			void DrawBorder(RectWH *pRectWH, DWORD Color); //矩形の枠を描画する

			//アクセッサ
			LPDIRECT3DDEVICE9 GetpD3DDevice() { return pD3DDevice; }
			D3DVERTEX_2D *Getpvt() { return vt; }
			void SetpMessageProcedure(bool (*pMessageProcedure)(void *pObj, MESSAGE Message)) { this->pMessageProcedure = pMessageProcedure; }
			void SetpMessageProcesureObj(void *pObj) { this->pMessageProcedureObj = pObj; }
			DrawString *GetpDrawStringObj() { return pDrawStringObj; }
			LONG GetFontHeight() { return FontHeight; }
			DWORD GetTime() { return Time; }
			void SetEnableUserInput(bool b) { this->EnableUserInput = b; }
		private:
			LPDIRECT3DDEVICE9 pD3DDevice;
			CharTexture *pCharTextureObj; //文字テクスチャオブジェクトへのポインタ
			DrawString *pDrawStringObj; //文字描画オブジェクトへのポインタ
			LONG FontHeight; //フォントの高さ
			UserInput *pUserInputObj; //ユーザー入力オブジェクトへのポインタ
			Element *pTopElementObj; //一番上の階層にあるエレメントへのポインタ
			D3DVERTEX_2D vt[4];
			std::vector<Element *> vectorSystemElementID; //-1以降の負の方向のID（システム専用）を管理する配列
			MOUSEDRAG MouseDrag; //マウスのドラッグ状態
			MOUSEHOVER MouseHover; //マウスホバーの状態
			DWORD Time; //FrameProcess メソッドが実行された回数
			std::queue<MESSAGE> queueMessage; //SndMsg関数で送られたメッセージ
			bool EnableUserInput; //ユーザー入力を受け付けるかどうか

			bool (*pMessageProcedure)(void *pObj, MESSAGE Message); //メッセージプロシージャ関数へのポインタ
			void *pMessageProcedureObj; //メッセージプロシージャ関数を持つオブジェクトへのポインタ
			void MouseInput(); //マウスなどの入力を処理
			void ProcessMessageQue(); //SnedMsg関数で溜まったメッセージを処理
			void DeleteAllElement(); //全てのエレメントを削除する
		};

		class Element
		{
		public:
			Element(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, Element *pParentElementObj = NULL); //コンストラクタ
			virtual ~Element(); //デストラクタ
			void MessageRouting(MESSAGE Message); //メッセージの経路選択
			virtual BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //メッセージ処理プロシージャ
			BOOL DefaultMessageProcedure(MESSAGE Message, BOOL SendChild); //デフォルトメッセージ処理プロシージャ
			virtual void Draw() {} //描画
			Element *Element::GetpElementObj(int ID); //IDから再帰処理で階層構造の中から該当するエレメントを探す
			void AddChildElementObj(Element *pElementObj); //子供エレメントを追加
			RectWH GetAbsolutePos(); //絶対位置を取得
			RectWH GetAbsoluteClientPos(); //クライアント領域のみの絶対位置を取得
			void SetForegroundChildID(int ID); //任意のIDの子供オブジェクトを最前面に並び替える

			static bool Less(const Element *p1, const Element *p2) { return *p1 < *p2; }

			//演算子
			friend bool operator < (const Element &obj1, const Element &obj2) { return obj1.z < obj2.z; } //STLのソート用

			//アクセッサ
			int GetID() { return ID; }
			RectWH GetrcPosition() { return rcPosition; }
			BOOL GetbShow() { return bShow; }
		protected:
			GUI *pGUIObj;
			RectWH rcPosition; //位置（親エレメントからの相対位置）
			ho::PT<int> MinSize; //rcPositionの最小サイズ
			RECT rcClientOffset; //rcPosition からクライアント領域までの間隔（枠やタイトルバーなど）
			int ID; //エレメント固有のID
			int ParentID; //親となるエレメントのID
			std::list<Element *> listpChildElementObj; //子供のエレメントオブジェクトへのポインタリスト
			Element *pParentElementObj; //親となるエレメントオブジェクトへのポインタ
			int z; //大きい値のものほど手前に描画される
			BOOL bShow; //表示するかどうか
			BOOL Hover; //ホバー状態かどうか

			void GetTopIDFromPos(MESSAGE Message); //GET_TOPID_FROMPOS（任意の座標上の一番上側のエレメントのIDを取得）
			RectWH GetAbsolutePosAlg(RectWH Rect, int Level); //絶対位置を取得（内部再帰関数）
			RectWH GetAbsoluteClientPosAlg(RectWH Rect, int Level); //クライアント領域のみの絶対位置を取得（内部再帰関数） 
			void SetPos_Center(MESSAGE Message); //クライアント領域の中央に移動
			void Show(MESSAGE Message); //表示、非表示の切り替え（4Byte、BOOL）
			virtual void SetText(MESSAGE Message) {}
			ho::PT<int> GetLocalPtFromAbsolutePt(ho::PT<int> ptPos); //絶対座標からエレメント内の相対座標に変換
		};

		class Window : public Element
		{
		public:
			Window(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption, BOOL Show, eWindowStyle::e WindowStyle); //コンストラクタ
			~Window(); //デストラクタ
			void Draw(); //描画
			void SetText(MESSAGE Message); //文字列を設定
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //メッセージプロシージャ
		private:
			enum eRegion //ウィンドウ内の領域を表す列挙体
			{
				LeftTop, Top, RightTop, Left, Right, LeftBottom, Bottom, RightBottom, TitleBar, Client
			};
			tstring strCaption; //タイトルバー文字列
			eWindowStyle::e WindowStyle; //ウィンドウのスタイル
			RectWH rcDragBeginPos; //rcPosition のドラッグ開始時の位置（親エレメントからの相対位置）
			eRegion DragBeginRegion; //ドラッグ開始時のマウス座標の領域
			int BorderWidth; //外側の枠の幅

			void Mouse_LDrag_Move(MESSAGE Message); //マウス左ボタンでドラッグ移動時の処理
			eRegion GetRegion(ho::PT<int> *pptLocalPos); //エレメント内の座標から領域を取得
			void MouseHover(ho::PT<int> *ptMousePos); //マウスホバー中のカーソルアイコン変更処理
		};

		class Button : public Element
		{
		public:
			Button(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption); //コンストラクタ
			~Button(); //デストラクタ
			void Draw(); //描画
		private:
			tstring strCaption; //タイトルバー文字列
			BOOL Enable; //使用可能かどうか
		};

		class Label : public Element
		{
		public:
			Label(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pStr); //コンストラクタ
			~Label(); //デストラクタ
			void Draw(); //描画
			void SetText(MESSAGE Message); //文字列を設定
		private:
			tstring strLabel; //ラベル文字列
		};

		class ComboBox : public Element
		{
		public:
			ComboBox(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z); //コンストラクタ
			~ComboBox(); //デストラクタ
			void Draw(); //描画
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //メッセージプロシージャ
		private:
			std::list<tstring> listStr; //各リストの文字列
			int SelectIndex; //選択中のリストインデックス
			RectWH rcOpenPosition; //リストを開いたときのサイズ
			BOOL ListOpen; //リストが開かれているかどうか
			int ScrollBarSize; //スクロールバーのサイズ
			ScrollBar *pScrollBarObj; //縦方向のスクロールバーエレメントへのポインタ
			int HoverPos; //マウスホバー中のリスト位置（-2=ホバーしていない、-1=上部ボックスでホバー、0以上=リスト中でホバー）

			void MouseClick(MESSAGE Message); //マウスクリック時の処理
			void CB_Push(MESSAGE Message); //コンボボックスの最後に要素を追加（4Byte、TCHAR *）
			void AdjustScrollBarSize(); //リストの長さに応じてスクロールバーのサイズを調整
			int GetListViewLines(); //開かれた状態のリスト部分に表示可能な行数を取得
			int GetListIndexFromMouseLocalPt(ho::PT<int> ptMouseLocalPos); //マウスのローカル座標から選択されるリストのインデックスを取得
			void MouseHoverMove(MESSAGE Message); //マウスホバー中に移動
		};

		class ScrollBar : public Element
		{
		public:
			enum eHV {Horizonal, Vertical}; //水平方向、垂直方向
		public:
			ScrollBar(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, Element *pParentElementObj, eHV hv, int Max, int Pos, int Range); //コンストラクタ
			~ScrollBar(); //デストラクタ
			void Draw(); //描画
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //メッセージプロシージャ
		private:
			eHV hv; //スクロールバーの方向
			int Max; //最大値（最小値は0）
			int Pos; //現在のスクロール位置
			int Range; //つまみの幅
			int Size; //見た目の幅
			int DragMoveDistance; //ドラッグ中にツマミが動かされた距離（ピクセル単位）
			int DragBeginPos; //ドラッグ開始時の選択位置

			void MouseClick(MESSAGE Message); //マウスクリック時の処理
			void Clamp(); //設定値がはみ出さないようにする
			void GetVerticalSliderDimension(int &SliderHeight, int &SliderBeginY, double &HeightPerPos, int &Y1, int &Y2, RectWH rcAbsolutePos); //縦方向スクロールバーのスライダー部分の寸法を取得
			void MouseDragMove(MESSAGE Message); //マウスドラッグ移動時の処理
		};

		class CheckBox : public Element
		{
		public:
			CheckBox(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, BOOL Check, TCHAR *pStr); //コンストラクタ
			~CheckBox(); //デストラクタ
			void Draw(); //描画
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //メッセージプロシージャ
		private:
			BOOL Check;
			tstring strText; //チェックボックス横のテキスト
		};

		class GaugeSlider : public Element
		{
		public:
			GaugeSlider(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, eHV hv, double Max, double Pos); //コンストラクタ
			~GaugeSlider(); //デストラクタ
			void Draw(); //描画
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //メッセージプロシージャ
		private:
			eHV hv; //縦横
			double Max; //最大値
			double Pos; //現在の値

			BOOL GetPosFromMousePos(double *pPos, ho::PT<int> *pptMousePos); //マウス座標よりゲージ内の値を取得
		};

		class Image : public Element
		{
		public:
			Image(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, ComPtr<LPDIRECT3DTEXTURE9 > cpTexture); //コンストラクタ
			~Image(); //デストラクタ
			void Draw(); //描画
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //メッセージプロシージャ
		private:
			ComPtr<LPDIRECT3DTEXTURE9> cpTexture;
		};
	}
}
