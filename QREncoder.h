#ifndef _QR_ENCODER_H_
#define _QR_ENCODER_H_

#include <string>
#include <memory>

#include <FreeImage.h>

namespace detail
{
	static const RGBQUAD BLACK = {  0,   0,   0,   0  };
	static const RGBQUAD WHITE = { 255, 255, 255, 255 };
}

class QREncoder
{
public:
	typedef std::shared_ptr<FIBITMAP>   QRBitmap;

public:
	QREncoder(const std::string& str, const uint32_t size, const uint32_t marign, const RGBQUAD& clr_background = detail::WHITE, const RGBQUAD& clr_foreground = detail::BLACK);
	~QREncoder();

public:
	void output(const FREE_IMAGE_FORMAT type, const std::wstring& file_name, bool logo = true);

	void load_logo(const FREE_IMAGE_FORMAT type, const std::wstring& logo_file);

	QRBitmap& get_img_ptr() { return m_qr_bitmap_; }

private:
	void generate(const std::string& str, const uint32_t size, const uint32_t marign);

	void fill_region(const float left, const float top, const float right, const float bottom);

private:
	QRBitmap		  m_qr_bitmap_;
	QRBitmap		  m_logo_bitmap_;
	RGBQUAD			  m_clr_background_;
	RGBQUAD			  m_clr_foreground_;
};

#endif

