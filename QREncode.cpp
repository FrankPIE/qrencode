// QREncode.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "QREncoder.h"

int _tmain(int argc, _TCHAR* argv[])
{
	QREncoder encoder("http://124.207.5.194:7000/panorama/vistandard/9c577624-3880-4d02-ad4-70911c22bc71/panorama.html");

	encoder.load_logo(FIF_BMP, L".\\logo.bmp");

	encoder.output(FIF_BMP, L"E:\\test1.bmp", 200);

	return 0;
}

