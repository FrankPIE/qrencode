#include "stdafx.h"
#include "QREncoder.h"

#include <qrencode.h>

QREncoder::QREncoder(const std::string& str, const uint32_t size, const uint32_t marign, const RGBQUAD& clr_background /*= detail::WHITE*/, const RGBQUAD& clr_foreground /*= detail::BLACK*/)
		: m_clr_background_(clr_background), m_clr_foreground_(clr_foreground)
{
	generate(str, size, marign);
}

QREncoder::~QREncoder()
{ }

void QREncoder::output( const FREE_IMAGE_FORMAT type, const std::wstring& file_name, bool logo )
{
	if (logo && m_logo_bitmap_)
	{
		auto size = FreeImage_GetWidth(m_qr_bitmap_.get());

		m_logo_bitmap_ = std::shared_ptr<FIBITMAP>(FreeImage_Rescale(m_logo_bitmap_.get(), size / 6, size / 6, FILTER_BILINEAR), [&](FIBITMAP *ptr) { FreeImage_Unload(ptr); });

		auto logo_size = size / 6;
		auto half_logo_size = logo_size / 2;

		FreeImage_Paste(m_qr_bitmap_.get(), m_logo_bitmap_.get(), size / 2 - half_logo_size, size / 2 - half_logo_size, 256);
	}

	FreeImage_SaveU(type, m_qr_bitmap_.get(), file_name.c_str());
}

void QREncoder::generate(const std::string& str, const uint32_t size, const uint32_t marign)
{
	auto qrcode_ptr = std::shared_ptr<QRcode>(QRcode_encodeString8bit(str.c_str(), 0, QR_ECLEVEL_H), [&](QRcode* ptr) { QRcode_free(ptr); });

	m_qr_bitmap_
		= std::shared_ptr<FIBITMAP>(FreeImage_Allocate(size, size, 32), [&](FIBITMAP *ptr) { FreeImage_Unload(ptr); });

	FreeImage_FillBackground(m_qr_bitmap_.get(), &m_clr_background_);

	auto mulriple = static_cast<float>(size - marign * 2) / static_cast<float>(qrcode_ptr->width);

	for (auto y = 0; y < qrcode_ptr->width; ++y)
	{
		auto dy = qrcode_ptr->width - y - 1;

		for (auto x = 0; x < qrcode_ptr->width; ++x)
		{
			auto b = qrcode_ptr->data[y * qrcode_ptr->width + x];

			if (b & 0x01)
			{
				fill_region(x * mulriple + marign, dy * mulriple + marign, (x + 1) * mulriple + marign, (dy + 1) * mulriple + marign);
			}
		}
	}
}

void QREncoder::load_logo(const FREE_IMAGE_FORMAT type, const std::wstring& logo_file)
{
	m_logo_bitmap_ = std::shared_ptr<FIBITMAP>(FreeImage_LoadU(type, logo_file.c_str()), [&](FIBITMAP *ptr) { FreeImage_Unload(ptr); });
}

void QREncoder::fill_region(const float left, const float top, const float right, const float bottom)
{
	if (m_qr_bitmap_)
	{
		auto width  = static_cast<float>(FreeImage_GetWidth(m_qr_bitmap_.get()));
		auto height = static_cast<float>(FreeImage_GetHeight(m_qr_bitmap_.get()));

		for (auto y = top; y < bottom; ++y)
		{
			if (y < 0.f && y > height) 
			{
				continue;
			}

			for (auto x = left; x < right; ++x)
			{
				if (x < 0.f && x > width)
				{
					continue;
				}

				FreeImage_SetPixelColor(m_qr_bitmap_.get(), static_cast<uint32_t>(x), static_cast<uint32_t>(y), &m_clr_foreground_);
			}
		}
	}
}
