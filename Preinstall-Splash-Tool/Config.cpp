#include "Config.h"
#include <fstream>
#include <mxml.h>
#include <mxml_document.h>

using namespace std;
using namespace MXML;

CXMLConfig::CXMLConfig(const CString& xml_file_name)
{
	ifstream stream((LPCWSTR)xml_file_name);
	if (!stream.is_open()) throw CString(L"XML configuration file not found.");
	try
	{
		Document doc(stream);
		Node::find_iterator params_node = doc.root()->find("params");

		Node::find_iterator header_node = params_node->find("header");
		header = header_node->getAttribute("text").c_str();

		Node::find_iterator sub_header_node = params_node->find("subHeader");
		sub_header = sub_header_node->getAttribute("text").c_str();

		background_file_name = params_node->find("backgroundImage")->data().c_str();
		button_icon_file_name = params_node->find("buttonIcon")->data().c_str();

		Node::find_iterator window_size_node = params_node->find("windowSize");
		min_width = atoi(window_size_node->getAttribute("minWidth").c_str());
		min_height = atoi(window_size_node->getAttribute("minHeight").c_str());
		resize_by_context = window_size_node->getAttribute("resizeByContent") == "Y";

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