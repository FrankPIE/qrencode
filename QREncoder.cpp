#include "stdafx.h"
#include "QREncoder.h"

#include <qrencode.h>

QREncoder::QREncoder(const std::string& str, const RGBQUAD& clr_background /*= detail::WHITE*/, const RGBQUAD& clr_foreground /*= detail::BLACK*/)
		: m_clr_background_(clr_background), m_clr_foreground_(clr_foreground)
{
	generate(str);
}

QREncoder::~QREncoder()
{ }

void QREncoder::generate(const std::string& str)
{
	auto qrcode = QRcode_encodeString8bit(str.c_str(), 0, QR_ECLEVEL_H);

	auto real_width = qrcode->width + 2;

	m_qr_bitmap_ 
		= std::shared_ptr<FIBITMAP>(FreeImage_Allocate(real_width, real_width, 32), [&](FIBITMAP *ptr) { FreeImage_Unload(ptr); });

	FreeImage_FillBackground(m_qr_bitmap_.get(), &m_clr_background_);

	for (auto y = 0; y < qrcode->width; ++y)
	{
		auto r_y = qrcode->width - y;

		for (auto x = 0; x < qrcode->width; ++x)
		{
			auto b = qrcode->data[y * qrcode->width + x];

			if (b & 0x01)
			{
				FreeImage_SetPixelColor(m_qr_bitmap_.get(), x + 1, r_y, &m_clr_foreground_);
			}
		}
	}

	QRcode_free(qrcode);
}

void QREncoder::load_logo(const FREE_IMAGE_FORMAT type, const std::wstring& logo_file)
{
	m_logo_bitmap_ = std::shared_ptr<FIBITMAP>(FreeImage_LoadU(type, logo_file.c_str()), [&](FIBITMAP *ptr) { FreeImage_Unload(ptr); });
}

void QREncoder::resize(const uint32_t size)
{
	if (size > 0)
	{
		m_qr_bitmap_ = std::shared_ptr<FIBITMAP>(FreeImage_Rescale(m_qr_bitmap_.get(), size, size, FILTER_BOX), [&](FIBITMAP *ptr) { FreeImage_Unload(ptr); });

		if (m_logo_bitmap_)
		{
			m_logo_bitmap_ = std::shared_ptr<FIBITMAP>(FreeImage_Rescale(m_logo_bitmap_.get(), size / 6, size / 6, FILTER_BILINEAR), [&](FIBITMAP *ptr) { FreeImage_Unload(ptr); });
		}
	}
}

void QREncoder::output(const FREE_IMAGE_FORMAT type, const std::wstring& file_name, const uint32_t size, bool logo /*= true*/)
{
	resize(size);

	if (logo && m_logo_bitmap_)
	{
		auto logo_size = size / 6;
		auto half_logo_size = logo_size / 2;

		FreeImage_Paste(m_qr_bitmap_.get(), m_logo_bitmap_.get(), size / 2 - half_logo_size, size / 2 - half_logo_size, 255);
	}

	FreeImage_SaveU(type, m_qr_bitmap_.get(), file_name.c_str());
}
