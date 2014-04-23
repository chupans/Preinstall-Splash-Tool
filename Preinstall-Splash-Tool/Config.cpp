#include "Config.h"
#include <fstream>
#include <mxml.h>
#include <mxml_document.h>

using namespace std;
using namespace MXML;

CFont* NewFont(const CString& typeface, int size, bool is_bold,	bool is_italic)
{
	CWindowDC dc(CWnd::FromHandle(GetDesktopWindow()));
	int pixels_per_inch = dc.GetDeviceCaps(LOGPIXELSY);
	int font_height = -MulDiv(size, pixels_per_inch, 72);
	CFont* font = new CFont();
	font->CreateFont(font_height, 0, 0, 0, is_bold ? FW_BOLD : FW_NORMAL, is_italic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, typeface);
	return font;
}

CFont* ReadFont(const Node::find_iterator& node)
{
	CString typeface = node->getAttribute("font").c_str();
	int font_size = atoi(node->getAttribute("fontSize").c_str());
	bool is_bold = false;
	bool is_italic = false;
	if (node->getAttribute("fontWeight") == "Bold") is_bold = true;
	else if (node->getAttribute("fontWeight") == "Italic") is_italic = true;
	return NewFont(typeface, font_size, is_bold, is_italic);
}

CXMLConfig::CXMLConfig()
{
	Init();
}

CXMLConfig::~CXMLConfig()
{
	//delete header_font;
	//delete sub_header_font;
}

CXMLConfig::CXMLConfig(const CString& xml_file_name)
{
	Init();
	ifstream stream((LPCWSTR)xml_file_name);
	if (!stream.is_open()) throw CString(L"XML configuration file not found.");
	try
	{
		Document doc(stream);
		Node::find_iterator params_node = doc.root()->find("params");

		Node::find_iterator header_node = params_node->find("header");
		header = header_node->getAttribute("text").c_str();
		header_font = ReadFont(header_node);

		Node::find_iterator sub_header_node = params_node->find("subHeader");
		sub_header = sub_header_node->getAttribute("text").c_str();
		sub_header_font = ReadFont(sub_header_node);

		background_file_name = params_node->find("backgroundImage")->data().c_str();
		button_icon_file_name = params_node->find("buttonIcon")->data().c_str();

		Node::find_iterator window_size_node = params_node->find("windowSize");
		if (window_size_node != params_node->end()) min_width = atoi(window_size_node->getAttribute("minWidth").c_str());
		if (min_width == 0) min_width = 300;
		if (min_height == 0) min_height = 200;
		if (window_size_node != params_node->end()) min_height = atoi(window_size_node->getAttribute("minHeight").c_str());
		if (window_size_node != params_node->end()) resize_by_context = window_size_node->getAttribute("resizeByContent") == "Y";

		Node::find_iterator items_node = params_node->find("items");
		for (Node::iterator i = items_node->begin(); i != items_node->end(); i++)
		{
			Item item; 
			item.description = i->getAttribute("descr").c_str();
			item.path = i->getAttribute("path").c_str();
			item.close = (i->getAttribute("close") == "Y");
			items.push_back(item);
		}

		Node::find_iterator caption_node = params_node->find("windowCaption");
		if (caption_node != params_node->end()) caption = caption_node->data().c_str();
	}
	catch (...)
	{
		throw CString(L"Can not read XML configuration file.");
	}
}

void CXMLConfig::Init()
{
	header_font = sub_header_font = NULL;
	min_width = 0;
	min_height = 0;
	resize_by_context = true;
}