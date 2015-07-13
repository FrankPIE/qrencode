// QREncode.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>
#include <qrencode.h>
#include <FreeImage.h>

int _tmain(int argc, _TCHAR* argv[])
{
	auto qrcode = QRcode_encodeString("你们这些凡人", 0, QR_ECLEVEL_H, QR_MODE_8, 0);

	auto realwidth = qrcode->width + 2;

	auto bitmap = FreeImage_Allocate(realwidth, realwidth, 32); //白色背景

	auto clr   = RGB(255, 255, 255);

	FreeImage_FillBackground(bitmap, &clr);

	RGBQUAD clr_r = { 0   };
	RGBQUAD clr_f = { 255 };

	int yy;
	for (auto y = 0; y < qrcode->width; ++y)
	{
		yy = qrcode->width - y;

		for (auto x = 0; x < qrcode->width; ++x)
		{
			auto b = qrcode->data[y * qrcode->width + x];
			if (b & 0x01)
			{
				FreeImage_SetPixelColor(bitmap, x + 1, yy, &clr_r);
			}
		}
	}

	auto temp = FreeImage_Rescale(bitmap, 100, 100, FILTER_BOX);
	FreeImage_Unload(bitmap);

	auto half = 50;

	for (auto x = half - 5; x <= half + 5; ++x)
	{
		for (auto y = half - 5; y <= half + 5; ++y)
		{
			FreeImage_SetPixelColor(temp, x, y, &clr_f);
		}
	}

	FreeImage_Save(FIF_BMP, temp, "E:\\test.bmp");
	FreeImage_Unload(temp);
	QRcode_free(qrcode);

	return 0;
}

