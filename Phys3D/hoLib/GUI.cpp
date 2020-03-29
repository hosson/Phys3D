#include "GUI.h"
#include "hoLib.h"
#include "Debug.h"
#include <algorithm>

namespace ho
{
	namespace GUI
	{
		//�R���X�g���N�^
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
			pTopElementObj = new Element(this, RectWH, 0, -1, 0); //��ԏ�̊K�w�ɂ���G�������g���쐬
		}

		//�f�X�g���N�^
		GUI::~GUI() 
		{
			SDELETE(pTopElementObj); //��ԏ�̊K�w�ɂ���G�������g���폜
		}

		//�S�ẴG�������g���폜����
		void GUI::DeleteAllElement() 
		{
			SendMsg(ho::GUI::MESSAGE(this, ho::GUI::EL_DELETE, -1, 0, NULL));

			return;
		}

		//���b�Z�[�W�������ɑ���
		void GUI::SendMsg(MESSAGE Message) 
		{
			if (pMessageProcedure) //���b�Z�[�W�v���V�[�W�����ݒ肳��Ă���ꍇ
				if ((pMessageProcedure)(pMessageProcedureObj, Message)) //���b�Z�[�W�v���V�[�W���Ń��b�Z�[�W���������ꂽ�ꍇ
					return;

			DefaultMessageProcedure(Message);

			return;
		}

		//���b�Z�[�W�����Ƃő���i���ݏ������̃��b�Z�[�W���I��������Ɓj
		void GUI::PostMsg(MESSAGE Message) 
		{
			queueMessage.push(Message);

			return;
		}

		//�f�t�H���g���b�Z�[�W�v���V�[�W��
		void GUI::DefaultMessageProcedure(MESSAGE Message) 
		{
			if (pTopElementObj)
				pTopElementObj->MessageRouting(Message);

			return;
		}

		//1�t���[�����̏���
		void GUI::FrameProcess() 
		{
			MouseInput(); //�}�E�X�Ȃǂ̓��͂�����
			ProcessMessageQue(); //SendMsg�֐��ŗ��܂������b�Z�[�W������

			Time++;

			return;
		}

		//�}�E�X�Ȃǂ̓��͂�����
		void GUI::MouseInput() 
		{
			if (!EnableUserInput) //���[�U�[���͂��󂯕t���Ȃ��ꍇ
				return;

			//�}�E�X���W�ƃN���b�N��Ԃ��擾
			int FrameFlag = pUserInputObj->GetFrameFlag();
			UserInput::MOUSESTATE *pMS = pUserInputObj->GetpMouseState();

			//���݂̃}�E�X�J�[�\���ʒu�ɑ��݂����ԏ㑤�̃G�������g��ID�𓾂�
			int PosID; //GET_TOPID_FROMPOS �œ���ꂽID
			{
				std::vector<BYTE> MsgBuf(sizeof(ho::PT<int>) + sizeof(int)); //���b�Z�[�W�p�̃o�b�t�@
				*(ho::PT<int> *)&MsgBuf.at(0) = pMS[FrameFlag].ptPos; //�o�b�t�@�ɃN���b�N���W���R�s�[
				*(int *)&MsgBuf.at(sizeof(ho::PT<int>)) = -1; //ID��-1�Ƃ��Ă���
				SendMsg(MESSAGE(this, GET_TOPID_FROMPOS, 0, MsgBuf.size(), &MsgBuf.at(0))); //�}�E�X�N���b�N�ʒu�ɂ���G�������g��ID���擾���郁�b�Z�[�W�𑗐M
				PosID = *(int *)&MsgBuf.at(sizeof(ho::PT<int>)); //GET_TOPID_FROMPOS �œ���ꂽID
			}

			if ((pMS[FrameFlag].LeftButton) && !(pMS[1 - FrameFlag].LeftButton)) //���{�^���������ꂽ�u�Ԃ̏ꍇ
			{
				if (PosID != -1) //�N���b�N�ʒu�ɉ��炩�̃G�������g�����݂����ꍇ
				{
					SendMsg(MESSAGE(this, MOUSE_LCLICK, PosID, sizeof(ho::PT<int>), &pMS[FrameFlag].ptPos)); //���N���b�N���b�Z�[�W�𑗐M

					MouseDrag.ptClickPos = pMS[FrameFlag].ptPos; //�h���b�O��Ԃ̃N���b�N�ʒu��ۑ�
					MouseDrag.DragElementID = PosID; //�N���b�N�m��ID��ۑ�
					MouseDrag.Enable = TRUE;
					SendMsg(MESSAGE(this, MOUSE_LDRAG_BEGIN, MouseDrag.DragElementID, sizeof(ho::PT<int>), &MouseDrag.ptClickPos)); //�h���b�O�J�n���b�Z�[�W�𑗐M
				}
			} else if ((pMS[FrameFlag].LeftButton) && (pMS[1 - FrameFlag].LeftButton)) //���{�^���������ꑱ���Ă���ꍇ
			{
				if (pMS[FrameFlag].ptPos != pMS[1 - FrameFlag].ptPos) //�}�E�X�J�[�\�����ړ������ꍇ
				{
					if (MouseDrag.Enable) //�h���b�O���̏ꍇ
					{
						std::vector<BYTE> MsgBuf(sizeof(ho::PT<int>) * 2); //���b�Z�[�W�p�̃o�b�t�@
						*(ho::PT<int> *)&MsgBuf.at(0) = MouseDrag.ptClickPos;
						*(ho::PT<int> *)&MsgBuf.at(sizeof(ho::PT<int>)) = pMS[FrameFlag].ptPos;
						SendMsg(MESSAGE(this, MOUSE_LDRAG_MOVE, MouseDrag.DragElementID, MsgBuf.size(), &MsgBuf.at(0))); //�h���b�O�ړ��̃��b�Z�[�W�𑗐M
					}
				}
			} else if (!(pMS[FrameFlag].LeftButton) && (pMS[1 - FrameFlag].LeftButton)) //���{�^���������ꂽ�u�Ԃ̏ꍇ
			{
				if (MouseDrag.Enable) //�h���b�O���̏ꍇ
				{
					std::vector<BYTE> MsgBuf(sizeof(ho::PT<int>) * 2); //���b�Z�[�W�p�̃o�b�t�@
					*(ho::PT<int> *)&MsgBuf.at(0) = MouseDrag.ptClickPos;
					*(ho::PT<int> *)&MsgBuf.at(sizeof(ho::PT<int>)) = pMS[FrameFlag].ptPos;
					SendMsg(MESSAGE(this, MOUSE_LDRAG_END, MouseDrag.DragElementID, MsgBuf.size(), &MsgBuf.at(0))); //�h���b�O�I���̃��b�Z�[�W�𑗐M
					MouseDrag.Enable = FALSE; //�h���b�O�I��
				}
			}

			//�z�o�[�Ɋւ��鏈��
			if (PosID != -1) //�}�E�X�|�C���^�����炩�̃G�������g��ɂ���ꍇ
			{
				if (MouseHover.Enable) //�z�o�[���̏ꍇ
				{
					if (MouseHover.HoverElementID != PosID) //�z�o�[�Ώۂ��ʂ̃G�������g��Ɉڂ��Ă����ꍇ
					{
						SendMsg(MESSAGE(this, MOUSE_HOVER_END, MouseHover.HoverElementID, 0, NULL)); //�z�o�[�I�����b�Z�[�W�𑗐M
					}
				} else { //�z�o�[���Ă��Ȃ��ꍇ
					MouseHover.Enable = TRUE;
				}
				SendMsg(MESSAGE(this, MOUSE_HOVER_BEGIN, PosID, 0, NULL)); //�z�o�[�J�n���b�Z�[�W�𑗐M
				SendMsg(MESSAGE(this, MOUSE_HOVER_MOVE, MouseHover.HoverElementID, 8, &pMS[FrameFlag].ptPos)); //�z�o�[�J�n�ʒu��m�点�邽�߂Ƀ��b�Z�[�W�𑗐M
				MouseHover.HoverElementID = PosID;
			} else { //�}�E�X�|�C���^���G�������g��ɂȂ��ꍇ
				MouseHover.Enable = FALSE;
			}
			if ((MouseHover.Enable) && (pMS[FrameFlag].ptPos != pMS[1 - FrameFlag].ptPos)) //�z�o�[���Ƀ}�E�X�J�[�\�����ړ����Ă����ꍇ
				SendMsg(MESSAGE(this, MOUSE_HOVER_MOVE, MouseHover.HoverElementID, 8, &pMS[FrameFlag].ptPos)); //�}�E�X�z�o�[���Ɉړ��������b�Z�[�W�𑗐M

			return;
		}

		//SnedMsg�֐��ŗ��܂������b�Z�[�W������
		void GUI::ProcessMessageQue() 
		{
			while (!queueMessage.empty()) //�L���[�̒��g����ɂȂ�܂ŌJ��Ԃ�
			{
				SendMsg(queueMessage.front()); //�擪�̃��b�Z�[�W������
				queueMessage.pop(); //�擪�̃��b�Z�[�W���폜
			}

			return;
		}

		//�`��
		void GUI::Draw() 
		{
			DWORD OldFVF;
			pD3DDevice->GetFVF(&OldFVF);
			pD3DDevice->SetFVF(D3DVERTEX_2D::FVF);

			SendMsg(MESSAGE(this, DRAW, -1, 0, NULL)); //�`��̍ċA���b�Z�[�W�𑗂�

			pD3DDevice->SetFVF(OldFVF);

			return;
		}

		//�E�B���h�E�쐬
		BOOL GUI::CreateWnd(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption, BOOL Show, eWindowStyle::e WindowStyle)
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //�e�ƂȂ�G�������g�𓾂�
			if (!pParentElementObj) //�e��������Ȃ������ꍇ
				return FALSE;

			Element *pElementObj = new Window(this, rcPosition, ID, ParentID, z, pCaption, Show, WindowStyle); //�E�B���h�E�G�������g���쐬
			pParentElementObj->AddChildElementObj(pElementObj); //�e�G�������g�ɍ쐬�����G�������g��ǉ�

			return TRUE;
		}

		//�{�^���쐬
		BOOL GUI::CreateButton(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption)
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //�e�ƂȂ�G�������g�𓾂�
			if (!pParentElementObj) //�e��������Ȃ������ꍇ
				return FALSE;

			Element *pElementObj = new Button(this, rcPosition, ID, ParentID, z, pCaption); //�{�^���G�������g���쐬
			pParentElementObj->AddChildElementObj(pElementObj); //�e�G�������g�ɍ쐬�����G�������g��ǉ�

			return TRUE;
		}

		//���x���쐬
		BOOL GUI::CreateLabel(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pStr) 
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //�e�ƂȂ�G�������g�𓾂�
			if (!pParentElementObj) //�e��������Ȃ������ꍇ
				return FALSE;

			Element *pElementObj = new Label(this, rcPosition, ID, ParentID, z, pStr); //���x���G�������g���쐬
			pParentElementObj->AddChildElementObj(pElementObj); //�e�G�������g�ɍ쐬�����G�������g��ǉ�

			return TRUE;
		}

		//�R���{�{�b�N�X�쐬
		BOOL GUI::CreateComboBox(ho::RectWH rcPosition, int ID, int ParentID, int z) 
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //�e�ƂȂ�G�������g�𓾂�
			if (!pParentElementObj) //�e��������Ȃ������ꍇ
				return FALSE;

			Element *pElementObj = new ComboBox(this, rcPosition, ID, ParentID, z); //�R���{�{�b�N�X�G�������g���쐬
			pParentElementObj->AddChildElementObj(pElementObj); //�e�G�������g�ɍ쐬�����G�������g��ǉ�

			return TRUE;
		}

		//�`�F�b�N�{�b�N�X�쐬
		BOOL GUI::CreateCheckBox(ho::RectWH rcPosition, int ID, int ParentID, int z, BOOL Check, TCHAR *pStr)
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //�e�ƂȂ�G�������g�𓾂�
			if (!pParentElementObj) //�e��������Ȃ������ꍇ
				return FALSE;

			Element *pElementObj = new CheckBox(this, rcPosition, ID, ParentID, z, Check, pStr); //�`�F�b�N�{�b�N�X�G�������g���쐬
			pParentElementObj->AddChildElementObj(pElementObj); //�e�G�������g�ɍ쐬�����G�������g��ǉ�

			return TRUE;
		}

		//�Q�[�W�X���C�_�[�쐬
		BOOL GUI::CreateGaugeSlider(ho::RectWH rcPosition, int ID, int ParentID, int z, eHV hv, double Max, double Pos)
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //�e�ƂȂ�G�������g�𓾂�
			if (!pParentElementObj) //�e��������Ȃ������ꍇ
				return FALSE;

			Element *pElementObj = new GaugeSlider(this, rcPosition, ID, ParentID, z, hv, Max, Pos); //�Q�[�W�X���C�_�[�G�������g���쐬
			pParentElementObj->AddChildElementObj(pElementObj); //�e�G�������g�ɍ쐬�����G�������g��ǉ�

			return TRUE;
		}

		//�C���[�W�쐬
		BOOL GUI::CreateImage(ho::RectWH rcPosition, int ID, int ParentID, int z, ComPtr<LPDIRECT3DTEXTURE9> cpTexture) 
		{
			Element *pParentElementObj = GetpElementObj(ParentID); //�e�ƂȂ�G�������g�𓾂�
			if (!pParentElementObj) //�e��������Ȃ������ꍇ
				return FALSE;

			Element *pElementObj = new Image(this, rcPosition, ID, ParentID, z, cpTexture); //�Q�[�W�X���C�_�[�G�������g���쐬
			pParentElementObj->AddChildElementObj(pElementObj); //�e�G�������g�ɍ쐬�����G�������g��ǉ�

			return TRUE;
		}

		//ID����G�������g���擾
		Element *GUI::GetpElementObj(int ID) 
		{
			Element *pElementObj = NULL;

			SendMsg(MESSAGE(this, GET_PTR, ID, 4, &pElementObj)); //�f�[�^�A�h���X�փG�������g�I�u�W�F�N�g�̃A�h���X���i�[���郁�b�Z�[�W�𑗂�

			return pElementObj;
		}

		//�o�^�\�ȃV�X�e���G�������gID���擾
		int GUI::GetNullSystemElementID() 
		{
			for (int i = 0; i < signed(vectorSystemElementID.size()); i++)
				if (!vectorSystemElementID.at(i)) //�z����ɋ󂫂�����ꍇ
					return 0x40000000 + i; //ID��0x40000000���琔�����l�ɕϊ����ĕԂ�

			//�z�����1���󂫂��Ȃ��ꍇ
			vectorSystemElementID.push_back(NULL); //�z��̍Ō�Ɋi�[
			return 0x40000000 + (signed)vectorSystemElementID.size(); //ID��Ԃ�
		}

		//�쐬�����V�X�e���G�������g��o�^���AID���Ԃ�
		int GUI::RegistSystemElementObj(Element *pElementObj) 
		{
			for (int i = 0; i < (signed)vectorSystemElementID.size(); i++)
			{
				if (!vectorSystemElementID.at(i)) //�z����ɋ󂫂�����ꍇ
				{
					vectorSystemElementID.at(i) = pElementObj; //�G�������g�I�u�W�F�N�g�ւ̃|�C���^���i�[
					return 0x40000000 + i; //ID�𕉂̒l�ɕϊ����ĕԂ�
				}
			}

			//�z�����1���󂫂��Ȃ��ꍇ
			vectorSystemElementID.push_back(pElementObj); //�z��̍Ō�Ɋi�[
			return -(signed)vectorSystemElementID.size(); //ID��Ԃ�
		}

		//�w�肳�ꂽID�̃V�X�e���G�������g���Ǘ����X�g����폜����
		void GUI::DeleteSystemElementObj(int ID) 
		{
			if (ID >= 0x40000000 && (signed)vectorSystemElementID.size() > ID - 0x40000000) //ID���z��T�C�Y���̏ꍇ
				vectorSystemElementID.at(ID - 0x40000000) = NULL;

			return;
		}

		//�O�p�`��3���_�̍��W���v�Z����
		void GUI::CalcTriangleVertex(ho::D3DVERTEX_2D *pVertex, ho::PT<int> ptCenter, int Radius, ho::Direction8 Angle) 
		{
			double Angle2[3];
			Angle2[0] = atan2((double)Angle.GetXY().y, (double)Angle.GetXY().x); //�O�p�`�̌����̊p�x�����߂�
			Angle2[1] = Angle2[0] + PI * 2.0 / 3.0;
			Angle2[2] = Angle2[0] + PI * 4.0 / 3.0;

			for (int i = 0; i < 3; i++)
			{
				pVertex[i].x = float(ptCenter.x + cos(Angle2[i]) * Radius);
				pVertex[i].y = float(ptCenter.y + sin(Angle2[i]) * Radius);
			}

			return;
		}

		//��`�̘g��`�悷��
		void GUI::DrawBorder(RectWH *pRectWH, DWORD Color) 
		{
			D3DVERTEX_2D vt[4];

			D3DVERTEX_2D::SetVertexFromRECT(vt, &pRectWH->GethoRECT(-1), 0, Color);
			WORD Index[] = {0, 1, 3, 2, 0};
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, 0, 4, 4, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D));

			return;
		}









		//�R���X�g���N�^
		Element::Element(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, Element *pParentElementObj) 
		{
			this->pGUIObj = pGUIObj;
			this->rcPosition = rcPosition;
			this->MinSize = ho::PT<int>(32, 32); //�ŏ��T�C�Y�i�b��I�ɂ����Ŏw��j
			this->ID = ID;
			this->ParentID = ParentID;
			this->z = z;
			this->bShow = TRUE;
			this->rcClientOffset = ho::RECT(0, 0, 0, 0);
			this->Hover = FALSE;

			this->pParentElementObj = pGUIObj->GetpElementObj(ParentID); //�e�G�������g�̃|�C���^���擾
			if (!this->pParentElementObj && pParentElementObj) //�e�G�������g���擾�ł����A�����ɐe�G�������g�ւ̃|�C���^���w�肳��Ă����ꍇ
				this->pParentElementObj = pParentElementObj;
		}

		//�f�X�g���N�^
		Element::~Element()
		{
			//���ׂĂ̎q���G�������g���폜
			for (std::list<Element *>::iterator itr = listpChildElementObj.begin(); itr != listpChildElementObj.end(); itr++)
				SDELETE(*itr);
		}

		//���b�Z�[�W�̌o�H�I��
		void Element::MessageRouting(MESSAGE Message) 
		{
			BOOL ProcessMessage; //���b�Z�[�W���������邩�ǂ���
			BOOL SendChild; //�q���G�������g�ɑ��邩�ǂ���

			//���b�Z�[�W�z�B����
			if (Message.TargetID == -1) //�ċA�����őS�̂ɑ���ꍇ
			{
				ProcessMessage = TRUE;
				SendChild = TRUE;
			} else { //���悪���܂��Ă���ꍇ
				if (ID != Message.TargetID) //���݂̃G�������g������ł͂Ȃ������ꍇ
				{
					ProcessMessage = FALSE;
					SendChild = TRUE;
				} else { //���݂̃G�������g�����悾�����ꍇ
					ProcessMessage = TRUE;
					SendChild = FALSE;
				}
			}

			if (ProcessMessage) //���b�Z�[�W�̏���������ꍇ
				SendChild = MessageProcedure(Message, SendChild); //���b�Z�[�W�����v���V�[�W��

			if (SendChild) //�q���G�������g�ɂ�����ꍇ
			{
				std::list<Element *> listTemp = listpChildElementObj; //�ꎞ���X�g�ɃR�s�[�i��̃��b�Z�[�W�������ɂ��̃��X�g�̓��e���ύX�����ꍇ�����邽�߁j
				for (std::list<Element *>::iterator itr = listTemp.begin(); itr != listTemp.end(); itr++) //�q�����X�g�𑖍�
					(*itr)->MessageRouting(Message);
			}

			return;
		}

		//���b�Z�[�W�����v���V�[�W��
		BOOL Element::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			return DefaultMessageProcedure(Message, SendChild);
		}

		//�f�t�H���g���b�Z�[�W�����v���V�[�W��
		BOOL Element::DefaultMessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case DRAW: //�`�惁�b�Z�[�W
				if (bShow) //�\������ꍇ
					Draw(); //�`��
				else
					SendChild = FALSE; //�q���I�u�W�F�N�g�ɂ�����Ȃ�
				break;
			case GET_PTR: //�G�������g�I�u�W�F�N�g�̃|�C���^���擾
				*(void **)Message.pParam = this;
				break;
			case SETPOS_CENTER: //�N���C�A���g�̈�̒����Ɉړ�
				SetPos_Center(Message);
				break;
			case GET_TOPID_FROMPOS:	//�C�ӂ̍��W��̈�ԏ㑤�̃G�������g��ID���擾�i12Byte�Aho::PT<int>�C�Ӎ��W�Aint *ID���i�[����|�C���^�j
				GetTopIDFromPos(Message);
				break;
			case SHOW: //�\���A��\���̐؂�ւ��i4Byte�ABOOL�j
				Show(Message);
				break;
			case SHOW_SWITCH: //�\���̔�\�� �����݂ɋC�s�ς���
				pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::SHOW, Message.TargetID, 4, LPVOID(TRUE - bShow))); //���b�Z�[�W���M 
				break;
			case GET_SHOW: //�\���A��\���̏�Ԃ��擾����i4Byte�A *BOOL ���ʂ𓾂�|�C���^�j
				*(BOOL *)Message.pParam = this->bShow;
				break;
			case SETTEXT: //�E�B���h�E�⃉�x�����̕������ݒ�i4Byte�ATCHAR *�j
				SetText(Message);
				break;
			case MOUSE_HOVER_BEGIN: //�z�o�[�J�n
				Hover = TRUE;
				break;
			case MOUSE_HOVER_MOVE: //�z�o�[�ړ�
				{
					HANDLE hCursor = NULL;
					hCursor = LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, NULL, NULL, LR_SHARED); 
					if (hCursor) //�J�[�\�������[�h����Ă����ꍇ
						SetCursor((HCURSOR)hCursor);//�J�[�\����ݒ�
				}
				break;
			case MOUSE_HOVER_END: //�z�o�[�I��
				Hover = FALSE;
				break;
			case POSITION: //�G�������g�̈ʒu���ircPosition�j��ύX����i16Byte�ARectWH *�V�����ʒu���j
				rcPosition = *(RectWH *)Message.pParam;
				if (rcPosition.width < MinSize.x)
					rcPosition.width = MinSize.x;
				if (rcPosition.height < MinSize.y)
					rcPosition.height = MinSize.y;
				break;
			case MOVE: //�G�������g�̍�����W��ύX����i8Byte�Aho::PT<int> *�V�������W�j
				rcPosition.left = ((ho::PT<int> *)Message.pParam)->x;
				rcPosition.top = ((ho::PT<int> *)Message.pParam)->y;
				break;
			case GET_CLIENT_RECT: //�G�������g�̃N���C�A���g�̈�̐�Έʒu���擾�i4Byte�ARectWH *�N���C�A���g�̈���������ރ|�C���^�j
				*(RectWH *)Message.pParam = GetAbsoluteClientPos();
				break;
			case SETFOREGROUND: //�e�G�������g�̃��X�g��ň�ԍŌ�i��ʏ�ł͏㑤�j�ɕ��ёւ���
				if (pParentElementObj) //�e�G�������g�����݂���ꍇ
					pParentElementObj->SetForegroundChildID(this->ID); //�e�G�������g���Ō��݂̃G�������g���őO�ʂɕ��ёւ���
				break;
			}

			return SendChild;
		}

		//�q���G�������g��ǉ�
		void Element::AddChildElementObj(Element *pElementObj) 
		{
			listpChildElementObj.push_back(pElementObj);

			return;
		}

		//��Έʒu���擾
		RectWH Element::GetAbsolutePos()
		{
			if (pParentElementObj) //�e�G�������g�����݂���ꍇ
				return pParentElementObj->GetAbsolutePosAlg(RectWH(rcPosition.left, rcPosition.top, rcPosition.width, rcPosition.height), 0); //�����ċA�֐������s

			return rcPosition;
		}

		//��Έʒu���擾�i�����ċA�֐��j
		RectWH Element::GetAbsolutePosAlg(RectWH Rect, int Level) 
		{
			Rect.left += this->rcPosition.left + rcClientOffset.left;
			Rect.top += this->rcPosition.top + rcClientOffset.top;

			if (pParentElementObj) //�e�G�������g�����݂���ꍇ
				Rect = pParentElementObj->GetAbsolutePosAlg(Rect, Level + 1);

			return Rect;
		}

		//�N���C�A���g�̈�݂̂̐�Έʒu���擾
		RectWH Element::GetAbsoluteClientPos() 
		{
			return GetAbsoluteClientPosAlg(RectWH(0, 0, rcPosition.width - rcClientOffset.left - rcClientOffset.right,
				rcPosition.height - rcClientOffset.top - rcClientOffset.bottom), 0); //�����ċA�֐������s
		}

		//�N���C�A���g�̈�݂̂̐�Έʒu���擾�i�����ċA�֐��j
		RectWH Element::GetAbsoluteClientPosAlg(RectWH Rect, int Level) 
		{
			Rect.left += this->rcPosition.left + rcClientOffset.left;
			Rect.top += this->rcPosition.top + rcClientOffset.top;

			if (pParentElementObj) //�e�G�������g�����݂���ꍇ
				Rect = pParentElementObj->GetAbsoluteClientPosAlg(Rect, Level + 1);

			return Rect;
		}

		//GET_TOPID_FROMPOS�i�C�ӂ̍��W��̈�ԏ㑤�̃G�������g��ID���擾�j
		void Element::GetTopIDFromPos(MESSAGE Message) 
		{
			ho::PT<int> ptClickPos = *(ho::PT<int> *)Message.pParam; //���b�Z�[�W����}�E�X�N���b�N�ʒu���擾
	
			//�q���G�������g�𑖍����ăN���b�N���W���܂܂��ꍇ�̓��b�Z�[�W��]������
			if (listpChildElementObj.size() >= 1) //�q���G�������g�����݂���ꍇ
			{
				//��O�ɂ���G�������g�����X�g�̍Ō�ɂ���̂ŁA�傫��������i�t��������j��������
				std::list<Element *>::iterator itr = listpChildElementObj.end();
				do
				{
					itr--;
					if ((*itr)->GetAbsolutePos().GethoRECT(-1).JudgeContain(ptClickPos) && (*itr)->GetbShow()) //�G�������g�͈͓̔��ł��\�����̏ꍇ
					{
						(*itr)->MessageRouting(MESSAGE(Message.pGUIObj, Message.Message, (*itr)->GetID(), Message.Bytes, Message.pParam)); //�G�������g�ɃN���b�N���b�Z�[�W�𑗂�
						return;
					}
				} while (itr != listpChildElementObj.begin());
			}

			//���W�Ɏq���G�������g���܂܂�Ă��Ȃ������ꍇ
			*(int *)((BYTE *)Message.pParam + sizeof(ho::PT<int>)) = this->ID; //���b�Z�[�W�̃o�b�t�@�ɃN���b�N�m�肵���G�������g��ID���i�[

			return;
		}

		//�N���C�A���g�̈�̒����Ɉړ�
		void Element::SetPos_Center(MESSAGE Message) 
		{
			if (!pParentElementObj) //�e�G�������g�����݂��Ȃ��ꍇ
				return;

			ho::RectWH rcParentPos = pParentElementObj->GetrcPosition(); //�e�I�u�W�F�N�g�̈ʒu���𓾂�
			this->rcPosition.left = int(rcParentPos.width * 0.5 - this->rcPosition.width * 0.5);
			this->rcPosition.top = int(rcParentPos.height * 0.5 - this->rcPosition.height * 0.5);

			return;
		}

		//�\���A��\���̐؂�ւ��i4Byte�ABOOL�j
		void Element::Show(MESSAGE Message) 
		{
			if ((BOOL)Message.pParam)
				bShow = TRUE;
			else if (!(BOOL)Message.pParam)
				bShow = FALSE;

			return;
		}

		//��΍��W����G�������g���̑��΍��W�ɕϊ�
		ho::PT<int> Element::GetLocalPtFromAbsolutePt(ho::PT<int> ptPos) 
		{
			RectWH rcAbsolutePos = this->GetAbsolutePos(); //�G�������g�̐�΍��W���擾

			return ho::PT<int>(ptPos.x - rcAbsolutePos.left, ptPos.y - rcAbsolutePos.top); //���W�𑊑΍��W�ɕϊ�

		}

		//�C�ӂ�ID�̎q���I�u�W�F�N�g���őO�ʂɕ��ёւ���
		void Element::SetForegroundChildID(int ID) 
		{
			Element *pElementObj = NULL;

			//������ID�����G�������g�����X�g���甭�����č폜
			for (std::list<Element *>::iterator itr = listpChildElementObj.begin(); itr != listpChildElementObj.end(); itr++)
			{
				if ((*itr)->GetID() == ID)
				{
					pElementObj = *itr;
					listpChildElementObj.erase(itr);
					break;
				}
			}

			if (pElementObj) //���X�g���甭������Ă����ꍇ
				listpChildElementObj.push_back(pElementObj); //���X�g�̍Ō�ɒǉ�

			return;
		}








		//�R���X�g���N�^
		Window::Window(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption, BOOL Show, eWindowStyle::e WindowStyle) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->BorderWidth = 2; //�b��I��2�Ƃ��Ă���
			strCaption = pCaption;
			this->bShow = Show;
			this->WindowStyle = WindowStyle;
			this->rcClientOffset = ho::RECT(BorderWidth, pGUIObj->GetFontHeight() + 8, BorderWidth, BorderWidth);
		}

		//�f�X�g���N�^
		Window::~Window()
		{
		}

		//�`��
		void Window::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			//�E�B���h�E�S��
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(), 0, 0x3f000000);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��

			//�^�C�g���o�[
			RectWH rcTitleBar = rcAbsolutePos;
			rcTitleBar.left += BorderWidth;
			rcTitleBar.top += BorderWidth;
			rcTitleBar.width -= BorderWidth * 2;
			rcTitleBar.height = pGUIObj->GetFontHeight() + 4;
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcTitleBar.GethoRECT(), 0, 0x3f000000);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��

			pGUIObj->GetpDrawStringObj()->Draw(rcTitleBar.left + 2, rcTitleBar.top + 2, (TCHAR *)strCaption.c_str(), pGUIObj->GetFontHeight(), 0xffffffff); //������`��

			return;
		}

		//�������ݒ�
		void Window::SetText(MESSAGE Message) 
		{
			if (Message.pParam) //������̃A�h���X�����݂��Ă���ꍇ
				strCaption = (TCHAR *)Message.pParam;
			else //�A�h���X��null�̏ꍇ
				strCaption.clear(); //�����������

			return;
		}

		//���b�Z�[�W�����v���V�[�W��
		BOOL Window::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case MOUSE_LDRAG_BEGIN: //�}�E�X�̍��{�^���Ńh���b�O�J�n�i8Byte�Aho::PT<int>�N���b�N���̍��W�j
				this->rcDragBeginPos = rcPosition; //�h���b�O�J�n���̈ʒu��ۑ�
				this->DragBeginRegion = GetRegion(&GetLocalPtFromAbsolutePt(*(ho::PT<int> *)Message.pParam)); //�h���b�O�J�n���̗̈��ۑ�
				break;
			case MOUSE_LDRAG_MOVE: //�}�E�X�̍��{�^���Ńh���b�O�ړ��i16Byte�Aho::PT<int>�N���b�N���̍��W�Aho::PT<int>���݂̃}�E�X���W�j
				Mouse_LDrag_Move(Message); //�}�E�X���{�^���Ńh���b�O�ړ����̏���
				break;
			case MOUSE_HOVER_MOVE: //�}�E�X�z�o�[���Ɉړ��i8Byte�Aho::PT<int> *�ړ����̃}�E�X���W�j
				MouseHover((ho::PT<int> *)Message.pParam); //�}�E�X�z�o�[���̃J�[�\���A�C�R���ύX����
				break;
			default:
				SendChild = DefaultMessageProcedure(Message, SendChild); //���̃v���V�[�W���ŏ�������Ȃ��������b�Z�[�W�̓f�t�H���g�v���V�[�W���֑�����
				break;
			}

			return SendChild;
		}

		//�}�E�X���{�^���Ńh���b�O�ړ����̏���
		void Window::Mouse_LDrag_Move(MESSAGE Message) 
		{
			ho::PT<int> ptSub = ((ho::PT<int> *)Message.pParam)[1] - ((ho::PT<int> *)Message.pParam)[0]; //�N���b�N�����猻�݈ʒu�܂ł̈ړ���
			RectWH rcNewPosition = rcPosition; //�V�����ʒu���

			switch (DragBeginRegion) //�h���b�O�J�n���̃}�E�X���W�̗̈�ɂ���ĕ���
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
			case TitleBar: //�^�C�g���o�[
				rcNewPosition.left = rcDragBeginPos.left + ptSub.x;
				rcNewPosition.top = rcDragBeginPos.top + ptSub.y;
				break;
			}

			if (rcPosition != rcNewPosition) //�ʒu��񂪕ύX�����ꍇ
				pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::POSITION, Message.TargetID, 16, &rcNewPosition)); //�ʒu���ύX�̃��b�Z�[�W�𑗐M

			return;
		}

		//�G�������g���̍��W����̈���擾
		Window::eRegion Window::GetRegion(ho::PT<int> *pptLocalPos) 
		{
			const int CornerSize = 24; //�p�̑傫��

			if (pptLocalPos->x < BorderWidth) //�����̘g���̏ꍇ
			{
				if (pptLocalPos->y < CornerSize)
					return LeftTop;
				else if (pptLocalPos->y >= rcPosition.height - CornerSize)
					return LeftBottom;
				return Left;
			} else if (pptLocalPos->x >= rcPosition.width - BorderWidth) //�E���̘g���̏ꍇ
			{
				if (pptLocalPos->y < CornerSize)
					return RightTop;
				else if (pptLocalPos->y >= rcPosition.height - CornerSize)
					return RightBottom;
				return Right;
			} else if (pptLocalPos->y < BorderWidth) //�㑤�̘g���̏ꍇ
			{
				if (pptLocalPos->x < CornerSize)
					return LeftTop;
				else if (pptLocalPos->x >= rcPosition.width - CornerSize)
					return RightTop;
				return Top;
			} else if (pptLocalPos->y >= rcPosition.height - BorderWidth) //�����̘g���̏ꍇ
			{
				if (pptLocalPos->x < CornerSize)
					return LeftBottom;
				else if (pptLocalPos->x >= rcPosition.width - CornerSize)
					return RightBottom;
				return Bottom;
			} else if (pptLocalPos->y < BorderWidth + pGUIObj->GetFontHeight() + 4) //�^�C�g���o�[�̏ꍇ
			{
				return TitleBar;
			}

			return Client; //�N���C�A���g�̈�
		}

		//�}�E�X�z�o�[���̃J�[�\���A�C�R���ύX����
		void Window::MouseHover(ho::PT<int> *ptMousePos) 
		{
			ho::PT<int> ptLocalMousePos = GetLocalPtFromAbsolutePt(*ptMousePos); //�}�E�X���W�����[�J�����W�ɕϊ�

			eRegion Region = GetRegion(&ptLocalMousePos); //�}�E�X���W����̈���擾
			HANDLE hCursor = NULL;

			switch (Region) //�擾�����̈�ɂ���ĕ���
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

			if (hCursor) //�J�[�\�������[�h����Ă����ꍇ
				SetCursor((HCURSOR)hCursor);

			return;
		}













		//�R���X�g���N�^
		Button::Button(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			strCaption = pCaption;
			this->Enable = TRUE;
		}

		//�f�X�g���N�^
		Button::~Button()
		{
		}

		//�`��
		void Button::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			//�z�o�[��Ԃɂ���ĐF��ύX
			DWORD Color; //�{�^���̐F
			if (Hover)
			{
				Color = 0x7f0000ff;
			} else {
				//�H�ɓ_��
				DWORD ModTime = pGUIObj->GetTime() % 180;
				if (ModTime < 10)
					Color = 0x7f000000 | ((0x7f - ModTime * 12) << 16) | ((0x7f - ModTime * 12) << 8) | (0x7f + ModTime * 12);
				else if (ModTime < 20)
					Color = 0x7f000000 | ((0x7f - (20 - ModTime) * 12) << 16) | ((0x7f - (20 - ModTime) * 12) << 8) | (0x7f + (20 - ModTime) * 12);
				else
					Color = 0x7f7f7f7f;
			}

			//�{�^���S��
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(), (float)0, Color);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��

			pGUIObj->GetpDrawStringObj()->Draw(rcAbsolutePos.left + 2, rcAbsolutePos.top + 2, (TCHAR *)strCaption.c_str(), pGUIObj->GetFontHeight(), 0xffffffff); //������`��

			return;
		}
















		//�R���X�g���N�^
		Label::Label(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pStr) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			strLabel = pStr;
		}

		//�f�X�g���N�^
		Label::~Label()
		{
		}

		//�`��
		void Label::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();
			//rcAbsolutePos.top += pGUIObj->GetFontHeight() + 8; //�^�C�g���o�[�������ɉ�����

			ho::PT<int> ptDrawPos(rcAbsolutePos.left, rcAbsolutePos.top); //�����`��ʒu
			TCHAR *pStr = (TCHAR *)strLabel.c_str(); //�`�悷�镶����̃|�C���^
			DWORD DrawedCharNum = 0; //�`�悵��������

			while (DrawedCharNum < strLabel.size()) //�S���̕����̕`�悪�I���܂ŌJ��Ԃ�
			{
				DrawedCharNum += pGUIObj->GetpDrawStringObj()->DrawWidth(ptDrawPos.x, ptDrawPos.y, pStr + DrawedCharNum, rcPosition.width, 16); //�������t���ŕ�����`��
				ptDrawPos.y += 20;
			}

			return;
		}

		//�������ݒ�
		void Label::SetText(MESSAGE Message) 
		{
			if (Message.pParam) //������̃A�h���X�����݂��Ă���ꍇ
				strLabel = (TCHAR *)Message.pParam;
			else //�A�h���X��null�̏ꍇ
				strLabel.clear(); //�����������

			return;
		}














		//�R���X�g���N�^
		ComboBox::ComboBox(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->ScrollBarSize = 16; //�X�N���[���o�[�̃T�C�Y

			this->rcOpenPosition = rcPosition;
			this->rcPosition.height = pGUIObj->GetFontHeight() + 4;
			SelectIndex = -1;
			ListOpen = FALSE;
			this->HoverPos = -2;

			this->pScrollBarObj = new ScrollBar(pGUIObj, ho::RectWH(rcOpenPosition.width - ScrollBarSize, pGUIObj->GetFontHeight() + 4, ScrollBarSize, rcOpenPosition.height - (pGUIObj->GetFontHeight() + 4)), pGUIObj->GetNullSystemElementID(), ID, z, this, ScrollBar::Vertical, 7, 0, 7); //�c�����X�N���[���o�[���쐬
			pGUIObj->RegistSystemElementObj(pScrollBarObj); //�X�N���[���o�[���V�X�e���G�������g�Ƃ��ēo�^
			AddChildElementObj(pScrollBarObj); //�q���G�������g�Ƃ��ăR���{�{�b�N�X�G�������g�ɓo�^
			pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SHOW, pScrollBarObj->GetID(), 4, (LPVOID)FALSE)); //��\���ɂ��郁�b�Z�[�W�𑗐M
		}

		//�f�X�g���N�^
		ComboBox::~ComboBox()
		{
			pGUIObj->DeleteSystemElementObj(pScrollBarObj->GetID()); //�V�X�e���G�������g���폜
		}

		//�`��
		void ComboBox::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			DWORD BoxColor = 0x7f7f7f7f; //�㕔�{�b�N�X�̐F
			if (HoverPos == -1) //�㕔�{�b�N�X���z�o�[���̏ꍇ
				BoxColor = 0x7f0000ff;

			//�㕔�̃{�b�N�X
			RectWH rcBox = rcAbsolutePos;
			rcBox.height = pGUIObj->GetFontHeight() + 4;
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcBox.GethoRECT(), 0, BoxColor);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��
			//�{�b�N�X���̎O�p
			DWORD ColorTriangle = 0xffffffff;
			ho::D3DVERTEX_2D vtTriangle[3];
			for (int i = 0; i < 3; i++)
				vtTriangle[i].SetVertex(0, 0, 0, ColorTriangle, 0, 0); //�F�Ȃǂ�ݒ�
			pGUIObj->CalcTriangleVertex(vtTriangle, ho::PT<int>(rcAbsolutePos.left + rcAbsolutePos.width - int(pGUIObj->GetFontHeight() * 0.5f) - 2, rcAbsolutePos.top + int(pGUIObj->GetFontHeight() * 0.5f) + 2), int(pGUIObj->GetFontHeight() * 0.4f), ho::Direction8(ho::Direction8::Down)); //�O�p�`�̒��_���W���v�Z
			D3DVERTEX_2D::Offset(vtTriangle, 3);
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vtTriangle, sizeof(D3DVERTEX_2D)); //�O�p�`�`��
			//�{�b�N�X���̕�����
			int i = 0;
			for (std::list<tstring>::iterator itr = listStr.begin(); itr != listStr.end(); itr++) //�����񃊃X�g�𑖍�
			{
				if (i == SelectIndex)
				{
					pGUIObj->GetpDrawStringObj()->Draw(rcAbsolutePos.left + 2, rcAbsolutePos.top + 2, (TCHAR *)itr->c_str(), pGUIObj->GetFontHeight(), 0xffffffff); //������`��
					break;
				}
				i++;
			}

			if (ListOpen) //���X�g���J���Ă���ꍇ
			{
				const DWORD ListColor = 0x7f7f7f7f;
				const DWORD ListHoverColor = 0x7f0000ff;
				int Lines = GetListViewLines(); //�\���\�ȍs��

				//���X�g�̔w�i�̐F��`��
				int Y = rcAbsolutePos.top + pGUIObj->GetFontHeight() + 4; //���X�g�v�f��Y���W
				if (HoverPos >= 0) //���X�g�����z�o�[���̏ꍇ
				{
					RectWH rcList = rcAbsolutePos;
					rcList.width -= ScrollBarSize;
					for (int i = 0; i < Lines;) //�s�����𑖍�
					{
						if (HoverPos == i) //���݂̍s���z�o�[���̏ꍇ
						{
							rcList.top = Y + (pGUIObj->GetFontHeight() + 4) * i;
							rcList.height = pGUIObj->GetFontHeight() + 4;
							D3DVERTEX_2D::SetVertexFromRECT(vt, &rcList.GethoRECT(), 0, ListHoverColor);
							D3DVERTEX_2D::Offset(vt, 4);
							pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��
							i++;
						} else { //�z�o�[���ł͂Ȃ��ꍇ
							if (i < HoverPos) //�z�o�[�ʒu����̏ꍇ
							{
								rcList.top = Y + (pGUIObj->GetFontHeight() + 4) * i;
								rcList.height = (pGUIObj->GetFontHeight() + 4) * (HoverPos - i);
								D3DVERTEX_2D::SetVertexFromRECT(vt, &rcList.GethoRECT(), 0, ListColor);
								D3DVERTEX_2D::Offset(vt, 4);
								pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��
								i = HoverPos;
							} else { //�z�o�[�ʒu��艺�̏ꍇ
								rcList.top = Y + (pGUIObj->GetFontHeight() + 4) * i;
								rcList.height = (pGUIObj->GetFontHeight() + 4) * (Lines - i);
								D3DVERTEX_2D::SetVertexFromRECT(vt, &rcList.GethoRECT(), 0, ListColor);
								D3DVERTEX_2D::Offset(vt, 4);
								pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��
								i = Lines;
							}
						}
					}
				} else { //���X�g���Ńz�o�[���Ă��Ȃ��ꍇ
					RectWH rcList = rcAbsolutePos;
					rcList.top += pGUIObj->GetFontHeight() + 4;
					rcList.height -= pGUIObj->GetFontHeight() + 4;
					D3DVERTEX_2D::SetVertexFromRECT(vt, &rcList.GethoRECT(), 0, ListColor);
					D3DVERTEX_2D::Offset(vt, 4);
					pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��
				}

				//�X�N���[���o�[�̊J�n�ʒu�̕����������񃊃X�g�̃C�e���[�^��i�߂�
				std::list<tstring>::iterator itr = listStr.begin(); //�����񃊃X�g
				int ScrollBarPos;
				pGUIObj->SendMsg(MESSAGE(pGUIObj, SB_GETPOS, pScrollBarObj->GetID(), 4, &ScrollBarPos));
				for (int i = 0; i < ScrollBarPos; i++)
					itr++;

				int i = 0;
				for (; itr != listStr.end(); itr++)
				{
					if (i >= Lines) //�\���\�ȍs�����߂����ꍇ
						break;
					pGUIObj->GetpDrawStringObj()->Draw(rcAbsolutePos.left + 2, Y, (TCHAR *)itr->c_str(), pGUIObj->GetFontHeight(), 0xffffffff); //������`��
					Y += pGUIObj->GetFontHeight() + 4;
					i++;
				}
			}

			return;
		}

		//���b�Z�[�W�����v���V�[�W��
		BOOL ComboBox::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case MOUSE_LCLICK: //�}�E�X�N���b�N���ꂽ�ꍇ
				MouseClick(Message); //�}�E�X�N���b�N���̏���
				break;
			case CB_PUSH: //�R���{�{�b�N�X�̍Ō�ɗv�f��ǉ��i4Byte�ATCHAR *�j
				CB_Push(Message);
				break;
			case CB_GETSELECT: //�R���{�{�b�N�X�̑I�����ꂽ�v�f�̃C���f�b�N�X�𓾂�i4Byte, ���ʂ𓾂�int�^�ւ̃|�C���^�j
				*(int *)Message.pParam = SelectIndex;
				break;
			case CB_SETSELECT: //�R���{�{�b�N�X�̗v�f��I������i4Byte�Aint �I������v�f���j
				SelectIndex = (int)Message.pParam;
				break;
			case MOUSE_HOVER_MOVE: //�}�E�X�z�o�[���Ɉړ��i8Byte�Aho::PT<int>�ړ����̃}�E�X���W�j
				MouseHoverMove(Message);
				break;
			case MOUSE_HOVER_END: //�}�E�X�z�o�[�I��
				HoverPos = -2;
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}

		//�}�E�X�N���b�N���̏���
		void ComboBox::MouseClick(MESSAGE Message) 
		{
			ho::PT<int> ptClickPos = *(ho::PT<int> *)Message.pParam; //�}�E�X�N���b�N���W
			ptClickPos = GetLocalPtFromAbsolutePt(ptClickPos); //���W���΍��W����R���{�{�b�N�X���̃��[�J�����W�ɕϊ�

			if (ptClickPos.y >= pGUIObj->GetFontHeight() + 4) //�N���b�N���W�����X�g���̏ꍇ
			{
				int Index = GetListIndexFromMouseLocalPt(ptClickPos); //�}�E�X�̃��[�J�����W����I������郊�X�g�̃C���f�b�N�X���擾
				pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::CB_SETSELECT, Message.TargetID, 4, (LPVOID)Index)); //�R���{�{�b�N�X�̔C�ӗv�f��I�����郁�b�Z�[�W�𑗐M
			}

			ListOpen = TRUE - ListOpen; //���X�g�̊J��

			if (ListOpen) //���X�g���J�����ꍇ
			{
				this->rcPosition = rcOpenPosition;

				if ((signed)listStr.size() > GetListViewLines()) //�J���ꂽ��Ԃ̃��X�g�����Ɉ�x�ɕ\��������Ȃ��ꍇ
					pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SHOW, pScrollBarObj->GetID(), 4, (LPVOID)TRUE)); //�X�N���[���o�[��\�����郁�b�Z�[�W�𑗐M
				pGUIObj->PostMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::SETFOREGROUND, this->ID, 0, NULL)); //�e�I�u�W�F�N�g���ŁA�R���{�{�b�N�X���őO�ʕ\���ɐݒ肷�郁�b�Z�[�W�𑗐M
			} else {
				this->rcPosition.height = pGUIObj->GetFontHeight() + 4;
				pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SHOW, pScrollBarObj->GetID(), 4, (LPVOID)FALSE)); //�X�N���[���o�[���\���ɂ��郁�b�Z�[�W�𑗐M
			}

			return;
		}

		//�}�E�X�̃��[�J�����W����I������郊�X�g�̃C���f�b�N�X���擾
		int ComboBox::GetListIndexFromMouseLocalPt(ho::PT<int> ptMouseLocalPos) 
		{
			int ScrollBarPos;
			pGUIObj->SendMsg(MESSAGE(pGUIObj, SB_GETPOS, pScrollBarObj->GetID(), 4, &ScrollBarPos)); //�X�N���[���o�[�̈ʒu���擾

			int ClickIndex = ptMouseLocalPos.y / (pGUIObj->GetFontHeight() + 4) - 1 + ScrollBarPos; //�}�E�X���W����N���b�N���ꂽ���X�g�����������

			return ClickIndex;
		}

		//�R���{�{�b�N�X�̍Ō�ɗv�f��ǉ��i4Byte�ATCHAR *�j
		void ComboBox::CB_Push(MESSAGE Message) 
		{
			listStr.push_back(tstring((TCHAR *)Message.pParam));

			AdjustScrollBarSize(); //���X�g�̒����ɉ����ăX�N���[���o�[�̃T�C�Y�𒲐�

			return;
		}

		//���X�g�̒����ɉ����ăX�N���[���o�[�̃T�C�Y�𒲐�
		void ComboBox::AdjustScrollBarSize() 
		{
			int Lines = GetListViewLines(); //�J���ꂽ��Ԃ̃��X�g�����ɕ\���\�ȍs�����擾

			if ((signed)listStr.size() > Lines) //�J���ꂽ��Ԃ̃��X�g�����Ɉ�x�ɕ\��������Ȃ��ꍇ
			{
				pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SB_SETMAX, pScrollBarObj->GetID(), 4, LPVOID(listStr.size() - 1))); //�X�N���[���o�[�̍ő�l��ݒ�
				pScrollBarObj->MessageRouting(MESSAGE(pGUIObj, SB_SETRANGE, pScrollBarObj->GetID(), 4, (LPVOID)Lines)); //�X�N���[���o�[�̃����W��ݒ�
			}

			return;
		}

		//�J���ꂽ��Ԃ̃��X�g�����ɕ\���\�ȍs�����擾
		int ComboBox::GetListViewLines() 
		{
			return (rcOpenPosition.height - pGUIObj->GetFontHeight() + 4) / (pGUIObj->GetFontHeight() + 4); //�\���\�ȍs��
		}

		//�}�E�X�z�o�[���Ɉړ�
		void ComboBox::MouseHoverMove(MESSAGE Message)
		{
			ho::PT<int> ptClickPos = *(ho::PT<int> *)Message.pParam; //�}�E�X�N���b�N���W
			ptClickPos = GetLocalPtFromAbsolutePt(ptClickPos); //���W���΍��W����R���{�{�b�N�X���̃��[�J�����W�ɕϊ�

			if (ptClickPos.y >= pGUIObj->GetFontHeight() + 4) //�N���b�N���W�����X�g���̏ꍇ
			{
				int ScrollBarPos;
				pGUIObj->SendMsg(MESSAGE(pGUIObj, SB_GETPOS, pScrollBarObj->GetID(), 4, &ScrollBarPos)); //�X�N���[���o�[�̈ʒu���擾

				HoverPos = GetListIndexFromMouseLocalPt(ptClickPos) - ScrollBarPos; //�}�E�X�̃��[�J�����W����I������郊�X�g�̃C���f�b�N�X���擾

			} else {
				HoverPos = -1;
			}

			return;
		}















		//�R���X�g���N�^
		ScrollBar::ScrollBar(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, Element *pParentElementObj, eHV hv, int Max, int Pos, int Range) : Element(pGUIObj, rcPosition, ID, ParentID, z, pParentElementObj)
		{
			this->hv = hv;
			this->Max = Max;
			this->Pos = Pos;
			this->Range = Range;
			this->DragMoveDistance = 0;

			if (hv == Horizonal) //���������̏ꍇ
				Size = this->rcPosition.height;
			else if (hv == Vertical) //�c�����̏ꍇ
				Size = this->rcPosition.width;
		}

		//�f�X�g���N�^
		ScrollBar::~ScrollBar()
		{
		}

		//�`��
		void ScrollBar::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			const DWORD BorderColor = 0x7fffffff;

			//�S�̂̊O�g��`��
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(-1), 0, BorderColor);
			WORD Index[] = {0, 1, 3, 2, 0};
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, 0, 4, 4, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��

			//�����̃{�^���ƃX���C�_�[�Ƃ̋��ڂ�2�̐���`��
			if (hv == Horizonal) //���������̏ꍇ
			{
				;
			} else if (hv == Vertical) //���������̏ꍇ
			{
				vt[0].SetVertex(float(rcAbsolutePos.left), float(rcAbsolutePos.top + Size - 1), 0, BorderColor, 0, 0);
				vt[1].SetVertex(float(rcAbsolutePos.left + rcAbsolutePos.width - 1), float(rcAbsolutePos.top + Size - 1), 0, BorderColor, 0, 0);
				pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vt, sizeof(D3DVERTEX_2D));
				vt[0].SetVertex(float(rcAbsolutePos.left), float(rcAbsolutePos.top + rcAbsolutePos.height - 1 - Size + 1), 0, BorderColor, 0, 0);
				vt[1].SetVertex(float(rcAbsolutePos.left + rcAbsolutePos.width - 1), float(rcAbsolutePos.top + rcAbsolutePos.height - 1 - Size + 1), 0, BorderColor, 0, 0);
				pD3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vt, sizeof(D3DVERTEX_2D));

				pGUIObj->CalcTriangleVertex(vt, ho::PT<int>(rcAbsolutePos.left + int(Size * 0.5f), rcAbsolutePos.top + int(Size * 0.5f)), int(Size * 0.4f), ho::Direction8(ho::Direction8::Up)); //�O�p�`�̒��_���W���v�Z
				D3DVERTEX_2D::Offset(vt, 3);
				pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vt, sizeof(D3DVERTEX_2D)); //�O�p�`�`��
				pGUIObj->CalcTriangleVertex(vt, ho::PT<int>(rcAbsolutePos.left + int(Size * 0.5f), rcAbsolutePos.top + rcAbsolutePos.height - int(Size * 0.5f)), int(Size * 0.4f), ho::Direction8(ho::Direction8::Down)); //�O�p�`�̒��_���W���v�Z
				D3DVERTEX_2D::Offset(vt, 3);
				pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vt, sizeof(D3DVERTEX_2D)); //�O�p�`�`��

			}

			const DWORD SliderColor = 0x7f0000ff;

			//�X���C�_�[��`��
			if (hv == Horizonal) //���������̏ꍇ
			{
				;
			} else if (hv == Vertical) //���������̏ꍇ
			{
				int SliderHeight; //�X���C�_�[�����̍���
				int SliderBeginY; //�X���C�_�[�����̊J�nY���W
				double HeightPerPos; //1���̃X���C�_�[���̍���
				int Y1; //�c�}�~�̊J�n���W
				int Y2; //�c�}�~�̏I�����W
				GetVerticalSliderDimension(SliderHeight, SliderBeginY, HeightPerPos, Y1, Y2, rcAbsolutePos); //�c�����X�N���[���o�[�̃X���C�_�[�����̐��@���擾

				vt[0].SetVertex(float(rcAbsolutePos.left + 1), float(Y1), 0, SliderColor, 0, 0);
				vt[1].SetVertex(float(rcAbsolutePos.left + rcAbsolutePos.width - 1), float(Y1), 0, SliderColor, 0, 0);
				vt[2].SetVertex(float(rcAbsolutePos.left + 1), float(Y2), 0, SliderColor, 0, 0);
				vt[3].SetVertex(float(rcAbsolutePos.left + rcAbsolutePos.width - 1), float(Y2), 0, SliderColor, 0, 0);
				D3DVERTEX_2D::Offset(vt, 4);
				pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D));
			}

			return;
		}

		//�c�����X�N���[���o�[�̃X���C�_�[�����̐��@���擾
		void ScrollBar::GetVerticalSliderDimension(int &SliderHeight, int &SliderBeginY, double &HeightPerPos, int &Y1, int &Y2, RectWH rcAbsolutePos) 
		{
			/* ������ rcAbsolutePos �ɂ��A�X�N���[���o�[�̃��[�J�����W�Ŏ擾���邩�X�N���[���S�̂̐�΍��W�Ŏ擾���邩�����܂�B */

			SliderHeight = rcPosition.height - Size * 2; //�X���C�_�[�����̍���
			SliderBeginY = rcAbsolutePos.top + Size; //�X���C�_�[�����̊J�nY���W
			HeightPerPos = ((double)SliderHeight / double(Max + 1)); //1���̃X���C�_�[���̍���

			if (DragMoveDistance) //�}�E�X�h���b�O�ɂ��ړ�������ꍇ
			{
				//���݂̑I���ʒu����c�}�~���W���v�Z
				Y1 = int(SliderBeginY + HeightPerPos * DragBeginPos); //�c�}�~�̊J�n���W
				Y2 = int(SliderBeginY + HeightPerPos * (DragBeginPos + Range)) - 1; //�c�}�~�̏I�����W

				Y1 += DragMoveDistance; //�c�}�~�̊J�n���W�Ƀ}�E�X�h���b�O�������Z
				Y2 += DragMoveDistance; //�c�}�~�̏I�����W�Ƀ}�E�X�h���b�O�������Z
				if (Y1 < SliderBeginY) //�c�}�~���X���C�_�[�J�n�ʒu�������������ꍇ
				{
					Y1 = SliderBeginY;
					Y2 = Y1 + int(HeightPerPos * Range);
					Pos = 0;
				} else if (Y2 >= SliderBeginY + SliderHeight) //�c�}�~�̉������X���C�_�[�I���ʒu�����Ⴂ�ꍇ
				{
					Y2 = SliderBeginY + SliderHeight;
					Y1 = Y2 - int(HeightPerPos * Range) + 1;
					Pos = Max - Range + 1;
				} else {
					Pos = int((Y1 - SliderBeginY) / HeightPerPos); //�}�E�X�h���b�O����I���ʒu���Čv�Z
				}

			} else { //�}�E�X�h���b�O�ɂ��ړ����Ȃ��ꍇ
				//���݂̑I���ʒu����c�}�~���W���v�Z
				Y1 = int(SliderBeginY + HeightPerPos * Pos); //�c�}�~�̊J�n���W
				Y2 = int(SliderBeginY + HeightPerPos * (Pos + Range)); //�c�}�~�̏I�����W
			}

			return;
		}


		//���b�Z�[�W�v���V�[�W��
		BOOL ScrollBar::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case MOUSE_LCLICK: //�}�E�X�N���b�N���ꂽ�ꍇ
				MouseClick(Message); //�}�E�X�N���b�N���̏���
				break;
			case SB_SETMAX: //�X�N���[���o�[�̍ő�l��ݒ�i4Byte�Aint�j
				Max = (int)Message.pParam;
				break;
			case SB_SETPOS: //�X�N���[���o�[�̈ʒu��ݒ�i4Byte, int�j
				Pos = (int)Message.pParam;
				break;
			case SB_SETRANGE: //�X�N���[���o�[�̕\���̈��ݒ�i4Byte, int�j
				Range = (int)Message.pParam;
				break;
			case SB_GETPOS: //�X�N���[���o�[�̌��݈ʒu���擾(4Byte�A*int�j
				*(int *)Message.pParam = Pos;
				break;
			case MOUSE_LDRAG_BEGIN: //�}�E�X�̍��{�^���Ńh���b�O�J�n�i8Byte�Aho::PT<int>�N���b�N���̍��W�j
				DragBeginPos = Pos; //�h���b�O�J�n���̑I���ʒu��ۑ�
				break;
			case MOUSE_LDRAG_MOVE: //�}�E�X�̍��{�^���Ńh���b�O�ړ��i16Byte�Aho::PT<int>�N���b�N���̍��W�Aho::PT<int>���݂̃}�E�X���W�j
				MouseDragMove(Message); 
				break;
			case MOUSE_LDRAG_END: //�}�E�X�̍��{�^���Ńh���b�O�I���i16Byte�Aho::PT<int>�N���b�N���̍��W�Aho::PT<int>���݂̃}�E�X���W�j
				DragMoveDistance = 0;
				//Pos = DragBeginPos;
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}

		//�}�E�X�N���b�N���̏���
		void ScrollBar::MouseClick(MESSAGE Message) 
		{
			ho::PT<int> ptClickPos = *(ho::PT<int> *)Message.pParam; //�}�E�X�N���b�N���W
			ptClickPos = GetLocalPtFromAbsolutePt(ptClickPos); //���W���΍��W����X�N���[���o�[���̃��[�J�����W�ɕϊ�

			if (hv == Horizonal) //���������X�N���[���o�[�̏ꍇ
			{
				;
			} else if (hv == Vertical) //���������X�N���[���o�[�̏ꍇ
			{
				if (ptClickPos.y < Size) //�㑤�̃{�^���������ꂽ�ꍇ
				{
					Pos--;
				} else if (ptClickPos.y >= rcPosition.height - Size) //�����̃{�^���������ꂽ�ꍇ
				{
					Pos++;
				} else { //�X���C�_�[���������ꂽ�ꍇ
					int SliderHeight; //�X���C�_�[�����̍���
					int SliderBeginY; //�X���C�_�[�����̊J�nY���W
					double HeightPerPos; //1���̃X���C�_�[���̍���
					int Y1; //�c�}�~�̊J�n���W
					int Y2; //�c�}�~�̏I�����W
					GetVerticalSliderDimension(SliderHeight, SliderBeginY, HeightPerPos, Y1, Y2, RectWH(0, 0, rcPosition.width, rcPosition.height)); //�c�����X�N���[���o�[�̃X���C�_�[�����̐��@���擾

					if (ptClickPos.y < Y1) //�c�}�~����̕����������ꂽ�ꍇ
					{
						Pos -= 3;
					} else if (ptClickPos.y > Y2) //�c�}�~��艺�̕����������ꂽ�ꍇ
					{
						Pos += 3;
					}
				}
			}

			Clamp(); //�ݒ�l���͂ݏo���Ȃ��悤�ɂ���

			return;
		}

		//�ݒ�l���͂ݏo���Ȃ��悤�ɂ���
		void ScrollBar::Clamp() 
		{
			if (Pos < 0)
				Pos = 0;
			else if (Pos + Range - 1 > Max)
				Pos = Max - (Range - 1);

			return;
		}

		//�}�E�X�h���b�O�ړ����̏���
		void ScrollBar::MouseDragMove(MESSAGE Message) 
		{
			if (hv = Horizonal) //���������X�N���[���o�[�̏ꍇ
			{
				DragMoveDistance = ((ho::PT<int> *)Message.pParam)[1].x - ((ho::PT<int> *)Message.pParam)[0].x;
			} else if (hv = Vertical) //���������X�N���[���o�[�̏ꍇ
			{
				DragMoveDistance = ((ho::PT<int> *)Message.pParam)[1].y - ((ho::PT<int> *)Message.pParam)[0].y;
			}

			return;
		}









		//�R���X�g���N�^
		CheckBox::CheckBox(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, BOOL Check, TCHAR *pStr) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->Check = Check;
			this->strText = pStr;
		}

		//�f�X�g���N�^
		CheckBox::~CheckBox()
		{
		}

		//�`��
		void CheckBox::Draw() 
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			const DWORD BorderColor = 0xffffffff;

			RectWH rcBorder = rcAbsolutePos; //�����`�̋�`
			rcBorder.width = rcBorder.height; //�����`�ɂ���
			pGUIObj->DrawBorder(&rcBorder, BorderColor);

			if (Check) //�`�F�b�N�������Ă���ꍇ
			{
				D3DVERTEX_2D *vt = pGUIObj->Getpvt();

				D3DVERTEX_2D::SetVertexFromRECT(vt, &rcBorder.GethoRECT(-1), 0, BorderColor);
				WORD Index[] = {0, 3, 2, 1};
				pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 4, 2, Index, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D));
			}

			int MarginY = int((rcAbsolutePos.height - pGUIObj->GetFontHeight()) * 0.5);
			if (MarginY < 0)
				MarginY = 0;
			pGUIObj->GetpDrawStringObj()->Draw(rcAbsolutePos.left + rcAbsolutePos.height + 2, rcAbsolutePos.top + MarginY, (TCHAR *)strText.c_str(), pGUIObj->GetFontHeight(), BorderColor); //������`��

			return;
		}

		//���b�Z�[�W�v���V�[�W��
		BOOL CheckBox::MessageProcedure(MESSAGE Message, BOOL SendChild) 
		{
			switch (Message.Message)
			{
			case MOUSE_LCLICK: //�}�E�X�N���b�N���ꂽ�ꍇ
				{
					BOOL NewCheck = TRUE - Check; //�V�����`�F�b�N���
					pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::CHECKBOX_SETCHECK, Message.TargetID, 4, (LPVOID)NewCheck)); //�`�F�b�N��ԕύX�̃��b�Z�[�W�𑗐M
				}
				break;
			case CHECKBOX_SETCHECK: //�`�F�b�N�{�b�N�X�̃`�F�b�N��ύX�i4Byte�ABOOL�j
				this->Check = (BOOL)Message.pParam;
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}









		//�R���X�g���N�^
		GaugeSlider::GaugeSlider(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, eHV hv, double Max, double Pos) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->Max = Max;
			this->Pos = Pos;
			Clamp(Pos, 0.0, Max);
			this->hv = hv;
		}

		//�f�X�g���N�^
		GaugeSlider::~GaugeSlider()
		{
		}

		//�`��
		void GaugeSlider::Draw()
		{
			LPDIRECT3DDEVICE9 pD3DDevice = pGUIObj->GetpD3DDevice();
			D3DVERTEX_2D *vt = pGUIObj->Getpvt();
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			const DWORD BackgroundColor = 0xff7f7f7f, BarColor = 0xff0000ff, BorderColor = 0xffffffff;

			//�w�i�̋�`��`��
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcAbsolutePos.GethoRECT(), (float)0, BackgroundColor);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��

			//���ݒl�̋�`��`��
			RectWH rcBar = rcAbsolutePos;
			rcBar.width = int(rcBar.width * (Pos / Max));
			D3DVERTEX_2D::SetVertexFromRECT(vt, &rcBar.GethoRECT(), (float)0, BarColor);
			D3DVERTEX_2D::Offset(vt, 4);
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��

			pGUIObj->DrawBorder(&rcAbsolutePos, BorderColor); //�O�g��`��


			return;
		}

		//���b�Z�[�W�v���V�[�W��
		BOOL GaugeSlider::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case MOUSE_LCLICK: //�}�E�X�̍��N���b�N�m��i8Byte�Aho::PT<int>�}�E�X���W�j
				{
					double NewPos;
					if (GetPosFromMousePos(&NewPos, (ho::PT<int> *)Message.pParam)) //�}�E�X���W���Q�[�W���̒l���擾���A�擾�ł����ꍇ
						pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::GAUGESLIDER_SETPOS, Message.TargetID, 8, (LPVOID)&NewPos)); //�X���C�_�[�̒l�ύX�̃��b�Z�[�W�𑗂�
				}
				break;
			case MOUSE_LDRAG_MOVE: //�}�E�X�̍��{�^���Ńh���b�O�ړ��i16Byte�Aho::PT<int>�N���b�N���̍��W�Aho::PT<int>���݂̃}�E�X���W�j
				{
					double NewPos;
					if (GetPosFromMousePos(&NewPos, (ho::PT<int> *)Message.pParam + 1)) //�}�E�X���W���Q�[�W���̒l���擾���A�擾�ł����ꍇ
						pGUIObj->SendMsg(ho::GUI::MESSAGE(pGUIObj, ho::GUI::GAUGESLIDER_SETPOS, Message.TargetID, 8, (LPVOID)&NewPos)); //�X���C�_�[�̒l�ύX�̃��b�Z�[�W�𑗂�
				}
				break;
			case GAUGESLIDER_SETPOS: //�Q�[�W�X���C�_�[�̒l��ύX�i8Byte�Adouble *�V�����l�j
				Pos = *(double *)Message.pParam;
				Clamp(Pos, 0.0, Max);
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}

		//�}�E�X���W���Q�[�W���̒l���擾
		BOOL GaugeSlider::GetPosFromMousePos(double *pPos, ho::PT<int> *pptMousePos) 
		{
			RectWH rcAbsolutePos = this->GetAbsolutePos();

			if (rcAbsolutePos.GethoRECT().JudgeContain(*pptMousePos)) //���W���X���C�_�[�͈͓̔��̏ꍇ
			{
				double Ratio;
				if (hv == Horizonal) //�������̏ꍇ
				{
					Ratio = double(pptMousePos->x - rcAbsolutePos.left) / (double)rcAbsolutePos.width;
					*pPos = Ratio * this->Max;
				} else if (hv == Vertical) //�c�����̏ꍇ
				{
				} else {
					return FALSE;
				}

				return TRUE;
			}

			return FALSE;
		}







		//�R���X�g���N�^
		Image::Image(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, ComPtr<LPDIRECT3DTEXTURE9> cpTexture) : Element(pGUIObj, rcPosition, ID, ParentID, z)
		{
			this->cpTexture = cpTexture;
		}

		//�f�X�g���N�^
		Image::~Image()
		{
		}

		//�`��
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
			pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, D3DVERTEX::IndexSquare, D3DFMT_INDEX16, vt, sizeof(D3DVERTEX_2D)); //�E�B���h�E�`��

			pD3DDevice->SetTexture(0, pOldTexture);

			return;
		}

		//���b�Z�[�W�v���V�[�W��
		BOOL Image::MessageProcedure(MESSAGE Message, BOOL SendChild)
		{
			switch (Message.Message)
			{
			case IMG_SETTEXTURE: //�C���[�W�Ƀe�N�X�`����ݒ�i4Byte�AComPtr<LPDIRECT3DTEXTURE9>�j
				this->cpTexture = *(ComPtr<LPDIRECT3DTEXTURE9> *)(Message.pParam);
				break;
			case IMG_RELEASETEXTURE: //�e�N�X�`����Com�|�C���^���������
				this->cpTexture = NULL; //�Q�ƃJ�E���^�����炷
				break;
			}

			DefaultMessageProcedure(Message, SendChild);

			return SendChild;
		}
	}
}
