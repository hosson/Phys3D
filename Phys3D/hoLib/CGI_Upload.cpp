#include "CGI_Upload.h"
#include "tstring.h"

#define _WIN32_WINNT 0x0501

#include <boost/asio.hpp>
#include <iostream>
#include <ostream>
#include <fstream>
#include "Debug.h"

#include <Windows.h>

//コンストラクタ
ho::CGI_Upload::CGI_Upload(const std::string &strHost)
	: strHost(strHost)
{
}

//デストラクタ
ho::CGI_Upload::~CGI_Upload()
{
}

//データを送信する
bool ho::CGI_Upload::Upload(const void *pData, const unsigned int Size) 
{
	boost::asio::ip::tcp::iostream tcpStream(this->strHost, "http"); //ホスト名（IP）とポート番号を指定

	if (tcpStream.fail()) //サーバが応答しない場合
		return false; //失敗

	std::string strBoundary = "xYzZY";

	//ヘッダ生成
	std::ostringstream osHeader;
	{
		//バージョン識別情報
		osHeader << "--" << strBoundary << "\r\n";
		osHeader << "Content-Disposition: form-data; name=\"version\"\r\n\r\n";
		osHeader << "0.01";
		osHeader << "\r\n";

		//ファイル本体情報
		osHeader << "--" << strBoundary << "\r\n";
		osHeader << "Content-Disposition: form-data; name=\"upload_file\"; filename=\"";
		osHeader << "TestFile.txt";	//ファイル名
		osHeader << "\"\r\n";
		osHeader << "Content-Type: application/octet-stream\r\n";
		osHeader << "\r\n";
	}

	//フッタ生成
	std::ostringstream osHooter;
	osHooter << "--" << strBoundary << "--";

	//送信
	tcpStream << "POST /cgi-bin/physius/upload.cgi HTTP/1.1\r\n";
	tcpStream << "Host: " << strHost << "\r\n"; //ホスト名
	tcpStream << "Content-Length:";
	tcpStream << std::dec << Size + osHeader.str().size() + osHooter.str().size(); //ヘッダ+フッタ+ファイル本体のサイズ
	tcpStream << "\r\n";
	tcpStream << "Connection: Keep-Alive\r\n";
	tcpStream << "Cache-Control: no-cache\r\n";
	tcpStream << "Content-Type: multipart/form-data; boundary=" << strBoundary << "\r\n";
	tcpStream << "\r\n";

	tcpStream << osHeader.str(); //ヘッダ送信

	tcpStream.write((char *)pData, Size); //データ出力	

	tcpStream << osHooter.str(); //フッタ送信

	tcpStream.flush(); //バッファに溜めない

	//サーバから送られてきたデータを受信し、デバッガに出力する
	std::string result;
	while (getline(tcpStream, result))
	{
		ho::tstring str = ho::CharToWChar(result.c_str());
		OutputDebugString(str.c_str());
		OutputDebugString(TEXT("\n"));
	}

	return true;
}

//ファイルを送信する
bool ho::CGI_Upload::Upload(const ho::tstring &strFilename, const std::string &strVer) 
{
	try
	{
		boost::asio::ip::tcp::iostream tcpStream(this->strHost, "http"); //ホスト名（IP）とポート番号を指定
		tcpStream.expires_from_now(boost::posix_time::seconds(4)); //タイムアウトの設定

		if (tcpStream.fail()) //サーバが応答しない場合
			throw ho::Exception(TEXT("サーバが応答しませんでした。"), __WFILE__, __LINE__, true);

		HANDLE hFile = CreateFile(strFilename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) //ファイルが開けなかった場合
		{
			ho::tstring str = TEXT("ファイルが開けませんでした。");
			throw ho::Exception(str, __WFILE__, __LINE__, true);
		}

		const int Size = (int)GetFileSize(hFile, NULL); //ファイルサイズ取得

		if (!CloseHandle(hFile)) //ファイルが閉じれなかった場合
		{
			ho::tstring str = strFilename + TEXT(" を閉じれませんでした。");
			throw ho::Exception(str, __WFILE__, __LINE__, true);
		}

		OutputDebugString(TEXT("Begin send file...\n"));

		std::ifstream ifs(strFilename.c_str());

		std::string strBoundary = "xYzZY00000000000654684605406840888888888888888888888888";

		//ヘッダ生成
		std::ostringstream osHeader;
		{
			//バージョン識別情報
			osHeader << "--" << strBoundary << "\r\n";
			osHeader << "Content-Disposition: form-data; name=\"version\"\r\n\r\n";
			osHeader << strVer;
			osHeader << "\r\n";

			//ファイル本体情報
			osHeader << "--" << strBoundary << "\r\n";
			osHeader << "Content-Disposition: form-data; name=\"upload_file\"; filename=\"";
			std::string strFilename2 = ho::WCharToChar(strFilename.c_str()); //ワイド文字(TCHAR)をマルチバイト文字(char)に変換
			osHeader << strFilename2; //ファイル名
			osHeader << "\"\r\n";
			osHeader << "Content-Type: application/octet-stream\r\n";
			osHeader << "\r\n";
		}

		//フッタ生成
		std::ostringstream osHooter;
		osHooter << "--" << strBoundary << "--";

		//送信
		tcpStream << "POST /cgi-bin/physius/upload.cgi HTTP/1.1\r\n";
		tcpStream << "Host: " << strHost << "\r\n"; //ホスト名
		tcpStream << "Content-Length:";
		tcpStream << std::dec << Size + osHeader.str().size() + osHooter.str().size(); //ヘッダ+フッタ+ファイル本体のサイズ
		tcpStream << "\r\n";
		tcpStream << "Connection: Keep-Alive\r\n";
		tcpStream << "Cache-Control: no-cache\r\n";
		tcpStream << "Content-Type: multipart/form-data; boundary=" << strBoundary << "\r\n";
		tcpStream << "\r\n";

		tcpStream << osHeader.str(); //ヘッダ送信

		//ファイルストリームを tcp ストリームに送信
		char Buffer[256];
		int ReadSize = 0;
		while (!ifs.eof())
		{
			ifs.read(Buffer, sizeof(Buffer));
			tcpStream.write(Buffer, ifs.gcount());
			tcpStream.flush(); //バッファに溜めない
		}

		tcpStream << osHooter.str(); //フッタ送信

		tcpStream.flush(); //バッファに溜めない

		OutputDebugString(TEXT("Receive Message from Server...\n"));

		//サーバから送られてきたデータを受信し、デバッガに出力する
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
