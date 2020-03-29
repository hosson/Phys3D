//GUI�N���X

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
		//�O���錾
		class GUI;
		class Element;
		class ScrollBar;

		//���b�Z�[�W
		enum eMessage
		{
			DRAW,					//�`��
			GET_PTR,				//�G�������g�I�u�W�F�N�g�̃|�C���^���擾�i4Byte�Avoid **�|�C���^���i�[����_�u���|�C���^�j
			MOUSE_LCLICK,			//�}�E�X�̍��N���b�N�m��i8Byte�Aho::PT<int>�}�E�X���W�j
			MOUSE_LDRAG_BEGIN,		//�}�E�X�̍��{�^���Ńh���b�O�J�n�i8Byte�Aho::PT<int>�N���b�N���̍��W�j
			MOUSE_LDRAG_MOVE,		//�}�E�X�̍��{�^���Ńh���b�O�ړ��i16Byte�Aho::PT<int>�N���b�N���̍��W�Aho::PT<int>���݂̃}�E�X���W�j
			MOUSE_LDRAG_END,		//�}�E�X�̍��{�^���Ńh���b�O�I���i16Byte�Aho::PT<int>�N���b�N���̍��W�Aho::PT<int>���݂̃}�E�X���W�j
			MOUSE_HOVER_BEGIN,		//�}�E�X�z�o�[�J�n
			MOUSE_HOVER_MOVE,		//�}�E�X�z�o�[���Ɉړ��i8Byte�Aho::PT<int> *�ړ����̃}�E�X���W�j
			MOUSE_HOVER_END,		//�}�E�X�z�o�[�I��
			GET_TOPID_FROMPOS,		//�C�ӂ̍��W��̈�ԏ㑤�̃G�������g��ID���擾�i12Byte�Aho::PT<int>�C�Ӎ��W�Aint *ID���i�[����|�C���^�j
			SETPOS_CENTER,			//�N���C�A���g�̈�̒����Ɉړ�
			SHOW,					//�\���A��\���̐؂�ւ��i4Byte�ABOOL�j
			SHOW_SWITCH,			//�\���̔�\�� �����݂ɋC�s�ς���
			GET_SHOW,				//�\���A��\���̏�Ԃ��擾����i4Byte�A *BOOL ���ʂ𓾂�|�C���^�j
			SETTEXT,				//�E�B���h�E�⃉�x�����̕������ݒ�i4Byte�ATCHAR *�j
			POSITION,				//�G�������g�̈ʒu���ircPosition�j��ύX����i16Byte�ARectWH *�V�����ʒu���j
			MOVE,					//�G�������g�̍�����W��ύX����i8Byte�Aho::PT<int> *�V�������W�j
			SETFOREGROUND,			//�e�G�������g�̃��X�g��ň�ԍŌ�i��ʏ�ł͏㑤�j�ɕ��ёւ���
			GET_CLIENT_RECT,		//�G�������g�̃N���C�A���g�̈�̐�Έʒu���擾�i4Byte�ARectWH *�N���C�A���g�̈���������ރ|�C���^�j
			CB_PUSH,				//�R���{�{�b�N�X�̍Ō�ɗv�f��ǉ��i4Byte�ATCHAR *�j
			CB_GETSELECT,			//�R���{�{�b�N�X�̑I�����ꂽ�v�f�̃C���f�b�N�X�𓾂�i4Byte, ���ʂ𓾂�int�^�ւ̃|�C���^�j
			CB_SETSELECT,			//�R���{�{�b�N�X�̗v�f��I������i4Byte�Aint �I������v�f���j
			SB_SETMAX,				//�X�N���[���o�[�̍ő�l��ݒ�i4Byte�Aint�j
			SB_SETPOS,				//�X�N���[���o�[�̈ʒu��ݒ�i4Byte, int�j
			SB_SETRANGE,			//�X�N���[���o�[�̕\���̈��ݒ�i4Byte, int�j
			SB_GETPOS,				//�X�N���[���o�[�̌��݈ʒu���擾(4Byte�A*int�j
			CHECKBOX_SETCHECK,		//�`�F�b�N�{�b�N�X�̃`�F�b�N��ύX�i4Byte�ABOOL�j
			GAUGESLIDER_SETPOS,		//�Q�[�W�X���C�_�[�̒l��ύX�i8Byte�Adouble *�V�����l�j
			IMG_SETTEXTURE,			//�C���[�W�Ƀe�N�X�`����ݒ�i4Byte�AComPtr<LPDIRECT3DTEXTURE9>�j
			IMG_RELEASETEXTURE,		//�e�N�X�`����Com�|�C���^���������
			EL_DELETE,				//�G�������g���폜����
		};

		//�E�B���h�E�̃X�^�C��
		namespace eWindowStyle
		{
			enum e
			{
				TITLE = 0x00000001,			//�^�C�g���o�[���g�p����
				RESIZE = 0x00000002,		//���T�C�Y��������
				CLOSEBUTTON = 0x00000004,	//����{�^�����g�p����
				MINIBUTTON = 0x00000008,	//�ŏ����{�^�����g�p����
				MAXBUTTON = 0x00000010		//�ő剻�{�^�����g�p����
			};
		}


		//���b�Z�[�W�N���X
		struct MESSAGE 
		{
		public:
			//�R���X�g���N�^
			MESSAGE() {};
			MESSAGE(GUI *pGUIObj, eMessage Message, int TargetID, int Bytes, LPVOID pParam) { this->pGUIObj = pGUIObj; this->Message = Message; this->TargetID = TargetID; this->Bytes = Bytes; this->pParam = pParam; }

			GUI *pGUIObj; //GUI�I�u�W�F�N�g�ւ̃|�C���^
			eMessage Message; //���b�Z�[�W�̎��
			int TargetID; //����ƂȂ�ID�i�ċA�����őS�̂ɑ���ꍇ��-1�j
			int Bytes; //�t�����̃o�C�g��
			LPVOID pParam; //�t�����ւ̃|�C���^
		};


		enum eHV {Horizonal, Vertical}; //���������A��������

		class GUI
		{
		public:
			struct MOUSEDRAG //�}�E�X�h���b�O���\����
			{
				int DragElementID; //�N���b�N���ꂽ�G�������g��ID
				ho::PT<int> ptClickPos; //�N���b�N���ꂽ�ʒu
				BOOL Enable; //�h���b�O�����ǂ���
			};
			struct MOUSEHOVER //�}�E�X�z�o�[���\����
			{
				int HoverElementID; //�z�o�[���̃G�������g��ID
				BOOL Enable; //�z�o�[�����ǂ���
			};
		public:
			GUI(LPDIRECT3DDEVICE9 pD3DDevice, DrawString *pDrawStringObj, RectWH pRectWH, LONG FontHeight, UserInput *pUserInputObj); //�R���X�g���N�^
			~GUI(); //�f�X�g���N�^

			void SendMsg(MESSAGE Message); //���b�Z�[�W�������ɑ���
			void PostMsg(MESSAGE Message); //���b�Z�[�W�����Ƃő���i���ݏ������̃��b�Z�[�W���I��������Ɓj
			void DefaultMessageProcedure(MESSAGE Message); //�f�t�H���g���b�Z�[�W�v���V�[�W��
			void FrameProcess(); //1�t���[�����̏���
			void Draw(); //�`��
			Element *GetpElementObj(int ID); //ID����G�������g���擾

			BOOL CreateWnd(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption, BOOL Show, eWindowStyle::e WindowStyle); //�E�B���h�E�쐬
			BOOL CreateButton(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption); //�{�^���쐬
			BOOL CreateLabel(ho::RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pStr); //���x���쐬
			BOOL CreateComboBox(ho::RectWH rcPosition, int ID, int ParentID, int z); //�R���{�{�b�N�X�쐬
			BOOL CreateCheckBox(ho::RectWH rcPosition, int ID, int ParentID, int z, BOOL Check, TCHAR *pStr); //�`�F�b�N�{�b�N�X�쐬
			BOOL CreateGaugeSlider(ho::RectWH rcPosition, int ID, int ParentID, int z, eHV hv, double Max, double Pos); //�Q�[�W�X���C�_�[�쐬
			BOOL CreateImage(ho::RectWH rcPosition, int ID, int ParentID, int z, ComPtr<LPDIRECT3DTEXTURE9> cpTexture); //�C���[�W�쐬

			int GetNullSystemElementID(); //�o�^�\�ȃV�X�e���G�������gID���擾
			int RegistSystemElementObj(Element *pElementObj); //�쐬�����V�X�e���G�������g��o�^���AID���Ԃ�
			void DeleteSystemElementObj(int ID); //�w�肳�ꂽID�̃V�X�e���G�������g���Ǘ����X�g����폜����

			void CalcTriangleVertex(ho::D3DVERTEX_2D *pVertex, ho::PT<int> ptCenter, int Radius, ho::Direction8 Angle); //�O�p�`��3���_�̍��W���v�Z����
			void DrawBorder(RectWH *pRectWH, DWORD Color); //��`�̘g��`�悷��

			//�A�N�Z�b�T
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
			CharTexture *pCharTextureObj; //�����e�N�X�`���I�u�W�F�N�g�ւ̃|�C���^
			DrawString *pDrawStringObj; //�����`��I�u�W�F�N�g�ւ̃|�C���^
			LONG FontHeight; //�t�H���g�̍���
			UserInput *pUserInputObj; //���[�U�[���̓I�u�W�F�N�g�ւ̃|�C���^
			Element *pTopElementObj; //��ԏ�̊K�w�ɂ���G�������g�ւ̃|�C���^
			D3DVERTEX_2D vt[4];
			std::vector<Element *> vectorSystemElementID; //-1�ȍ~�̕��̕�����ID�i�V�X�e����p�j���Ǘ�����z��
			MOUSEDRAG MouseDrag; //�}�E�X�̃h���b�O���
			MOUSEHOVER MouseHover; //�}�E�X�z�o�[�̏��
			DWORD Time; //FrameProcess ���\�b�h�����s���ꂽ��
			std::queue<MESSAGE> queueMessage; //SndMsg�֐��ő���ꂽ���b�Z�[�W
			bool EnableUserInput; //���[�U�[���͂��󂯕t���邩�ǂ���

			bool (*pMessageProcedure)(void *pObj, MESSAGE Message); //���b�Z�[�W�v���V�[�W���֐��ւ̃|�C���^
			void *pMessageProcedureObj; //���b�Z�[�W�v���V�[�W���֐������I�u�W�F�N�g�ւ̃|�C���^
			void MouseInput(); //�}�E�X�Ȃǂ̓��͂�����
			void ProcessMessageQue(); //SnedMsg�֐��ŗ��܂������b�Z�[�W������
			void DeleteAllElement(); //�S�ẴG�������g���폜����
		};

		class Element
		{
		public:
			Element(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, Element *pParentElementObj = NULL); //�R���X�g���N�^
			virtual ~Element(); //�f�X�g���N�^
			void MessageRouting(MESSAGE Message); //���b�Z�[�W�̌o�H�I��
			virtual BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //���b�Z�[�W�����v���V�[�W��
			BOOL DefaultMessageProcedure(MESSAGE Message, BOOL SendChild); //�f�t�H���g���b�Z�[�W�����v���V�[�W��
			virtual void Draw() {} //�`��
			Element *Element::GetpElementObj(int ID); //ID����ċA�����ŊK�w�\���̒�����Y������G�������g��T��
			void AddChildElementObj(Element *pElementObj); //�q���G�������g��ǉ�
			RectWH GetAbsolutePos(); //��Έʒu���擾
			RectWH GetAbsoluteClientPos(); //�N���C�A���g�̈�݂̂̐�Έʒu���擾
			void SetForegroundChildID(int ID); //�C�ӂ�ID�̎q���I�u�W�F�N�g���őO�ʂɕ��ёւ���

			static bool Less(const Element *p1, const Element *p2) { return *p1 < *p2; }

			//���Z�q
			friend bool operator < (const Element &obj1, const Element &obj2) { return obj1.z < obj2.z; } //STL�̃\�[�g�p

			//�A�N�Z�b�T
			int GetID() { return ID; }
			RectWH GetrcPosition() { return rcPosition; }
			BOOL GetbShow() { return bShow; }
		protected:
			GUI *pGUIObj;
			RectWH rcPosition; //�ʒu�i�e�G�������g����̑��Έʒu�j
			ho::PT<int> MinSize; //rcPosition�̍ŏ��T�C�Y
			RECT rcClientOffset; //rcPosition ����N���C�A���g�̈�܂ł̊Ԋu�i�g��^�C�g���o�[�Ȃǁj
			int ID; //�G�������g�ŗL��ID
			int ParentID; //�e�ƂȂ�G�������g��ID
			std::list<Element *> listpChildElementObj; //�q���̃G�������g�I�u�W�F�N�g�ւ̃|�C���^���X�g
			Element *pParentElementObj; //�e�ƂȂ�G�������g�I�u�W�F�N�g�ւ̃|�C���^
			int z; //�傫���l�̂��̂قǎ�O�ɕ`�悳���
			BOOL bShow; //�\�����邩�ǂ���
			BOOL Hover; //�z�o�[��Ԃ��ǂ���

			void GetTopIDFromPos(MESSAGE Message); //GET_TOPID_FROMPOS�i�C�ӂ̍��W��̈�ԏ㑤�̃G�������g��ID���擾�j
			RectWH GetAbsolutePosAlg(RectWH Rect, int Level); //��Έʒu���擾�i�����ċA�֐��j
			RectWH GetAbsoluteClientPosAlg(RectWH Rect, int Level); //�N���C�A���g�̈�݂̂̐�Έʒu���擾�i�����ċA�֐��j 
			void SetPos_Center(MESSAGE Message); //�N���C�A���g�̈�̒����Ɉړ�
			void Show(MESSAGE Message); //�\���A��\���̐؂�ւ��i4Byte�ABOOL�j
			virtual void SetText(MESSAGE Message) {}
			ho::PT<int> GetLocalPtFromAbsolutePt(ho::PT<int> ptPos); //��΍��W����G�������g���̑��΍��W�ɕϊ�
		};

		class Window : public Element
		{
		public:
			Window(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption, BOOL Show, eWindowStyle::e WindowStyle); //�R���X�g���N�^
			~Window(); //�f�X�g���N�^
			void Draw(); //�`��
			void SetText(MESSAGE Message); //�������ݒ�
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //���b�Z�[�W�v���V�[�W��
		private:
			enum eRegion //�E�B���h�E���̗̈��\���񋓑�
			{
				LeftTop, Top, RightTop, Left, Right, LeftBottom, Bottom, RightBottom, TitleBar, Client
			};
			tstring strCaption; //�^�C�g���o�[������
			eWindowStyle::e WindowStyle; //�E�B���h�E�̃X�^�C��
			RectWH rcDragBeginPos; //rcPosition �̃h���b�O�J�n���̈ʒu�i�e�G�������g����̑��Έʒu�j
			eRegion DragBeginRegion; //�h���b�O�J�n���̃}�E�X���W�̗̈�
			int BorderWidth; //�O���̘g�̕�

			void Mouse_LDrag_Move(MESSAGE Message); //�}�E�X���{�^���Ńh���b�O�ړ����̏���
			eRegion GetRegion(ho::PT<int> *pptLocalPos); //�G�������g���̍��W����̈���擾
			void MouseHover(ho::PT<int> *ptMousePos); //�}�E�X�z�o�[���̃J�[�\���A�C�R���ύX����
		};

		class Button : public Element
		{
		public:
			Button(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pCaption); //�R���X�g���N�^
			~Button(); //�f�X�g���N�^
			void Draw(); //�`��
		private:
			tstring strCaption; //�^�C�g���o�[������
			BOOL Enable; //�g�p�\���ǂ���
		};

		class Label : public Element
		{
		public:
			Label(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, TCHAR *pStr); //�R���X�g���N�^
			~Label(); //�f�X�g���N�^
			void Draw(); //�`��
			void SetText(MESSAGE Message); //�������ݒ�
		private:
			tstring strLabel; //���x��������
		};

		class ComboBox : public Element
		{
		public:
			ComboBox(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z); //�R���X�g���N�^
			~ComboBox(); //�f�X�g���N�^
			void Draw(); //�`��
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //���b�Z�[�W�v���V�[�W��
		private:
			std::list<tstring> listStr; //�e���X�g�̕�����
			int SelectIndex; //�I�𒆂̃��X�g�C���f�b�N�X
			RectWH rcOpenPosition; //���X�g���J�����Ƃ��̃T�C�Y
			BOOL ListOpen; //���X�g���J����Ă��邩�ǂ���
			int ScrollBarSize; //�X�N���[���o�[�̃T�C�Y
			ScrollBar *pScrollBarObj; //�c�����̃X�N���[���o�[�G�������g�ւ̃|�C���^
			int HoverPos; //�}�E�X�z�o�[���̃��X�g�ʒu�i-2=�z�o�[���Ă��Ȃ��A-1=�㕔�{�b�N�X�Ńz�o�[�A0�ȏ�=���X�g���Ńz�o�[�j

			void MouseClick(MESSAGE Message); //�}�E�X�N���b�N���̏���
			void CB_Push(MESSAGE Message); //�R���{�{�b�N�X�̍Ō�ɗv�f��ǉ��i4Byte�ATCHAR *�j
			void AdjustScrollBarSize(); //���X�g�̒����ɉ����ăX�N���[���o�[�̃T�C�Y�𒲐�
			int GetListViewLines(); //�J���ꂽ��Ԃ̃��X�g�����ɕ\���\�ȍs�����擾
			int GetListIndexFromMouseLocalPt(ho::PT<int> ptMouseLocalPos); //�}�E�X�̃��[�J�����W����I������郊�X�g�̃C���f�b�N�X���擾
			void MouseHoverMove(MESSAGE Message); //�}�E�X�z�o�[���Ɉړ�
		};

		class ScrollBar : public Element
		{
		public:
			enum eHV {Horizonal, Vertical}; //���������A��������
		public:
			ScrollBar(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, Element *pParentElementObj, eHV hv, int Max, int Pos, int Range); //�R���X�g���N�^
			~ScrollBar(); //�f�X�g���N�^
			void Draw(); //�`��
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //���b�Z�[�W�v���V�[�W��
		private:
			eHV hv; //�X�N���[���o�[�̕���
			int Max; //�ő�l�i�ŏ��l��0�j
			int Pos; //���݂̃X�N���[���ʒu
			int Range; //�܂݂̕�
			int Size; //�����ڂ̕�
			int DragMoveDistance; //�h���b�O���Ƀc�}�~���������ꂽ�����i�s�N�Z���P�ʁj
			int DragBeginPos; //�h���b�O�J�n���̑I���ʒu

			void MouseClick(MESSAGE Message); //�}�E�X�N���b�N���̏���
			void Clamp(); //�ݒ�l���͂ݏo���Ȃ��悤�ɂ���
			void GetVerticalSliderDimension(int &SliderHeight, int &SliderBeginY, double &HeightPerPos, int &Y1, int &Y2, RectWH rcAbsolutePos); //�c�����X�N���[���o�[�̃X���C�_�[�����̐��@���擾
			void MouseDragMove(MESSAGE Message); //�}�E�X�h���b�O�ړ����̏���
		};

		class CheckBox : public Element
		{
		public:
			CheckBox(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, BOOL Check, TCHAR *pStr); //�R���X�g���N�^
			~CheckBox(); //�f�X�g���N�^
			void Draw(); //�`��
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //���b�Z�[�W�v���V�[�W��
		private:
			BOOL Check;
			tstring strText; //�`�F�b�N�{�b�N�X���̃e�L�X�g
		};

		class GaugeSlider : public Element
		{
		public:
			GaugeSlider(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, eHV hv, double Max, double Pos); //�R���X�g���N�^
			~GaugeSlider(); //�f�X�g���N�^
			void Draw(); //�`��
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //���b�Z�[�W�v���V�[�W��
		private:
			eHV hv; //�c��
			double Max; //�ő�l
			double Pos; //���݂̒l

			BOOL GetPosFromMousePos(double *pPos, ho::PT<int> *pptMousePos); //�}�E�X���W���Q�[�W���̒l���擾
		};

		class Image : public Element
		{
		public:
			Image(GUI *pGUIObj, RectWH rcPosition, int ID, int ParentID, int z, ComPtr<LPDIRECT3DTEXTURE9 > cpTexture); //�R���X�g���N�^
			~Image(); //�f�X�g���N�^
			void Draw(); //�`��
			BOOL MessageProcedure(MESSAGE Message, BOOL SendChild); //���b�Z�[�W�v���V�[�W��
		private:
			ComPtr<LPDIRECT3DTEXTURE9> cpTexture;
		};
	}
}
