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
	font = LoadFont(source);
	CSize size = CountTextSize(font, text);
	width = size.cx;
	height = size.cy;
}

void CXMLConfig::Text::Draw(CDC& dc, int x, int y)
{
	dc.SelectObject(font);
	dc.TextOutW(x, y, text);
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

CXMLConfig::CXMLConfig()
{
	Init();
}

CXMLConfig::CXMLConfig(const CString& xml_file_name)
{
	Init();
	ifstream stream((LPCWSTR)xml_file_name);
	if (!stream.is_open()) throw CString(L"XML configuration file not found.");
	try
	{
		Document doc(stream);
		Node& params_node = *doc.root()->find("params");

		header.Load(*params_node.find("header"));
		sub_header.Load(*params_node.find("subHeader"));

		CString background_file_name = params_node.find("backgroundImage")->data().c_str();
		CString button_file_name = params_node.find("buttonIcon")->data().c_str();

		background.Load(background_file_name);
		button.Load(button_file_name);

		if (button.IsNull()) throw CString(L"Button icon file not found.");

		button_font = Text::NewFont(params_node.find("header")->getAttribute("font").c_str(), GetButtonHeight() / 2);

		Node& window_size_node = *params_node.find("windowSize");
		min_width = atoi(window_size_node.getAttribute("minWidth").c_str());
		min_height = atoi(window_size_node.getAttribute("minHeight").c_str());
		resize_by_content = window_size_node.getAttribute("resizeByContent") == "Y";

		Node& items_node = *params_node.find("items");
		for (Node::iterator i = items_node.begin(); i != items_node.end(); i++)
		{
			Item item; 
			item.Load(*i, button_font);
			items.push_back(item);
		}

		CountWndSize();
	}
	catch (const CString& exception) { throw exception; }
	catch (...)
	{
		throw CString(L"Can not read XML configuration file.");
	}
}

void CXMLConfig::Init()
{
	max_width = max_height = min_width = min_height = 0;
	resize_by_content = true;
	button_font = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void CXMLConfig::CountDeltas()
{
	border_button_delta_x = GetButtonWidth();
	button_text_delta_x = border_button_delta_x / 2;
	button_button_delta_y = button_text_delta_x;
	subheader_button_delta_y = button_text_delta_x;
	border_header_delta_y = 5;
	header_subheader_delta_y = 5;
}

void CXMLConfig::CountWndSize()
{
	CountDeltas();

	max_width = min_width;
	max_height = max(min_height, GetButtonRect(items.size()).bottom);

	max_width = max(max_width, 2 * header.width);
	max_width = max(max_width, 2 * sub_header.width);
	for (UINT i = 0; i < items.size(); i++)
	{
		max_width = max(max_width, GetButtonTextPosition(i).x + items[i].description.width + button_text_delta_x);
	}

}

CString CXMLConfig::GetCaption()
{
	return header.text;
}

CPoint CXMLConfig::GetHeaderPosition()
{
	CPoint pos;
	pos.y = border_header_delta_y;
	pos.x = (max_width - header.width) / 2;
	return pos;
}

CPoint CXMLConfig::GetSubHeaderPosition()
{
	CPoint pos = GetHeaderPosition();
	pos.x = (max_width - sub_header.width) / 2;
	pos.y += header.height + header_subheader_delta_y;
	return pos;
}

int CXMLConfig::GetButtonWidth()
{
	return button.GetWidth();
}

int CXMLConfig::GetButtonHeight()
{
	return button.GetHeight();
}

CRect CXMLConfig::GetButtonRect(int button_number)
{
	int y = GetSubHeaderPosition().y + sub_header.height + subheader_button_delta_y;
	y += button_number * (GetButtonHeight() + button_button_delta_y);
	CRect rect;
	rect.left = border_button_delta_x;
	rect.right = rect.left + GetButtonWidth();
	rect.top = y;
	rect.bottom = rect.top + GetButtonHeight();
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