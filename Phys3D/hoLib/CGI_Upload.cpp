#include "CGI_Upload.h"
#include "tstring.h"

#define _WIN32_WINNT 0x0501

#include <boost/asio.hpp>
#include <iostream>
#include <ostream>
#include <fstream>
#include "Debug.h"

#include <Windows.h>

//�R���X�g���N�^
ho::CGI_Upload::CGI_Upload(const std::string &strHost)
	: strHost(strHost)
{
}

//�f�X�g���N�^
ho::CGI_Upload::~CGI_Upload()
{
}

//�f�[�^�𑗐M����
bool ho::CGI_Upload::Upload(const void *pData, const unsigned int Size) 
{
	boost::asio::ip::tcp::iostream tcpStream(this->strHost, "http"); //�z�X�g���iIP�j�ƃ|�[�g�ԍ����w��

	if (tcpStream.fail()) //�T�[�o���������Ȃ��ꍇ
		return false; //���s

	std::string strBoundary = "xYzZY";

	//�w�b�_����
	std::ostringstream osHeader;
	{
		//�o�[�W�������ʏ��
		osHeader << "--" << strBoundary << "\r\n";
		osHeader << "Content-Disposition: form-data; name=\"version\"\r\n\r\n";
		osHeader << "0.01";
		osHeader << "\r\n";

		//�t�@�C���{�̏��
		osHeader << "--" << strBoundary << "\r\n";
		osHeader << "Content-Disposition: form-data; name=\"upload_file\"; filename=\"";
		osHeader << "TestFile.txt";	//�t�@�C����
		osHeader << "\"\r\n";
		osHeader << "Content-Type: application/octet-stream\r\n";
		osHeader << "\r\n";
	}

	//�t�b�^����
	std::ostringstream osHooter;
	osHooter << "--" << strBoundary << "--";

	//���M
	tcpStream << "POST /cgi-bin/physius/upload.cgi HTTP/1.1\r\n";
	tcpStream << "Host: " << strHost << "\r\n"; //�z�X�g��
	tcpStream << "Content-Length:";
	tcpStream << std::dec << Size + osHeader.str().size() + osHooter.str().size(); //�w�b�_+�t�b�^+�t�@�C���{�̂̃T�C�Y
	tcpStream << "\r\n";
	tcpStream << "Connection: Keep-Alive\r\n";
	tcpStream << "Cache-Control: no-cache\r\n";
	tcpStream << "Content-Type: multipart/form-data; boundary=" << strBoundary << "\r\n";
	tcpStream << "\r\n";

	tcpStream << osHeader.str(); //�w�b�_���M

	tcpStream.write((char *)pData, Size); //�f�[�^�o��	

	tcpStream << osHooter.str(); //�t�b�^���M

	tcpStream.flush(); //�o�b�t�@�ɗ��߂Ȃ�

	//�T�[�o���瑗���Ă����f�[�^����M���A�f�o�b�K�ɏo�͂���
	std::string result;
	while (getline(tcpStream, result))
	{
		ho::tstring str = ho::CharToWChar(result.c_str());
		OutputDebugString(str.c_str());
		OutputDebugString(TEXT("\n"));
	}

	return true;
}

//�t�@�C���𑗐M����
bool ho::CGI_Upload::Upload(const ho::tstring &strFilename, const std::string &strVer) 
{
	try
	{
		boost::asio::ip::tcp::iostream tcpStream(this->strHost, "http"); //�z�X�g���iIP�j�ƃ|�[�g�ԍ����w��
		tcpStream.expires_from_now(boost::posix_time::seconds(4)); //�^�C���A�E�g�̐ݒ�

		if (tcpStream.fail()) //�T�[�o���������Ȃ��ꍇ
			throw ho::Exception(TEXT("�T�[�o���������܂���ł����B"), __WFILE__, __LINE__, true);

		HANDLE hFile = CreateFile(strFilename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) //�t�@�C�����J���Ȃ������ꍇ
		{
			ho::tstring str = TEXT("�t�@�C�����J���܂���ł����B");
			throw ho::Exception(str, __WFILE__, __LINE__, true);
		}

		const int Size = (int)GetFileSize(hFile, NULL); //�t�@�C���T�C�Y�擾

		if (!CloseHandle(hFile)) //�t�@�C��������Ȃ������ꍇ
		{
			ho::tstring str = strFilename + TEXT(" �����܂���ł����B");
			throw ho::Exception(str, __WFILE__, __LINE__, true);
		}

		OutputDebugString(TEXT("Begin send file...\n"));

		std::ifstream ifs(strFilename.c_str());

		std::string strBoundary = "xYzZY00000000000654684605406840888888888888888888888888";

		//�w�b�_����
		std::ostringstream osHeader;
		{
			//�o�[�W�������ʏ��
			osHeader << "--" << strBoundary << "\r\n";
			osHeader << "Content-Disposition: form-data; name=\"version\"\r\n\r\n";
			osHeader << strVer;
			osHeader << "\r\n";

			//�t�@�C���{�̏��
			osHeader << "--" << strBoundary << "\r\n";
			osHeader << "Content-Disposition: form-data; name=\"upload_file\"; filename=\"";
			std::string strFilename2 = ho::WCharToChar(strFilename.c_str()); //���C�h����(TCHAR)���}���`�o�C�g����(char)�ɕϊ�
			osHeader << strFilename2; //�t�@�C����
			osHeader << "\"\r\n";
			osHeader << "Content-Type: application/octet-stream\r\n";
			osHeader << "\r\n";
		}

		//�t�b�^����
		std::ostringstream osHooter;
		osHooter << "--" << strBoundary << "--";

		//���M
		tcpStream << "POST /cgi-bin/physius/upload.cgi HTTP/1.1\r\n";
		tcpStream << "Host: " << strHost << "\r\n"; //�z�X�g��
		tcpStream << "Content-Length:";
		tcpStream << std::dec << Size + osHeader.str().size() + osHooter.str().size(); //�w�b�_+�t�b�^+�t�@�C���{�̂̃T�C�Y
		tcpStream << "\r\n";
		tcpStream << "Connection: Keep-Alive\r\n";
		tcpStream << "Cache-Control: no-cache\r\n";
		tcpStream << "Content-Type: multipart/form-data; boundary=" << strBoundary << "\r\n";
		tcpStream << "\r\n";

		tcpStream << osHeader.str(); //�w�b�_���M

		//�t�@�C���X�g���[���� tcp �X�g���[���ɑ��M
		char Buffer[256];
		int ReadSize = 0;
		while (!ifs.eof())
		{
			ifs.read(Buffer, sizeof(Buffer));
			tcpStream.write(Buffer, ifs.gcount());
			tcpStream.flush(); //�o�b�t�@�ɗ��߂Ȃ�
		}

		tcpStream << osHooter.str(); //�t�b�^���M

		tcpStream.flush(); //�o�b�t�@�ɗ��߂Ȃ�

		OutputDebugString(TEXT("Receive Message from Server...\n"));

		//�T�[�o���瑗���Ă����f�[�^����M���A�f�o�b�K�ɏo�͂���
		std::string result;
		while (getline(tcpStream, result))
		{
			ho::tstring str = ho::CharToWChar(result.c_str());
			OutputDebugString(str.c_str());
			OutputDebugString(TEXT("\n"));
		}
	}
	catch (ho::Exception &e)
	{
		ER(e.strMsg.c_str(), e.pFile, e.Line, e.Continue);
		return false;
	}

	return true;
}
