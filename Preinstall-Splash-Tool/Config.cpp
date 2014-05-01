#include "Config.h"
#include <fstream>

using namespace std;
using namespace MXML;

//CXMLConfig::Text

CXMLConfig::Text::Text()
{
	width = height = 0;
	font = NULL;
}

void CXMLConfig::Text::Load(const Node& source)
{
	text = source.getAttribute("text").c_str();
	this->font = LoadFont(source);
	CSize size = CountTextSize(font, text);
	width = size.cx;
	height = size.cy;
}

void CXMLConfig::Text::Draw(CDC& dc, CPoint pos)
{
	dc.SelectObject(font);
	dc.TextOutW(pos.x, pos.y, text);
}

CFont* CXMLConfig::Text::NewFont(const CString& typeface, int size, bool is_bold, bool is_italic)
{
	static CWindowDC dc(CWnd::FromHandle(GetDesktopWindow()));
	int pixels_per_inch = dc.GetDeviceCaps(LOGPIXELSY);
	int font_height = -MulDiv(size, pixels_per_inch, 72);
	CFont* font = new CFont();
	font->CreateFont(font_height, 0, 0, 0, is_bold ? FW_BOLD : FW_NORMAL, is_italic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, typeface);
	return font;
}

CFont* CXMLConfig::Text::LoadFont(const Node& source)
{
	CString typeface = source.getAttribute("font").c_str();
	int font_size = atoi(source.getAttribute("fontSize").c_str());
	bool is_bold = false;
	bool is_italic = false;
	if (source.getAttribute("fontWeight") == "Bold") is_bold = true;
	else if (source.getAttribute("fontWeight") == "Italic") is_italic = true;
	return NewFont(typeface, font_size, is_bold, is_italic);
}

CSize CXMLConfig::Text::CountTextSize(CFont* font, const CString& str)
{
	static CWindowDC dc(CWnd::FromHandle(::GetDesktopWindow()));
	CFont* old = dc.SelectObject(font);
	CSize size = dc.GetTextExtent(str);
	dc.SelectObject(old);
	return size;
}

//CXMLConfig::Item

CXMLConfig::Item::Item()
{
	close = false;
}

void CXMLConfig::Item::Load(const Node& source, CFont* font)
{
	description.text = source.getAttribute("descr").c_str();
	description.font = font;
	CSize size = Text::CountTextSize(font, description.text);
	description.width = size.cx;
	description.height = size.cy;

	path = source.getAttribute("path").c_str();
	close = (source.getAttribute("close") == "Y");
}

//CXMLConfig

void CXMLConfig::LoadButtonIcon(CImage& image, const CString& file_name, int size)
{
	CImage t;
	t.Load(file_name);
	image.Create(size, size, 32);
	HDC dc = image.GetDC();
	t.Draw(dc, CRect(0, 0, size, size));
	image.ReleaseDC();
}

CXMLConfig::CXMLConfig(const CString& xml_file_name)
{
	ifstream stream((LPCWSTR)xml_file_name);
	if (!stream.is_open()) throw CString(L"XML configuration file not found.");
	try
	{
		Document doc(stream);
		Node& params_node = *doc.root()->find("params");

		header.Load(*params_node.find("header"));
		sub_header.Load(*params_node.find("subHeader"));

		button_font = sub_header.font;

		CString background_file_name = params_node.find("backgroundImage")->data().c_str();
		CString button_file_name = params_node.find("buttonIcon")->data().c_str();

		background.Load(background_file_name);
		//button.Load(button_file_name);
		LoadButtonIcon(button, button_file_name, sub_header.height);

		if (background.IsNull()) throw CString(L"Background image file not found.");
		if (button.IsNull()) throw CString(L"Button icon file not found.");

		Node& window_size_node = *params_node.find("windowSize");
		min_wnd_size.cx = atoi(window_size_node.getAttribute("minWidth").c_str());
		min_wnd_size.cy = atoi(window_size_node.getAttribute("minHeight").c_str());
		resize_by_content = window_size_node.getAttribute("resizeByContent") == "Y";

		Node& items_node = *params_node.find("items");
		for (Node::iterator i = items_node.begin(); i != items_node.end(); i++)
		{
			Item item; 
			item.Load(*i, button_font);
			items.push_back(item);
		}

		CountMaxWndSize();
	}
	catch (const CString& exception) { throw exception; }
	catch (...)
	{
		throw CString(L"Can not read XML configuration file.");
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void CXMLConfig::CountDeltas()
{
	border_button_delta_x = GetButtonSize().cx;
	button_text_delta_x = border_button_delta_x / 2;
	button_button_delta_y = button_text_delta_x;
	subheader_button_delta_y = button_text_delta_x;
	border_header_delta_y = 5;
	header_subheader_delta_y = 5;
}

void CXMLConfig::CountMaxWndSize()
{
	CountDeltas();
	max_wnd_size.cx = min_wnd_size.cx;
	max_wnd_size.cy = max(min_wnd_size.cy, GetButtonRect(items.size()).bottom + button_button_delta_y);

	max_wnd_size.cx = max(max_wnd_size.cx, header.width + 2 * border_button_delta_x);
	max_wnd_size.cx = max(max_wnd_size.cx, sub_header.width + 2 * border_button_delta_x);
	for (UINT i = 0; i < items.size(); i++)
	{
		max_wnd_size.cx = max(max_wnd_size.cx, GetButtonTextPosition(i).x + items[i].description.width + button_text_delta_x);
	}

	if (max_wnd_size.cx > min_wnd_size.cx) max_wnd_size.cx += GetSystemMetrics(SM_CYVSCROLL);
	if (max_wnd_size.cy > min_wnd_size.cy) max_wnd_size.cy += GetSystemMetrics(SM_CXVSCROLL);
}

CString CXMLConfig::GetCaption()
{
	return header.text;
}

CPoint CXMLConfig::GetHeaderPosition()
{
	CPoint pos;
	pos.y = border_header_delta_y;

	pos.x = (max_wnd_size.cx - GetSystemMetrics(SM_CYVSCROLL) - 3 - header.width) / 2;
	return pos;
}

CPoint CXMLConfig::GetSubHeaderPosition()
{
	CPoint pos = GetHeaderPosition();
	pos.x = (max_wnd_size.cx - GetSystemMetrics(SM_CYVSCROLL) - 3 - sub_header.width) / 2;
	pos.y += header.height + header_subheader_delta_y;
	return pos;
}

CSize CXMLConfig::GetButtonSize()
{
	return CSize(button.GetWidth(), button.GetHeight());
}

CRect CXMLConfig::GetButtonRect(int button_number)
{
	int y = GetSubHeaderPosition().y + sub_header.height + subheader_button_delta_y;
	y += button_number * (GetButtonSize().cy + button_button_delta_y);
	CRect rect;
	rect.left = border_button_delta_x;
	rect.right = rect.left + GetButtonSize().cx;
	rect.top = y;
	rect.bottom = rect.top + GetButtonSize().cy;
	return rect;
}

CPoint CXMLConfig::GetButtonTextPosition(int button_number)
{
	CRect button_rect = GetButtonRect(button_number);
	CPoint pos;
	pos.y = button_rect.top;
	pos.x = button_rect.right + button_text_delta_x;
	return pos;
}