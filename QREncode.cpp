// QREncode.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "QREncoder.h"

int _tmain(int argc, _TCHAR* argv[])
{
	QREncoder encoder("http://www.baidu.com", 300, 10);

	encoder.load_logo(FIF_BMP, L".\\logo.bmp");

	encoder.output(FIF_BMP, L"E:\\test1.bmp", true);

	return 0;
}

