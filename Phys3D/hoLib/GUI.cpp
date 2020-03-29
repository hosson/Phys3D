#include "GUI.h"
#include "hoLib.h"
#include "Debug.h"
#include <algorithm>

namespace ho
{
	namespace GUI
	{
		//コンストラクタ
		GUI::GUI(LPDIRECT3DDEVICE9 pD3DDevice, DrawString *pDrawStringObj, RectWH RectWH, LONG FontHeight, UserInput *pUserInputObj)
		{
			this->pD3DDevice = pD3DDevice;
			this->pDrawStringObj = pDrawStringObj;
			this->pMessageProcedure = NULL;
			this->FontHeight = FontHeight;
			this->pUserInputObj = pUserInputObj;
			this->MouseDrag.Enable = FALSE;
			this->MouseHover.Enable = FALSE;
			this->Time = 0;
			this->EnableUserInput = TRUE;

			pTopElementObj = NULL;
			pTopElementObj = new Element(this, RectWH, 0, -1, 0); //一番上の階層にあるエレメントを作成
		}

		//デストラクタ
		GUI::~GUI() 
		{
			SDELETE(pTopElementObj); //一番上の階層にあるエレメントを削除
		}

		//全てのエレメントを削除する
		void GUI::DeleteAllElement() 
		{
			SendMsg(ho::GUI::MESSAGE(this, ho::GUI::EL_DELETE, -1, 0, NULL));

			return;
		}

		//メッセージをすぐに送る
		void GUI::SendMsg(MESSAGE Message) 
		{
			if (pMessageProcedure) //メッセージプロシージャが設定されている場合
				if ((pMessageProcedure)(pMessageProcedureObj, Message)) //メッセージプロシージャでメッセージが処理された場合
					return;

			DefaultMessageProcedure(Message);

			return;
		}

		//メッセージをあとで送る（現在処理中のメッセージが終わったあと）
		void GUI::PostMsg(MESSAGE Message) 
		{
			queueMessage.push(Message);

			return;
		}

		//デフォルトメッセージプロシージャ
		void GUI::DefaultMessageProcedure(MESSAGE Message) 
		{
			if (pTopElementObj)
				pTopElementObj->MessageRouting(Message);

			return;
		}

		//1フレーム毎の処理
		void GUI::FrameProcess() 
		{
			MouseInput(); //マウスなどの入力を処理
			ProcessMessageQue(); //SendMsg関数で溜まったメッセージを処理

			Time++;

			return;
		}

		//マウスなどの入力を処理
		void GUI::MouseInput() 
		{
			if (!EnableUserInput) //ユーザー入力を受け付けない場合
				return;

			//マウス座標とクリック状態を取得
			int FrameFlag = pUserInputObj->GetFrameFlag();
			UserInput::MOUSESTATE *pMS = pUserInputObj->GetpMouseState();

			//現在のマウスカーソル位置に存在する一番上側のエレメントのIDを得る
			int PosID; //GET_TOPID_FROMPOS で得られたID
			{
				std::vector<BYTE> MsgBuf(sizeof(ho::PT<int>) + sizeof(int)); //メッセージ用のバッファ
				*(ho::PT<int> *)&MsgBuf.at(0) = pMS[FrameFlag].ptPos; //バッファにクリック座標をコピー
				*(int *)&MsgBuf.at(sizeof(ho::PT<int>)) = -1; //IDは-1としておく
				SendMsg(MESSAGE(this, GET_TOPID_FROMPOS, 0, MsgBuf.size(), &MsgBuf.at(0))); //マウスクリック位置にあるエレメントのIDを取得するメッセージを送信
				PosID = *(int *)&MsgBuf.at(sizeof(ho::PT<int>)); //GET_TOPID_FROMPOS で得られたID
			}

			if ((pMS[FrameFlag].LeftButton) && !(pMS[1 - FrameFlag].LeftButton)) //左ボタンが押された瞬間の場合
			{
				if (PosID != -1) //クリック位置に何らかのエレメントが存在した場合
				{
					SendMsg(MESSAGE(this, MOUSE_LCLICK, PosID, sizeof(ho::PT<int>), &pMS[FrameFlag].ptPos)); //左クリックメッセージを送信

					MouseDrag.ptClickPos = pMS[FrameFlag].ptPos; //ドラッグ状態のクリック位置を保存
					MouseDrag.DragElementID = PosID; //クリック確定IDを保存
					MouseDrag.Enable = TRUE;
					SendMsg(MESSAGE(this, MOUSE_LDRAG_BEGIN, MouseDrag.DragElementID, sizeof(ho::PT<int>), &MouseDrag.ptClickPos)); //ドラッグ開始メッセージを送信
				}
			} else if ((pMS[FrameFlag].LeftButton) && (pMS[1 - FrameFlag].LeftButton)) //左ボタンが押され続けている場合
			{
				if (pMS[FrameFlag].ptPos != pMS[1 - FrameFlag].ptPos) //マウスカーソルが移動した場合
				{
					if (MouseDrag.Enable) //ドラッグ中の場合
					{
						std::vector<BYTE> MsgBuf(sizeof(ho::PT<int>) * 2); //メッセージ用のバッファ
						*(ho::PT<int> *)&MsgBuf.at(0) = MouseDrag.ptClickPos;
						*(ho::PT<int> *)&MsgBuf.at(sizeof(ho::PT<int>)) = pMS[FrameFlag].ptPos;
						SendMsg(MESSAGE(this, MOUSE_LDRAG_MOVE, MouseDrag.DragElementID, MsgBuf.size(), &MsgBuf.at(0))); //ドラッグ移動のメッセージを送信
					}
				}
			} else if (!(pMS[FrameFlag].LeftButton) && (pMS[1 - FrameFlag].LeftButton)) //左ボタンが離された瞬間の場合
			{
				if (MouseDrag.Enable) //ドラッグ中の場合
				{
					std::vector<BYTE> MsgBuf(sizeof(ho::PT<int>) * 2); //メッセージ用のバッファ
					*(ho::PT<int> *)&MsgBuf.at(0) = MouseDrag.ptClickPos;
					*(ho::PT<int> *)&MsgBuf.at(sizeof(ho::PT<int>)) = pMS[FrameFlag].ptPos;
					SendMsg(MESSAGE(this, MOUSE_LDRAG_END, MouseDrag.DragElementID, MsgBuf.size(), &MsgBuf.at(0))); //ドラッグ終了のメッセージを送信
					MouseDrag.Enable = FALSE; //ドラッグ終了
				}
			}

			//ホバーに関する処理
			if (PosID != -1) //マウスポインタが何らかのエレメント上にある場合
			{
				if (MouseHover.Enable) //ホバー中の場合
				{
					if (MouseHover.HoverElementID != PosID) //ホバー対象が別のエレメント上に移っていた場合
					{
						SendMsg(MESSAGE(this, MOUSE_HOVER_END, MouseHover.HoverElementID, 0, NULL)); //ホバー終了メッセージを送信
					}
				} else { //ホバーしていない場合
					MouseHover.Enable = TRUE;
				}
				SendMsg(MESSAGE(this, MOUSE_HOVER_BEGIN, PosID, 0, NULL)); //ホバー開始メッセージを送信
				SendMsg(MESSAGE(this, MOUSE_HOVER_MOVE, MouseHover.HoverElementID, 8, &pMS[FrameFlag].ptPos)); //ホバー開始位置を知らせるためにメッセージを送信
				MouseHover.HoverElementID = PosID;
			} else { //マウスポインタがエレメント上にない場合
				MouseHover.Enable = FALSE;
			}
			if ((MouseHover.Enable) && (pMS[FrameFlag].ptPos != pMS[1 - FrameFlag].ptPos)) //ホバー中にマウスカーソルが移動していた場合
				SendMsg(MESSAGE(this, MOUSE_HOVER_MOVE, MouseHover.HoverElementID, 8, &pMS[FrameFlag].ptPos)); //マウスホバー中に移動したメッセージを送信

			return;
		}

		//SnedMsg関数で溜まったメッセージを処理
		void GUI::ProcessMessageQue() 
		{
			while (!queueMessage.empty()) //キューの中身が空になるまで繰り返す
			{
				SendMsg(queueMessage.front()); //先頭のメッセージを処理
				queueMessage.pop(); //先頭のメッセージを削除
			}

			return;
		}

		//描画
		void GUI::Draw() 
		{
			DWORD OldFVF;
			pD3DDevice->GetFVF(&OldFVF);
			pD3DDevice->SetFVF(D3DVERTEX_2D::FVF);

			SendMsg(MESSAGE(this, DRAW, -1, 0, NULL)); //描画の再帰メッセージを送る

			pD3DDevice->SetFVF(OldFVF);

			return;
		}

		//ウィンドウ作成
		BOOL GUI::CreateWnd(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption, BOOL Show, eWindowStyle::e WindowStyle)
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //親となるエレメントを得る
			if (!pParentElementObj) //親が見つからなかった場合
				return FALSE;

			Element *pElementObj = new Window(this, rcPosition, ID, ParentID, z, pCaption, Show, WindowStyle); //ウィンドウエレメントを作成
			pParentElementObj->AddChildElementObj(pElementObj); //親エレメントに作成したエレメントを追加

			return TRUE;
		}

		//ボタン作成
		BOOL GUI::CreateButton(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption)
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //親となるエレメントを得る
			if (!pParentElementObj) //親が見つからなかった場合
				return FALSE;

			Element *pElementObj = new Button(this, rcPosition, ID, ParentID, z, pCaption); //ボタンエレメントを作成
			pParentElementObj->AddChildElementObj(pElementObj); //親エレメントに作成したエレメントを追加

			return TRUE;
		}

		//ラベル作成
		BOOL GUI::CreateLabel(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pStr) 
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //親となるエレメントを得る
			if (!pParentElementObj) //親が見つからなかった場合
				return FALSE;

			Element *pElementObj = new Label(this, rcPosition, ID, ParentID, z, pStr); //ラベルエレメントを作成
			pParentElementObj->AddChildElementObj(pElementObj); //親エレメントに作成したエレメントを追加

			return TRUE;
		}

		//コンボボックス作成
		BOOL GUI::CreateComboBox(ho::RectWH rcPosition, int ID, int ParentID, int z) 
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //親となるエレメントを得る
			if (!pParentElementObj) //親が見つからなかった場合
				return FALSE;

			Element *pElementObj = new ComboBox(this, rcPosition, ID, ParentID, z); //コンボボックスエレメントを作成
			pParentElementObj->AddChildElementObj(pElementObj); //親エレメントに作成したエレメントを追加

			return TRUE;
		}

		//チェックボックス作成
		BOOL GUI::CreateCheckBox(ho::RectWH rcPosition, int ID, int ParentID, int z, BOOL Check, TCHAR *pStr)
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //親となるエレメントを得る
			if (!pParentElementObj) //親が見つからなかった場合
				return FALSE;

			Element *pElementObj = new CheckBox(this, rcPosition, ID, ParentID, z, Check, pStr); //チェックボックスエレメントを作成
			pParentElementObj->AddChildElementObj(pElementObj); //親エレメントに作成したエレメントを追加

			return TRUE;
		}

		//ゲージスライダー作成
		BOOL GUI::CreateGaugeSlider(ho::RectWH rcPosition, int ID, int ParentID, int z, eHV hv, double Max, double Pos)
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //親となるエレメントを得る
			if (!pParentElementObj) //親が見つからなかった場合
				return FALSE;

			Element *pElementObj = new GaugeSlider(this, rcPosition, ID, ParentID, z, hv, Max, Pos); //ゲージスライダーエレメントを作成
			pParentElementObj->AddChildElementObj(pElementObj); //親エレメントに作成したエレメントを追加

			return TRUE;
		}

		//イメージ作成
		BOOL GUI::CreateImage(ho::RectWH rcPosition, int ID, int ParentID, int z, ComPtr<LPDIRECT3DTEXTURE9> cpTexture) 
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //親となるエレメントを得る
			if (!pParentElementObj) //親が見つからなかった場合
				return FALSE;

			Element *pElementObj = new Image(this, rcPosition, ID, ParentID, z, cpTexture); //ゲージスライダーエレメントを作成
			pParentElementObj->AddChildElementObj(pElementObj); //親エレメントに作成したエレメントを追加

			return TRUE;
		}

		//IDからエレメントを取得
		Element *GUI::GetpElementObj(int ID) 
		{
			Element *pElementObj = NULL;

			SendMsg(MESSAGE(this, GET_PTR, ID, 4, &pElementObj)); //データアドレスへエレメントオブジェクトのアドレスを格納するメッセージを送る

			return pElementObj;
		}

		//登録可能なシステムエレメントIDを取得
		int GUI::GetNullSystemElementID() 
		{
			for (int i = 0; i < signed(vectorSystemElementID.size()); i++)
				if (!vectorSystemElementID.at(i)) //配列内に空きがある場合
					return 0x40000000 + i; //IDを0x40000000から数えた値に変換して返す

			//配列内に1つも空きがない場合
			vectorSystemElementID.push_back(NULL); //配列の最後に格納
			return 0x40000000 + (signed)vectorSystemElementID.size(); //IDを返す
		}

		//作成したシステムエレメントを登録し、IDが返る
		int GUI::RegistSystemElementObj(Element *pElementObj) 
		{
			for (int i = 0; i < (signed)vectorSystemElementID.size(); i++)
			{
				if (!vectorSystemElementID.at(i)) //配列内に空きがある場合
				{
					vectorSystemElementID.at(i) = pElementObj; //エレメントオブジェクトへのポインタを格納
					return 0x40000000 + i; //IDを負の値に変換して返す
				}
			}

			//配列内に1つも空きがない場合
			vectorSystemElementID.push_back(pElementObj); //配列の最後に格納
			return -(signed)vectorSystemElementID.size(); //IDを返す
		}

		//指定されたIDのシステムエレメントを管理リストから削除する
		void GUI::DeleteSystemElementObj(int ID) 
		{
			if (ID >= 0x40000000 && (signed)vectorSystemElementID.size() > ID - 0x40000000) //IDが配列サイズ内の場合
				vectorSystemElementID.at(ID - 0x40000000) = NULL;

			return;
		}

		//三角形の3頂点の座標を計算する
		void GUI::CalcTriangleVertex(ho::D3DVERTEX_2D *pVertex, ho::PT<int> ptCenter, int Radius, ho::Direction8 Angle) 
		{
			double Angle2[3];
			Angle2[0] = atan2((double)Angle.GetXY().y, (double)Angle.GetXY().x); //三角形の向きの角度を求める
			Angle2[1] = Angle2[0] + PI * 2.0 / 3.0;
			Angle2[2] = Angle2[0] + PI * 4.0 / 3.0;

			for (int i = 0; i < 3; i++)
			{
				pVertex[i].x = float(ptCenter.x + cos(Angle2[i]) * Radius);
				pVertex[i].y = float(ptCenter.y + sin(Angle2[i]) * Radius);
			}

			return;
		}

		//矩形の枠を描画する
		void GUI::DrawBorder(RectWH *pRectWH, DWORD Color) 
		{
			D3DVERTEX_2D vt[4];

			D3DVERTEX_2D::SetVertexFromRECT(vt, &pRectWH->GethoRECT(-1), 0, Color);
			WORD Index[] = {0, 1, 3, 2, 0};
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, 0, 4, 4, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D));

			return;
		}









		//コンストラクタ
		Element::Element(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, Element *pParentElementObj) 
		{
			this->pGUIObj = pGUIObj;
			this->rcPosition = rcPosition;
			this->MinSize = ho::PT<int>(32, 32); //最小サイズ（暫定的にここで指定）
			this->ID = ID;
			this->ParentID = ParentID;
			this->z = z;
			this->bShow = TRUE;
			this->rcClientOffset = ho::RECT(0, 0, 0, 0);
			this->Hover = FALSE;

			this->pParentElementObj = pGUIObj->GetpElementObj(ParentID); //親エレメントのポインタを取得
			if (!this->pParentElementObj && pParentElementObj) //親エレメントが取得できず、引数に親エレメントへのポインタが指定されていた場合
				this->pParentElementObj = pParentElementObj;
		}

		//デストラクタ
		Element::~Element()
		{
			//すべての子供エレメントを削除
			for (std::list<Element *>::iterator itr = listpChildElementObj.begin(); itr != listpChildElementObj.end(); itr++)
				SDELETE(*itr);
		}

		//メッセージの経路選択
		void Element::MessageRouting(MESSAGE Message) 
		{
			BOOL ProcessMessage; //メッセージを処理するかどうか
			BOOL SendChild; //子供エレメントに送るかどうか

			//メッセージ配達判定
			if (Message.TargetID == -1) //再帰処理で全体に送る場合
			{
				ProcessMessage = TRUE;
				SendChild = TRUE;
			} else { //宛先が決まっている場合
				if (ID != Message.TargetID) //現在のエレメントが宛先ではなかった場合
				{
					ProcessMessage = FALSE;
					SendChild = TRUE;
				} else { //現在のエレメントが宛先だった場合
					ProcessMessage = TRUE;
					SendChild = FALSE;
				}
			}

			if (ProcessMessage) //メッセージの処理をする場合
				SendChild = MessageProcedure(Message, SendChild); //メッセージ処理プロシージャ

			if (SendChild) //子供エレメントにも送る場合
			{
				std::list<Element *> listTemp = listpChildElementObj; //一時リストにコピー（後のメッセージ処理中にこのリストの内容が変更される場合があるため）
				for (std::list<Element *>::iterator itr = listTemp.begin(); itr != listTemp.end(); itr++) //子供リストを走査
					(*itr)->MessageRouting(Message);
			}

			return;
		}

		//メッセージ処理プロシージャ
		BOOL Element::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			return DefaultMessageProcedure(Message, SendChild);
		}

		//デフォルトメッセージ処理プロシージャ
		BOOL Element::DefaultMessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case DRAW: //描画メッセージ
				if (bShow) //表示する場合
					Draw(); //描画
				else
					SendChild = FALSE; //子供オブジェクトにも送らない
				break;
			case GET_PTR: //エレメントオブジェクトのポインタを取得
				*(void **)Message.pParam = this;
				break;
			case SETPOS_CENTER: //クライアント領域の中央に移動
				SetPos_Center(Message);
				break;
			case GET_TOPID_FROMPOS:	//任意の座標上の一番上側のエレメントのIDを取得（12Byte、ho::PT<int>任意座標、int *IDを格納するポインタ）
				GetTopIDFromPos(Message);
				break;
			case SHOW: //表示、非表示の切り替え（4Byte、BOOL）
				Show(Message);
				break;
			case SHOW_SWITCH: //表示⇔非表示 を交互に気鋭変える
				pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::SHOW, Message.TargetID, 4, LPVOID(TRUE - bShow))); //メッセージ送信 
				break;
			case GET_SHOW: //表示、非表示の状態を取得する（4Byte、 *BOOL 結果を得るポインタ）
				*(BOOL *)Message.pParam = this->bShow;
				break;
			case SETTEXT: //ウィンドウやラベル等の文字列を設定（4Byte、TCHAR *）
				SetText(Message);
				break;
			case MOUSE_HOVER_BEGIN: //ホバー開始
				Hover = TRUE;
				break;
			case MOUSE_HOVER_MOVE: //ホバー移動
				{
					HANDLE hCursor = NULL;
					hCursor = LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, NULL, NULL, LR_SHARED); 
					if (hCursor) //カーソルがロードされていた場合
						SetCursor((HCURSOR)hCursor);//カーソルを設定
				}
				break;
			case MOUSE_HOVER_END: //ホバー終了
				Hover = FALSE;
				break;
			case POSITION: //エレメントの位置情報（rcPosition）を変更する（16Byte、RectWH *新しい位置情報）
				rcPosition = *(RectWH *)Message.pParam;
				if (rcPosition.width < MinSize.x)
					rcPosition.width = MinSize.x;
				if (rcPosition.height < MinSize.y)
					rcPosition.height = MinSize.y;
				break;
			case MOVE: //エレメントの左上座標を変更する（8Byte、ho::PT<int> *新しい座標）
				rcPosition.left = ((ho::PT<int> *)Message.pParam)->x;
				rcPosition.top = ((ho::PT<int> *)Message.pParam)->y;
				break;
			case GET_CLIENT_RECT: //エレメントのクライアント領域の絶対位置を取得（4Byte、RectWH *クライアント領域を書き込むポインタ）
				*(RectWH *)Message.pParam = GetAbsoluteClientPos();
				break;
			case SETFOREGROUND: //親エレメントのリスト上で一番最後（画面上では上側）に並び替える
				if (pParentElementObj) //親エレメントが存在する場合
					pParentElementObj->SetForegroundChildID(this->ID); //親エレメント側で現在のエレメントを最前面に並び替える
				break;
			}

			return SendChild;
		}

		//子供エレメントを追加
		void Element::AddChildElementObj(Element *pElementObj) 
		{
			listpChildElementObj.push_back(pElementObj);

			return;
		}

		//絶対位置を取得
		RectWH Element::GetAbsolutePos()
		{
			if (pParentElementObj) //親エレメントが存在する場合
				return pParentElementObj->GetAbsolutePosAlg(RectWH(rcPosition.left, rcPosition.top, rcPosition.width, rcPosition.height), 0); //内部再帰関数を実行

			return rcPosition;
		}

		//絶対位置を取得（内部再帰関数）
		RectWH Element::GetAbsolutePosAlg(RectWH Rect, int Level) 
		{
			Rect.left += this->rcPosition.left + rcClientOffset.left;
			Rect.top += this->rcPosition.top + rcClientOffset.top;

			if (pParentElementObj) //親エレメントが存在する場合
				Rect = pParentElementObj->GetAbsolutePosAlg(Rect, Level + 1);

			return Rect;
		}

		//クライアント領域のみの絶対位置を取得
		RectWH Element::GetAbsoluteClientPos() 
		{
			return GetAbsoluteClientPosAlg(RectWH(0, 0, rcPosition.width - rcClientOffset.left - rcClientOffset.right,
				rcPosition.height - rcClientOffset.top - rcClientOffset.bottom), 0); //内部再帰関数を実行
		}

		//クライアント領域のみの絶対位置を取得（内部再帰関数）
		RectWH Element::GetAbsoluteClientPosAlg(RectWH Rect, int Level) 
		{
			Rect.left += this->rcPosition.left + rcClientOffset.left;
			Rect.top += this->rcPosition.top + rcClientOffset.top;

			if (pParentElementObj) //親エレメントが存在する場合
				Rect = pParentElementObj->GetAbsoluteClientPosAlg(Rect, Level + 1);

			return Rect;
		}

		//GET_TOPID_FROMPOS（任意の座標上の一番上側のエレメントのIDを取得）
		void Element::GetTopIDFromPos(MESSAGE Message) 
		{
			ho::PT<int> ptClickPos = *(ho::PT<int> *)Message.pParam; //メッセージからマウスクリック位置を取得
	
			//子供エレメントを走査してクリック座標が含まれる場合はメッセージを転送する
			if (listpChildElementObj.size() >= 1) //子供エレメントが存在する場合
			{
				//手前にあるエレメントがリストの最後にあるので、大きい方から（逆方向から）走査する
				std::list<Element *>::iterator itr = listpChildElementObj.end();
				do
				{
					itr--;
					if ((*itr)->GetAbsolutePos().GethoRECT(-1).JudgeContain(ptClickPos) && (*itr)->GetbShow()) //エレメントの範囲内でかつ表示中の場合
					{
						(*itr)->MessageRouting(MESSAGE(Message.pGUIObj, Message.Message, (*itr)->GetID(), Message.Bytes, Message.pParam)); //エレメントにクリックメッセージを送る
						return;
					}
				} while (itr != listpChildElementObj.begin());
			}

			//座標に子供エレメントが含まれていなかった場合
			*(int *)((BYTE *)Message.pParam + sizeof(ho::PT<int>)) = this->ID; //メッセージのバッファにクリック確定したエレメントのIDを格納

			return;
		}

		//クライアント領域の中央に移動
		void Element::SetPos_Center(MESSAGE Message) 
		{
			if (!pParentElementObj) //親エレメントが存在しない場合
				return;

			ho::RectWH rcParentPos = pParentElementObj->GetrcPosition(); //親オブジェクトの位置情報を得る
			this->rcPosition.left = int(rcParentPos.width * 0.5 - this->rcPosition.width * 0.5);
			this->rcPosition.top = int(rcParentPos.height * 0.5 - this->rcPosition.height * 0.5);

			return;
		}

		//表示、非表示の切り替え（4Byte、BOOL）
		void Element::Show(MESSAGE Message) 
		{
			if ((BOOL)Message.pParam)
				bShow = TRUE;
			else if (!(BOOL)Message.pParam)
				bShow = FALSE;

			return;
		}

		//絶対座標からエレメント内の相対座標に変換
		ho::PT<int> Element::GetLocalPtFromAbsolutePt(ho::PT<int> ptPos) 
		{
			RectWH rcAbsolutePos = this->GetAbsolutePos(); //エレメントの絶対座標を取得

			return ho::PT<int>(ptPos.x - rcAbsolutePos.left, ptPos.y - rcAbsolutePos.top); //座標を相対座標に変換

		}

		//任意のIDの子供オブジェクトを最前面に並び替える
		void Element::SetForegroundChildID(int ID) 
		{
			Element *pElementObj = NULL;

			//引数のIDを持つエレメントをリストから発見して削除
			for (std::list<Element *>::iterator itr = listpChildElementObj.begin(); itr != listpChildElementObj.end(); itr++)
			{
				if ((*itr)->GetID() == ID)
				{
					pElementObj = *itr;
					listpChildElementObj.erase(itr);
					break;
				}
			}

			if (pElementObj) //リストから発見されていた場合
				listpChildElementObj.push_back(pElementObj); //リストの最後に追加

			return;
		}








		//コンストラクタ
		Window::Window(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption, BOOL Show, eWindowStyle::e WindowStyle) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->BorderWidth = 2; //暫定的に2としてある
			strCaption = pCaption;
			this->bShow = Show;
			this->WindowStyle = WindowStyle;
			this->rcClientOffset = ho::RECT(BorderWidth, pGUIObj->GetFontHeight() + 8, BorderWidth, BorderWidth);
		}

		//デストラクタ
		Window::~Window()
		{
		}

		//描画
		void Window::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			//ウィンドウ全体
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(), 0, 0x3f000000);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画

			//タイトルバー
			RectWH rcTitleBar = rcAbsolutePos;
			rcTitleBar.left += BorderWidth;
			rcTitleBar.top += BorderWidth;
			rcTitleBar.width -= BorderWidth * 2;
			rcTitleBar.height = pGUIObj->GetFontHeight() + 4;
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcTitleBar.GethoRECT(), 0, 0x3f000000);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画

			pGUIObj->GetpDrawStringObj()->Draw(rcTitleBar.left + 2, rcTitleBar.top + 2, (TCHAR *)strCaption.c_str(), pGUIObj->GetFontHeight(), 0xffffffff); //文字列描画

			return;
		}

		//文字列を設定
		void Window::SetText(MESSAGE Message) 
		{
			if (Message.pParam) //文字列のアドレスが存在している場合
				strCaption = (TCHAR *)Message.pParam;
			else //アドレスがnullの場合
				strCaption.clear(); //文字列を消去

			return;
		}

		//メッセージ処理プロシージャ
		BOOL Window::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case MOUSE_LDRAG_BEGIN: //マウスの左ボタンでドラッグ開始（8Byte、ho::PT<int>クリック時の座標）
				this->rcDragBeginPos = rcPosition; //ドラッグ開始時の位置を保存
				this->DragBeginRegion = GetRegion(&GetLocalPtFromAbsolutePt(*(ho::PT<int> *)Message.pParam)); //ドラッグ開始時の領域を保存
				break;
			case MOUSE_LDRAG_MOVE: //マウスの左ボタンでドラッグ移動（16Byte、ho::PT<int>クリック時の座標、ho::PT<int>現在のマウス座標）
				Mouse_LDrag_Move(Message); //マウス左ボタンでドラッグ移動時の処理
				break;
			case MOUSE_HOVER_MOVE: //マウスホバー中に移動（8Byte、ho::PT<int> *移動中のマウス座標）
				MouseHover((ho::PT<int> *)Message.pParam); //マウスホバー中のカーソルアイコン変更処理
				break;
			default:
				SendChild = DefaultMessageProcedure(Message, SendChild); //このプロシージャで処理されなかったメッセージはデフォルトプロシージャへ送られる
				break;
			}

			return SendChild;
		}

		//マウス左ボタンでドラッグ移動時の処理
		void Window::Mouse_LDrag_Move(MESSAGE Message) 
		{
			ho::PT<int> ptSub = ((ho::PT<int> *)Message.pParam)[1] - ((ho::PT<int> *)Message.pParam)[0]; //クリック時から現在位置までの移動量
			RectWH rcNewPosition = rcPosition; //新しい位置情報

			switch (DragBeginRegion) //ドラッグ開始時のマウス座標の領域によって分岐
			{
			case Left:
				rcNewPosition.left = rcDragBeginPos.left + ptSub.x;
				rcNewPosition.width = rcDragBeginPos.width - ptSub.x;
				break;
			case Right:
				rcNewPosition.width = rcDragBeginPos.width + ptSub.x;
				break;
			case Top:
				rcNewPosition.top = rcDragBeginPos.top + ptSub.y;
				rcNewPosition.height = rcDragBeginPos.height - ptSub.y;
				break;
			case Bottom:
				rcNewPosition.height = rcDragBeginPos.height + ptSub.y;
				break;
			case LeftTop:
				rcNewPosition.left = rcDragBeginPos.left + ptSub.x;
				rcNewPosition.width = rcDragBeginPos.width - ptSub.x;
				rcNewPosition.top = rcDragBeginPos.top + ptSub.y;
				rcNewPosition.height = rcDragBeginPos.height - ptSub.y;
				break;
			case RightTop:
				rcNewPosition.width = rcDragBeginPos.width + ptSub.x;
				rcNewPosition.top = rcDragBeginPos.top + ptSub.y;
				rcNewPosition.height = rcDragBeginPos.height - ptSub.y;
				break;
			case LeftBottom:
				rcNewPosition.left = rcDragBeginPos.left + ptSub.x;
				rcNewPosition.width = rcDragBeginPos.width - ptSub.x;
				rcNewPosition.height = rcDragBeginPos.height + ptSub.y;
				break;
			case RightBottom:
				rcNewPosition.width = rcDragBeginPos.width + ptSub.x;
				rcNewPosition.height = rcDragBeginPos.height + ptSub.y;
				break;
			case TitleBar: //タイトルバー
				rcNewPosition.left = rcDragBeginPos.left + ptSub.x;
				rcNewPosition.top = rcDragBeginPos.top + ptSub.y;
				break;
			}

			if (rcPosition != rcNewPosition) //位置情報が変更される場合
				pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::POSITION, Message.TargetID, 16, &rcNewPosition)); //位置情報変更のメッセージを送信

			return;
		}

		//エレメント内の座標から領域を取得
		Window::eRegion Window::GetRegion(ho::PT<int> *pptLocalPos) 
		{
			const int CornerSize = 24; //角の大きさ

			if (pptLocalPos->x < BorderWidth) //左側の枠内の場合
			{
				if (pptLocalPos->y < CornerSize)
					return LeftTop;
				else if (pptLocalPos->y >= rcPosition.height - CornerSize)
					return LeftBottom;
				return Left;
			} else if (pptLocalPos->x >= rcPosition.width - BorderWidth) //右側の枠内の場合
			{
				if (pptLocalPos->y < CornerSize)
					return RightTop;
				else if (pptLocalPos->y >= rcPosition.height - CornerSize)
					return RightBottom;
				return Right;
			} else if (pptLocalPos->y < BorderWidth) //上側の枠内の場合
			{
				if (pptLocalPos->x < CornerSize)
					return LeftTop;
				else if (pptLocalPos->x >= rcPosition.width - CornerSize)
					return RightTop;
				return Top;
			} else if (pptLocalPos->y >= rcPosition.height - BorderWidth) //下側の枠内の場合
			{
				if (pptLocalPos->x < CornerSize)
					return LeftBottom;
				else if (pptLocalPos->x >= rcPosition.width - CornerSize)
					return RightBottom;
				return Bottom;
			} else if (pptLocalPos->y < BorderWidth + pGUIObj->GetFontHeight() + 4) //タイトルバーの場合
			{
				return TitleBar;
			}

			return Client; //クライアント領域
		}

		//マウスホバー中のカーソルアイコン変更処理
		void Window::MouseHover(ho::PT<int> *ptMousePos) 
		{
			ho::PT<int> ptLocalMousePos = GetLocalPtFromAbsolutePt(*ptMousePos); //マウス座標をローカル座標に変換

			eRegion Region = GetRegion(&ptLocalMousePos); //マウス座標から領域を取得
			HANDLE hCursor = NULL;

			switch (Region) //取得した領域によって分岐
			{
			case LeftTop:
			case RightBottom:
				if (WindowStyle & eWindowStyle::RESIZE)
					hCursor = LoadImage(NULL, IDC_SIZENWSE, IMAGE_CURSOR, NULL, NULL, LR_SHARED);
				break;
			case Top:
			case Bottom:
				if (WindowStyle & eWindowStyle::RESIZE)
					hCursor = LoadImage(NULL, IDC_SIZENS, IMAGE_CURSOR, NULL, NULL, LR_SHARED);
				break;
			case RightTop:
			case LeftBottom:
				if (WindowStyle & eWindowStyle::RESIZE)
					hCursor = LoadImage(NULL, IDC_SIZENESW, IMAGE_CURSOR, NULL, NULL, LR_SHARED);
				break;
			case Left:
			case Right:
				if (WindowStyle & eWindowStyle::RESIZE)
					hCursor = LoadImage(NULL, IDC_SIZEWE, IMAGE_CURSOR, NULL, NULL, LR_SHARED);
				break;
			case TitleBar:
				hCursor = LoadImage(NULL, IDC_SIZEALL, IMAGE_CURSOR, NULL, NULL, LR_SHARED);
				break;
			case Client:
				hCursor = LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, NULL, NULL, LR_SHARED);
				break;
			}

			if (hCursor) //カーソルがロードされていた場合
				SetCursor((HCURSOR)hCursor);

			return;
		}













		//コンストラクタ
		Button::Button(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			strCaption = pCaption;
			this->Enable = TRUE;
		}

		//デストラクタ
		Button::~Button()
		{
		}

		//描画
		void Button::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			//ホバー状態によって色を変更
			DWORD Color; //ボタンの色
			if (Hover)
			{
				Color = 0x7f0000ff;
			} else {
				//稀に点滅
				DWORD ModTime = pGUIObj->GetTime() % 180;
				if (ModTime < 10)
					Color = 0x7f000000 | ((0x7f - ModTime * 12) << 16) | ((0x7f - ModTime * 12) << 8) | (0x7f + ModTime * 12);
				else if (ModTime < 20)
					Color = 0x7f000000 | ((0x7f - (20 - ModTime) * 12) << 16) | ((0x7f - (20 - ModTime) * 12) << 8) | (0x7f + (20 - ModTime) * 12);
				else
					Color = 0x7f7f7f7f;
			}

			//ボタン全体
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(), (float)0, Color);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画

			pGUIObj->GetpDrawStringObj()->Draw(rcAbsolutePos.left + 2, rcAbsolutePos.top + 2, (TCHAR *)strCaption.c_str(), pGUIObj->GetFontHeight(), 0xffffffff); //文字列描画

			return;
		}
















		//コンストラクタ
		Label::Label(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pStr) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			strLabel = pStr;
		}

		//デストラクタ
		Label::~Label()
		{
		}

		//描画
		void Label::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();
			//rcAbsolutePos.top += pGUIObj->GetFontHeight() + 8; //タイトルバー分を下に下げる

			ho::PT<int> ptDrawPos(rcAbsolutePos.left, rcAbsolutePos.top); //文字描画位置
			TCHAR *pStr = (TCHAR *)strLabel.c_str(); //描画する文字列のポインタ
			DWORD DrawedCharNum = 0; //描画した文字数

			while (DrawedCharNum < strLabel.size()) //全部の文字の描画が終わるまで繰り返す
			{
				DrawedCharNum += pGUIObj->GetpDrawStringObj()->DrawWidth(ptDrawPos.x, ptDrawPos.y, pStr + DrawedCharNum, rcPosition.width, 16); //幅制限付きで文字を描画
				ptDrawPos.y += 20;
			}

			return;
		}

		//文字列を設定
		void Label::SetText(MESSAGE Message) 
		{
			if (Message.pParam) //文字列のアドレスが存在している場合
				strLabel = (TCHAR *)Message.pParam;
			else //アドレスがnullの場合
				strLabel.clear(); //文字列を消去

			return;
		}














		//コンストラクタ
		ComboBox::ComboBox(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->ScrollBarSize = 16; //スクロールバーのサイズ

			this->rcOpenPosition = rcPosition;
			this->rcPosition.height = pGUIObj->GetFontHeight() + 4;
			SelectIndex = -1;
			ListOpen = FALSE;
			this->HoverPos = -2;

			this->pScrollBarObj = new ScrollBar(pGUIObj, ho::RectWH(rcOpenPosition.width - ScrollBarSize, pGUIObj->GetFontHeight() + 4, ScrollBarSize, rcOpenPosition.height - (pGUIObj->GetFontHeight() + 4)), pGUIObj->GetNullSystemElementID(), ID, z, this, ScrollBar::Vertical, 7, 0, 7); //縦方向スクロールバーを作成
			pGUIObj->RegistSystemElementObj(pScrollBarObj); //スクロールバーをシステムエレメントとして登録
			AddChildElementObj(pScrollBarObj); //子供エレメントとしてコンボボックスエレメントに登録
			pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SHOW, pScrollBarObj->GetID(), 4, (LPVOID)FALSE)); //非表示にするメッセージを送信
		}

		//デストラクタ
		ComboBox::~ComboBox()
		{
			pGUIObj->DeleteSystemElementObj(pScrollBarObj->GetID()); //システムエレメントを削除
		}

		//描画
		void ComboBox::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			DWORD BoxColor = 0x7f7f7f7f; //上部ボックスの色
			if (HoverPos == -1) //上部ボックスがホバー中の場合
				BoxColor = 0x7f0000ff;

			//上部のボックス
			RectWH rcBox = rcAbsolutePos;
			rcBox.height = pGUIObj->GetFontHeight() + 4;
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcBox.GethoRECT(), 0, BoxColor);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画
			//ボックス内の三角
			DWORD ColorTriangle = 0xffffffff;
			ho::D3DVERTEX_2D vtTriangle[3];
			for (int i = 0; i < 3; i++)
				vtTriangle[i].SetVertex(0, 0, 0, ColorTriangle, 0, 0); //色などを設定
			pGUIObj->CalcTriangleVertex(vtTriangle, ho::PT<int>(rcAbsolutePos.left + rcAbsolutePos.width - int(pGUIObj->GetFontHeight() * 0.5f) - 2, rcAbsolutePos.top + int(pGUIObj->GetFontHeight() * 0.5f) + 2), int(pGUIObj->GetFontHeight() * 0.4f), ho::Direction8(ho::Direction8::Down)); //三角形の頂点座標を計算
			D3DVERTEX_2D::Offset(vtTriangle, 3);
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vtTriangle, sizeof(D3DVERTEX_2D)); //三角形描画
			//ボックス内の文字列
			int i = 0;
			for (std::list<tstring>::iterator itr = listStr.begin(); itr != listStr.end(); itr++) //文字列リストを走査
			{
				if (i == SelectIndex)
				{
					pGUIObj->GetpDrawStringObj()->Draw(rcAbsolutePos.left + 2, rcAbsolutePos.top + 2, (TCHAR *)itr->c_str(), pGUIObj->GetFontHeight(), 0xffffffff); //文字列描画
					break;
				}
				i++;
			}

			if (ListOpen) //リストが開いている場合
			{
				const DWORD ListColor = 0x7f7f7f7f;
				const DWORD ListHoverColor = 0x7f0000ff;
				int Lines = GetListViewLines(); //表示可能な行数

				//リストの背景の色を描画
				int Y = rcAbsolutePos.top + pGUIObj->GetFontHeight() + 4; //リスト要素のY座標
				if (HoverPos >= 0) //リスト内をホバー中の場合
				{
					RectWH rcList = rcAbsolutePos;
					rcList.width -= ScrollBarSize;
					for (int i = 0; i < Lines;) //行数分を走査
					{
						if (HoverPos == i) //現在の行がホバー中の場合
						{
							rcList.top = Y + (pGUIObj->GetFontHeight() + 4) * i;
							rcList.height = pGUIObj->GetFontHeight() + 4;
							D3DVERTEX_2D::SetVertexFromRECT(vt, &rcList.GethoRECT(), 0, ListHoverColor);
							D3DVERTEX_2D::Offset(vt, 4);
							pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画
							i++;
						} else { //ホバー中ではない場合
							if (i < HoverPos) //ホバー位置より上の場合
							{
								rcList.top = Y + (pGUIObj->GetFontHeight() + 4) * i;
								rcList.height = (pGUIObj->GetFontHeight() + 4) * (HoverPos - i);
								D3DVERTEX_2D::SetVertexFromRECT(vt, &rcList.GethoRECT(), 0, ListColor);
								D3DVERTEX_2D::Offset(vt, 4);
								pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画
								i = HoverPos;
							} else { //ホバー位置より下の場合
								rcList.top = Y + (pGUIObj->GetFontHeight() + 4) * i;
								rcList.height = (pGUIObj->GetFontHeight() + 4) * (Lines - i);
								D3DVERTEX_2D::SetVertexFromRECT(vt, &rcList.GethoRECT(), 0, ListColor);
								D3DVERTEX_2D::Offset(vt, 4);
								pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画
								i = Lines;
							}
						}
					}
				} else { //リスト内でホバーしていない場合
					RectWH rcList = rcAbsolutePos;
					rcList.top += pGUIObj->GetFontHeight() + 4;
					rcList.height -= pGUIObj->GetFontHeight() + 4;
					D3DVERTEX_2D::SetVertexFromRECT(vt, &rcList.GethoRECT(), 0, ListColor);
					D3DVERTEX_2D::Offset(vt, 4);
					pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画
				}

				//スクロールバーの開始位置の分だけ文字列リストのイテレータを進める
				std::list<tstring>::iterator itr = listStr.begin(); //文字列リスト
				int ScrollBarPos;
				pGUIObj->SendMsg(MESSAGE(pGUIObj, SB_GETPOS, pScrollBarObj->GetID(), 4, &ScrollBarPos));
				for (int i = 0; i < ScrollBarPos; i++)
					itr++;

				int i = 0;
				for (; itr != listStr.end(); itr++)
				{
					if (i >= Lines) //表示可能な行数を過ぎた場合
						break;
					pGUIObj->GetpDrawStringObj()->Draw(rcAbsolutePos.left + 2, Y, (TCHAR *)itr->c_str(), pGUIObj->GetFontHeight(), 0xffffffff); //文字列描画
					Y += pGUIObj->GetFontHeight() + 4;
					i++;
				}
			}

			return;
		}

		//メッセージ処理プロシージャ
		BOOL ComboBox::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case MOUSE_LCLICK: //マウスクリックされた場合
				MouseClick(Message); //マウスクリック時の処理
				break;
			case CB_PUSH: //コンボボックスの最後に要素を追加（4Byte、TCHAR *）
				CB_Push(Message);
				break;
			case CB_GETSELECT: //コンボボックスの選択された要素のインデックスを得る（4Byte, 結果を得るint型へのポインタ）
				*(int *)Message.pParam = SelectIndex;
				break;
			case CB_SETSELECT: //コンボボックスの要素を選択する（4Byte、int 選択する要素数）
				SelectIndex = (int)Message.pParam;
				break;
			case MOUSE_HOVER_MOVE: //マウスホバー中に移動（8Byte、ho::PT<int>移動中のマウス座標）
				MouseHoverMove(Message);
				break;
			case MOUSE_HOVER_END: //マウスホバー終了
				HoverPos = -2;
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}

		//マウスクリック時の処理
		void ComboBox::MouseClick(MESSAGE Message) 
		{
			ho::PT<int> ptClickPos = *(ho::PT<int> *)Message.pParam; //マウスクリック座標
			ptClickPos = GetLocalPtFromAbsolutePt(ptClickPos); //座標を絶対座標からコンボボックス内のローカル座標に変換

			if (ptClickPos.y >= pGUIObj->GetFontHeight() + 4) //クリック座標がリスト内の場合
			{
				int Index = GetListIndexFromMouseLocalPt(ptClickPos); //マウスのローカル座標から選択されるリストのインデックスを取得
				pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::CB_SETSELECT, Message.TargetID, 4, (LPVOID)Index)); //コンボボックスの任意要素を選択するメッセージを送信
			}

			ListOpen = TRUE - ListOpen; //リストの開閉

			if (ListOpen) //リストが開いた場合
			{
				this->rcPosition = rcOpenPosition;

				if ((signed)listStr.size() > GetListViewLines()) //開かれた状態のリスト部分に一度に表示しきれない場合
					pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SHOW, pScrollBarObj->GetID(), 4, (LPVOID)TRUE)); //スクロールバーを表示するメッセージを送信
				pGUIObj->PostMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::SETFOREGROUND, this->ID, 0, NULL)); //親オブジェクト側で、コンボボックスを最前面表示に設定するメッセージを送信
			} else {
				this->rcPosition.height = pGUIObj->GetFontHeight() + 4;
				pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SHOW, pScrollBarObj->GetID(), 4, (LPVOID)FALSE)); //スクロールバーを非表示にするメッセージを送信
			}

			return;
		}

		//マウスのローカル座標から選択されるリストのインデックスを取得
		int ComboBox::GetListIndexFromMouseLocalPt(ho::PT<int> ptMouseLocalPos) 
		{
			int ScrollBarPos;
			pGUIObj->SendMsg(MESSAGE(pGUIObj, SB_GETPOS, pScrollBarObj->GetID(), 4, &ScrollBarPos)); //スクロールバーの位置を取得

			int ClickIndex = ptMouseLocalPos.y / (pGUIObj->GetFontHeight() + 4) - 1 + ScrollBarPos; //マウス座標からクリックされたリスト文字列を識別

			return ClickIndex;
		}

		//コンボボックスの最後に要素を追加（4Byte、TCHAR *）
		void ComboBox::CB_Push(MESSAGE Message) 
		{
			listStr.push_back(tstring((TCHAR *)Message.pParam));

			AdjustScrollBarSize(); //リストの長さに応じてスクロールバーのサイズを調整

			return;
		}

		//リストの長さに応じてスクロールバーのサイズを調整
		void ComboBox::AdjustScrollBarSize() 
		{
			int Lines = GetListViewLines(); //開かれた状態のリスト部分に表示可能な行数を取得

			if ((signed)listStr.size() > Lines) //開かれた状態のリスト部分に一度に表示しきれない場合
			{
				pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SB_SETMAX, pScrollBarObj->GetID(), 4, LPVOID(listStr.size() - 1))); //スクロールバーの最大値を設定
				pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SB_SETRANGE, pScrollBarObj->GetID(), 4, (LPVOID)Lines)); //スクロールバーのレンジを設定
			}

			return;
		}

		//開かれた状態のリスト部分に表示可能な行数を取得
		int ComboBox::GetListViewLines() 
		{
			return (rcOpenPosition.height - pGUIObj->GetFontHeight() + 4) / (pGUIObj->GetFontHeight() + 4); //表示可能な行数
		}

		//マウスホバー中に移動
		void ComboBox::MouseHoverMove(MESSAGE Message)
		{
			ho::PT<int> ptClickPos = *(ho::PT<int> *)Message.pParam; //マウスクリック座標
			ptClickPos = GetLocalPtFromAbsolutePt(ptClickPos); //座標を絶対座標からコンボボックス内のローカル座標に変換

			if (ptClickPos.y >= pGUIObj->GetFontHeight() + 4) //クリック座標がリスト内の場合
			{
				int ScrollBarPos;
				pGUIObj->SendMsg(MESSAGE(pGUIObj, SB_GETPOS, pScrollBarObj->GetID(), 4, &ScrollBarPos)); //スクロールバーの位置を取得

				HoverPos = GetListIndexFromMouseLocalPt(ptClickPos) - ScrollBarPos; //マウスのローカル座標から選択されるリストのインデックスを取得

			} else {
				HoverPos = -1;
			}

			return;
		}















		//コンストラクタ
		ScrollBar::ScrollBar(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, Element *pParentElementObj, eHV hv, int Max, int Pos, int Range) : Element(pGUIObj, rcPosition, ID, ParentID, z, pParentElementObj)
		{
			this->hv = hv;
			this->Max = Max;
			this->Pos = Pos;
			this->Range = Range;
			this->DragMoveDistance = 0;

			if (hv == Horizonal) //水平方向の場合
				Size = this->rcPosition.height;
			else if (hv == Vertical) //縦方向の場合
				Size = this->rcPosition.width;
		}

		//デストラクタ
		ScrollBar::~ScrollBar()
		{
		}

		//描画
		void ScrollBar::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			const DWORD BorderColor = 0x7fffffff;

			//全体の外枠を描画
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(-1), 0, BorderColor);
			WORD Index[] = {0, 1, 3, 2, 0};
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, 0, 4, 4, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画

			//両側のボタンとスライダーとの境目の2つの線を描画
			if (hv == Horizonal) //水平方向の場合
			{
				;
			} else if (hv == Vertical) //垂直方向の場合
			{
				vt[0].SetVertex(float(rcAbsolutePos.left), float(rcAbsolutePos.top + Size - 1), 0, BorderColor, 0, 0);
				vt[1].SetVertex(float(rcAbsolutePos.left + rcAbsolutePos.width - 1), float(rcAbsolutePos.top + Size - 1), 0, BorderColor, 0, 0);
				pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vt, sizeof(D3DVERTEX_2D));
				vt[0].SetVertex(float(rcAbsolutePos.left), float(rcAbsolutePos.top + rcAbsolutePos.height - 1 - Size + 1), 0, BorderColor, 0, 0);
				vt[1].SetVertex(float(rcAbsolutePos.left + rcAbsolutePos.width - 1), float(rcAbsolutePos.top + rcAbsolutePos.height - 1 - Size + 1), 0, BorderColor, 0, 0);
				pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vt, sizeof(D3DVERTEX_2D));

				pGUIObj->CalcTriangleVertex(vt, ho::PT<int>(rcAbsolutePos.left + int(Size * 0.5f), rcAbsolutePos.top + int(Size * 0.5f)), int(Size * 0.4f), ho::Direction8(ho::Direction8::Up)); //三角形の頂点座標を計算
				D3DVERTEX_2D::Offset(vt, 3);
				pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vt, sizeof(D3DVERTEX_2D)); //三角形描画
				pGUIObj->CalcTriangleVertex(vt, ho::PT<int>(rcAbsolutePos.left + int(Size * 0.5f), rcAbsolutePos.top + rcAbsolutePos.height - int(Size * 0.5f)), int(Size * 0.4f), ho::Direction8(ho::Direction8::Down)); //三角形の頂点座標を計算
				D3DVERTEX_2D::Offset(vt, 3);
				pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vt, sizeof(D3DVERTEX_2D)); //三角形描画

			}

			const DWORD SliderColor = 0x7f0000ff;

			//スライダーを描画
			if (hv == Horizonal) //水平方向の場合
			{
				;
			} else if (hv == Vertical) //垂直方向の場合
			{
				int SliderHeight; //スライダー部分の高さ
				int SliderBeginY; //スライダー部分の開始Y座標
				double HeightPerPos; //1つ分のスライダー内の高さ
				int Y1; //ツマミの開始座標
				int Y2; //ツマミの終了座標
				GetVerticalSliderDimension(SliderHeight, SliderBeginY, HeightPerPos, Y1, Y2, rcAbsolutePos); //縦方向スクロールバーのスライダー部分の寸法を取得

				vt[0].SetVertex(float(rcAbsolutePos.left + 1), float(Y1), 0, SliderColor, 0, 0);
				vt[1].SetVertex(float(rcAbsolutePos.left + rcAbsolutePos.width - 1), float(Y1), 0, SliderColor, 0, 0);
				vt[2].SetVertex(float(rcAbsolutePos.left + 1), float(Y2), 0, SliderColor, 0, 0);
				vt[3].SetVertex(float(rcAbsolutePos.left + rcAbsolutePos.width - 1), float(Y2), 0, SliderColor, 0, 0);
				D3DVERTEX_2D::Offset(vt, 4);
				pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D));
			}

			return;
		}

		//縦方向スクロールバーのスライダー部分の寸法を取得
		void ScrollBar::GetVerticalSliderDimension(int &SliderHeight, int &SliderBeginY, double &HeightPerPos, int &Y1, int &Y2, RectWH rcAbsolutePos) 
		{
			/* 引数の rcAbsolutePos により、スクロールバーのローカル座標で取得するかスクリーン全体の絶対座標で取得するかが決まる。 */

			SliderHeight = rcPosition.height - Size * 2; //スライダー部分の高さ
			SliderBeginY = rcAbsolutePos.top + Size; //スライダー部分の開始Y座標
			HeightPerPos = ((double)SliderHeight / double(Max + 1)); //1つ分のスライダー内の高さ

			if (DragMoveDistance) //マウスドラッグによる移動がある場合
			{
				//現在の選択位置からツマミ座標を計算
				Y1 = int(SliderBeginY + HeightPerPos * DragBeginPos); //ツマミの開始座標
				Y2 = int(SliderBeginY + HeightPerPos * (DragBeginPos + Range)) - 1; //ツマミの終了座標

				Y1 += DragMoveDistance; //ツマミの開始座標にマウスドラッグ分を加算
				Y2 += DragMoveDistance; //ツマミの終了座標にマウスドラッグ分を加算
				if (Y1 < SliderBeginY) //ツマミがスライダー開始位置よりも高かった場合
				{
					Y1 = SliderBeginY;
					Y2 = Y1 + int(HeightPerPos * Range);
					Pos = 0;
				} else if (Y2 >= SliderBeginY + SliderHeight) //ツマミの下側がスライダー終了位置よりも低い場合
				{
					Y2 = SliderBeginY + SliderHeight;
					Y1 = Y2 - int(HeightPerPos * Range) + 1;
					Pos = Max - Range + 1;
				} else {
					Pos = int((Y1 - SliderBeginY) / HeightPerPos); //マウスドラッグから選択位置を再計算
				}

			} else { //マウスドラッグによる移動がない場合
				//現在の選択位置からツマミ座標を計算
				Y1 = int(SliderBeginY + HeightPerPos * Pos); //ツマミの開始座標
				Y2 = int(SliderBeginY + HeightPerPos * (Pos + Range)); //ツマミの終了座標
			}

			return;
		}


		//メッセージプロシージャ
		BOOL ScrollBar::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case MOUSE_LCLICK: //マウスクリックされた場合
				MouseClick(Message); //マウスクリック時の処理
				break;
			case SB_SETMAX: //スクロールバーの最大値を設定（4Byte、int）
				Max = (int)Message.pParam;
				break;
			case SB_SETPOS: //スクロールバーの位置を設定（4Byte, int）
				Pos = (int)Message.pParam;
				break;
			case SB_SETRANGE: //スクロールバーの表示領域を設定（4Byte, int）
				Range = (int)Message.pParam;
				break;
			case SB_GETPOS: //スクロールバーの現在位置を取得(4Byte、*int）
				*(int *)Message.pParam = Pos;
				break;
			case MOUSE_LDRAG_BEGIN: //マウスの左ボタンでドラッグ開始（8Byte、ho::PT<int>クリック時の座標）
				DragBeginPos = Pos; //ドラッグ開始時の選択位置を保存
				break;
			case MOUSE_LDRAG_MOVE: //マウスの左ボタンでドラッグ移動（16Byte、ho::PT<int>クリック時の座標、ho::PT<int>現在のマウス座標）
				MouseDragMove(Message); 
				break;
			case MOUSE_LDRAG_END: //マウスの左ボタンでドラッグ終了（16Byte、ho::PT<int>クリック時の座標、ho::PT<int>現在のマウス座標）
				DragMoveDistance = 0;
				//Pos = DragBeginPos;
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}

		//マウスクリック時の処理
		void ScrollBar::MouseClick(MESSAGE Message) 
		{
			ho::PT<int> ptClickPos = *(ho::PT<int> *)Message.pParam; //マウスクリック座標
			ptClickPos = GetLocalPtFromAbsolutePt(ptClickPos); //座標を絶対座標からスクロールバー内のローカル座標に変換

			if (hv == Horizonal) //水平方向スクロールバーの場合
			{
				;
			} else if (hv == Vertical) //垂直方向スクロールバーの場合
			{
				if (ptClickPos.y < Size) //上側のボタンが押された場合
				{
					Pos--;
				} else if (ptClickPos.y >= rcPosition.height - Size) //下側のボタンが押された場合
				{
					Pos++;
				} else { //スライダー内が押された場合
					int SliderHeight; //スライダー部分の高さ
					int SliderBeginY; //スライダー部分の開始Y座標
					double HeightPerPos; //1つ分のスライダー内の高さ
					int Y1; //ツマミの開始座標
					int Y2; //ツマミの終了座標
					GetVerticalSliderDimension(SliderHeight, SliderBeginY, HeightPerPos, Y1, Y2, RectWH(0, 0, rcPosition.width, rcPosition.height)); //縦方向スクロールバーのスライダー部分の寸法を取得

					if (ptClickPos.y < Y1) //ツマミより上の部分が押された場合
					{
						Pos -= 3;
					} else if (ptClickPos.y > Y2) //ツマミより下の部分が押された場合
					{
						Pos += 3;
					}
				}
			}

			Clamp(); //設定値がはみ出さないようにする

			return;
		}

		//設定値がはみ出さないようにする
		void ScrollBar::Clamp() 
		{
			if (Pos < 0)
				Pos = 0;
			else if (Pos + Range - 1 > Max)
				Pos = Max - (Range - 1);

			return;
		}

		//マウスドラッグ移動時の処理
		void ScrollBar::MouseDragMove(MESSAGE Message) 
		{
			if (hv = Horizonal) //水平方向スクロールバーの場合
			{
				DragMoveDistance = ((ho::PT<int> *)Message.pParam)[1].x - ((ho::PT<int> *)Message.pParam)[0].x;
			} else if (hv = Vertical) //垂直方向スクロールバーの場合
			{
				DragMoveDistance = ((ho::PT<int> *)Message.pParam)[1].y - ((ho::PT<int> *)Message.pParam)[0].y;
			}

			return;
		}









		//コンストラクタ
		CheckBox::CheckBox(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, BOOL Check, TCHAR *pStr) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->Check = Check;
			this->strText = pStr;
		}

		//デストラクタ
		CheckBox::~CheckBox()
		{
		}

		//描画
		void CheckBox::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			const DWORD BorderColor = 0xffffffff;

			RectWH rcBorder = rcAbsolutePos; //正方形の矩形
			rcBorder.width = rcBorder.height; //正方形にする
			pGUIObj->DrawBorder(&rcBorder, BorderColor);

			if (Check) //チェックが入っている場合
			{
				D3DVERTEX_2D *vt = pGUIObj->Getpvt();

				D3DVERTEX_2D::SetVertexFromRECT(vt, &rcBorder.GethoRECT(-1), 0, BorderColor);
				WORD Index[] = {0, 3, 2, 1};
				pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 4, 2, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D));
			}

			int MarginY = int((rcAbsolutePos.height - pGUIObj->GetFontHeight()) * 0.5);
			if (MarginY < 0)
				MarginY = 0;
			pGUIObj->GetpDrawStringObj()->Draw(rcAbsolutePos.left + rcAbsolutePos.height + 2, rcAbsolutePos.top + MarginY, (TCHAR *)strText.c_str(), pGUIObj->GetFontHeight(), BorderColor); //文字列描画

			return;
		}

		//メッセージプロシージャ
		BOOL CheckBox::MessageProcedure(MESSAGE Message, BOOL SendChild) 
		{
			switch (Message.Message)
			{
			case MOUSE_LCLICK: //マウスクリックされた場合
				{
					BOOL NewCheck = TRUE - Check; //新しいチェック状態
					pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::CHECKBOX_SETCHECK, Message.TargetID, 4, (LPVOID)NewCheck)); //チェック状態変更のメッセージを送信
				}
				break;
			case CHECKBOX_SETCHECK: //チェックボックスのチェックを変更（4Byte、BOOL）
				this->Check = (BOOL)Message.pParam;
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}









		//コンストラクタ
		GaugeSlider::GaugeSlider(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, eHV hv, double Max, double Pos) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->Max = Max;
			this->Pos = Pos;
			Clamp(Pos, 0.0, Max);
			this->hv = hv;
		}

		//デストラクタ
		GaugeSlider::~GaugeSlider()
		{
		}

		//描画
		void GaugeSlider::Draw()
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			const DWORD BackgroundColor = 0xff7f7f7f, BarColor = 0xff0000ff, BorderColor = 0xffffffff;

			//背景の矩形を描画
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(), (float)0, BackgroundColor);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画

			//現在値の矩形を描画
			RectWH rcBar = rcAbsolutePos;
			rcBar.width = int(rcBar.width * (Pos / Max));
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcBar.GethoRECT(), (float)0, BarColor);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画

			pGUIObj->DrawBorder(&rcAbsolutePos, BorderColor); //外枠を描画


			return;
		}

		//メッセージプロシージャ
		BOOL GaugeSlider::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case MOUSE_LCLICK: //マウスの左クリック確定（8Byte、ho::PT<int>マウス座標）
				{
					double NewPos;
					if (GetPosFromMousePos(&NewPos, (ho::PT<int> *)Message.pParam)) //マウス座標よりゲージ内の値を取得し、取得できた場合
						pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::GAUGESLIDER_SETPOS, Message.TargetID, 8, (LPVOID)&NewPos)); //スライダーの値変更のメッセージを送る
				}
				break;
			case MOUSE_LDRAG_MOVE: //マウスの左ボタンでドラッグ移動（16Byte、ho::PT<int>クリック時の座標、ho::PT<int>現在のマウス座標）
				{
					double NewPos;
					if (GetPosFromMousePos(&NewPos, (ho::PT<int> *)Message.pParam + 1)) //マウス座標よりゲージ内の値を取得し、取得できた場合
						pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::GAUGESLIDER_SETPOS, Message.TargetID, 8, (LPVOID)&NewPos)); //スライダーの値変更のメッセージを送る
				}
				break;
			case GAUGESLIDER_SETPOS: //ゲージスライダーの値を変更（8Byte、double *新しい値）
				Pos = *(double *)Message.pParam;
				Clamp(Pos, 0.0, Max);
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}

		//マウス座標よりゲージ内の値を取得
		BOOL GaugeSlider::GetPosFromMousePos(double *pPos, ho::PT<int> *pptMousePos) 
		{
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			if (rcAbsolutePos.GethoRECT().JudgeContain(*pptMousePos)) //座標がスライダーの範囲内の場合
			{
				double Ratio;
				if (hv == Horizonal) //横方向の場合
				{
					Ratio = double(pptMousePos->x - rcAbsolutePos.left) / (double)rcAbsolutePos.width;
					*pPos = Ratio * this->Max;
				} else if (hv == Vertical) //縦方向の場合
				{
				} else {
					return FALSE;
				}

				return TRUE;
			}

			return FALSE;
		}







		//コンストラクタ
		Image::Image(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, ComPtr<LPDIRECT3DTEXTURE9> cpTexture) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->cpTexture = cpTexture;
		}

		//デストラクタ
		Image::~Image()
		{
		}

		//描画
		void Image::Draw()
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			IDirect3DBaseTexture9 *pOldTexture;
			pD3DDevice->GetTexture(0, &pOldTexture); 
			pD3DDevice->SetTexture(0, cpTexture);

			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(), (float)0, 0xffffffff);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //ウィンドウ描画

			pD3DDevice->SetTexture(0, pOldTexture);

			return;
		}

		//メッセージプロシージャ
		BOOL Image::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case IMG_SETTEXTURE: //イメージにテクスチャを設定（4Byte、ComPtr<LPDIRECT3DTEXTURE9>）
				this->cpTexture = *(ComPtr<LPDIRECT3DTEXTURE9> *)(Message.pParam);
				break;
			case IMG_RELEASETEXTURE: //テクスチャのComポインタを解放する
				this->cpTexture = NULL; //参照カウンタを減らす
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}
	}
}
