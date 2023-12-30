#include <Common/Macros.h>

#include <Engine/Scene.h>

#include <Interface/LayoutParser.h>
#include <Interface/Node.h>

#include <Interface/Components/GridPanel.h>
#include <Interface/Components/StackPanel.h>
#include <Interface/Components/Button.h>
#include <Interface/Components/Text.h>
#include <Interface/Components/Histogram.h>

#include <Vulkan/Window.h>

namespace interface
{
	std::map<std::string_view, NodeInfo> gNodeDeclarations = {};

	Node* LayoutParser::Parse(Scene* Scene, std::filesystem::path const& File)
	{
		XMLDocument document = {};

		XMLError result = document.LoadFile(File.string().data());

		if (result == XML_SUCCESS)
		{
			XMLElement* element = document.FirstChildElement();

			return ParseRecursive(element, Scene, 0, 0.0F);
		}

		return 0;
	}

	Node* LayoutParser::ParseRecursive(XMLElement* Element, Scene* Scene, Node* Parent, R32 Depth)
	{
		Node* node = 0;

		for (auto const& [nodeName, nodeInfo] : gNodeDeclarations)
		{
			if (std::strcmp(nodeName.data(), Element->Name()) == 0)
			{
				if (std::strcmp("GridPanel", nodeInfo.BaseName.data()) == 0)
				{
					GridPanel* gridPanel = (GridPanel*)nodeInfo.CreateProc(Parent);

					ParseAttributeRows(Element, gridPanel);
					ParseAttributeColumns(Element, gridPanel);

					node = gridPanel;
				}
				else if (std::strcmp("StackPanel", nodeInfo.BaseName.data()) == 0)
				{
					StackPanel* stackPanel = (StackPanel*)nodeInfo.CreateProc(Parent);

					ParseAttributeOrientation(Element, stackPanel);

					node = stackPanel;
				}
				else if (std::strcmp("Button", nodeInfo.BaseName.data()) == 0)
				{
					Button* button = (Button*)nodeInfo.CreateProc(Parent);

					node = button;
				}
				else if (std::strcmp("Text", nodeInfo.BaseName.data()) == 0)
				{
					Text* text = (Text*)nodeInfo.CreateProc(Parent);

					ParseAttributeFont(Element, text);
					ParseAttributeFontSize(Element, text);
					ParseAttributeText(Element, text);
					ParseAttributeValue(Element, text);
					ParseAttributeAlignment(Element, text);

					node = text;
				}
				else if (std::strcmp("Histogram", nodeInfo.BaseName.data()) == 0)
				{
					Histogram* histogram = (Histogram*)nodeInfo.CreateProc(Parent);

					ParseAttributeTimeSteps(Element, histogram);
					ParseAttributeScale(Element, histogram);
					ParseAttributeInterval(Element, histogram);
					ParseAttributeValue(Element, histogram);

					node = histogram;
				}
				else
				{
					node = nodeInfo.CreateProc(Parent);
				}

				break;
			}
		}

		if (!node)
		{
			if (std::strcmp("GridPanel", Element->Name()) == 0)
			{
				GridPanel* gridPanel = new GridPanel{ Parent };

				ParseAttributeRows(Element, gridPanel);
				ParseAttributeColumns(Element, gridPanel);

				node = gridPanel;
			}
			else if (std::strcmp("StackPanel", Element->Name()) == 0)
			{
				StackPanel* stackPanel = new StackPanel{ Parent };

				ParseAttributeOrientation(Element, stackPanel);

				node = stackPanel;
			}
			else if (std::strcmp("Button", Element->Name()) == 0)
			{
				Button* button = new Button{ Parent };

				node = button;
			}
			else if (std::strcmp("Text", Element->Name()) == 0)
			{
				Text* text = new Text{ Parent };

				ParseAttributeFont(Element, text);
				ParseAttributeFontSize(Element, text);
				ParseAttributeText(Element, text);
				ParseAttributeValue(Element, text);
				ParseAttributeAlignment(Element, text);

				node = text;
			}
			else if (std::strcmp("Histogram", Element->Name()) == 0)
			{
				Histogram* histogram = new Histogram{ Parent };

				ParseAttributeTimeSteps(Element, histogram);
				ParseAttributeScale(Element, histogram);
				ParseAttributeInterval(Element, histogram);
				ParseAttributeValue(Element, histogram);

				node = histogram;
			}
			else
			{
				node = new Node{ Parent };
			}
		}

		if (node)
		{
			node->SetPositionZ(1.0F - Depth);

			ParseAttributeId(Element, node);
			ParseAttributeName(Element, node);
			ParseAttributeColor(Element, node);
			ParseAttributeBackgroundColor(Element, node);
			ParseAttributeHorizontalAlignment(Element, node);
			ParseAttributeVerticalAlignment(Element, node);
			ParseAttributeHorizontalOffset(Element, node);
			ParseAttributeVerticalOffset(Element, node);
			ParseAttributeWidth(Element, node);
			ParseAttributeHeight(Element, node);
			ParseAttributeRow(Element, node);
			ParseAttributeColumn(Element, node);

			if (Parent)
			{
				Parent->AddChild(node);
			}
		}

		for (XMLElement* child = Element->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			ParseRecursive(child, Scene, node, Depth + 0.01F);
		}

		return node;
	}

	void LayoutParser::ParseAttributeId(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Id"))
		{
			Node->SetId(attribute->Value());
		}
	}

	void LayoutParser::ParseAttributeName(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Name"))
		{
			Node->SetName(attribute->Value());
		}
		else
		{
			Node->SetName(Element->Value());
		}
	}

	void LayoutParser::ParseAttributeColor(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Color"))
		{
			Node->SetColor(std::stoul(attribute->Value(), 0, 16));
		}
	}
	
	void LayoutParser::ParseAttributeBackgroundColor(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("BackgroundColor"))
		{
			Node->SetBackgroundColor(std::stoul(attribute->Value(), 0, 16));
		}
	}

	void LayoutParser::ParseAttributeHorizontalAlignment(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("HorizontalAlignment"))
		{
			if (std::strcmp("Center", attribute->Value()) == 0)
			{
				Node->SetHorizontalAlignment(NodeAlignmentMode::Center);
			}
			else if (std::strcmp("Top", attribute->Value()) == 0)
			{
				Node->SetHorizontalAlignment(NodeAlignmentMode::Top);
			}
			else if (std::strcmp("Bottom", attribute->Value()) == 0)
			{
				Node->SetHorizontalAlignment(NodeAlignmentMode::Bottom);
			}
			else if (std::strcmp("Left", attribute->Value()) == 0)
			{
				Node->SetHorizontalAlignment(NodeAlignmentMode::Left);
			}
			else if (std::strcmp("Right", attribute->Value()) == 0)
			{
				Node->SetHorizontalAlignment(NodeAlignmentMode::Right);
			}
		}
	}

	void LayoutParser::ParseAttributeVerticalAlignment(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("VerticalAlignment"))
		{
			if (std::strcmp("Center", attribute->Value()) == 0)
			{
				Node->SetVerticalAlignment(NodeAlignmentMode::Center);
			}
			else if (std::strcmp("Top", attribute->Value()) == 0)
			{
				Node->SetVerticalAlignment(NodeAlignmentMode::Top);
			}
			else if (std::strcmp("Bottom", attribute->Value()) == 0)
			{
				Node->SetVerticalAlignment(NodeAlignmentMode::Bottom);
			}
			else if (std::strcmp("Left", attribute->Value()) == 0)
			{
				Node->SetVerticalAlignment(NodeAlignmentMode::Left);
			}
			else if (std::strcmp("Right", attribute->Value()) == 0)
			{
				Node->SetVerticalAlignment(NodeAlignmentMode::Right);
			}
		}
	}

	void LayoutParser::ParseAttributeHorizontalOffset(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("HorizontalOffset"))
		{
			Node->SetHorizontalOffset(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeVerticalOffset(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("VerticalOffset"))
		{
			Node->SetVerticalOffset(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeWidth(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Width"))
		{
			Node->SetWidth(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeHeight(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Height"))
		{
			Node->SetHeight(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeRow(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Row"))
		{
			Node->SetRow(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeColumn(XMLElement* Element, Node* Node)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Column"))
		{
			Node->SetColumn(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeRows(XMLElement* Element, GridPanel* GridPanel)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Rows"))
		{
			GridPanel->SetRows(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeColumns(XMLElement* Element, GridPanel* GridPanel)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Columns"))
		{
			GridPanel->SetColumns(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeOrientation(XMLElement* Element, StackPanel* StackPanel)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Orientation"))
		{
			if (std::strcmp("Horizontal", attribute->Value()) == 0)
			{
				StackPanel->SetOrientation(StackOrientationMode::Horizontal);
			}
			else if (std::strcmp("Vertical", attribute->Value()) == 0)
			{
				StackPanel->SetOrientation(StackOrientationMode::Vertical);
			}
		}
	}

	void LayoutParser::ParseAttributeFont(XMLElement* Element, Text* Text)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Font"))
		{
			Text->SetFont(attribute->Value());
		}
	}

	void LayoutParser::ParseAttributeFontSize(XMLElement* Element, Text* Text)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("FontSize"))
		{
			Text->SetFontSize(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeText(XMLElement* Element, Text* Text)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Text"))
		{
			Text->SetText(attribute->Value());
		}
	}

	void LayoutParser::ParseAttributeValue(XMLElement* Element, Text* Text)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Value"))
		{
			Text->SetValue(attribute->Value());
		}
	}

	void LayoutParser::ParseAttributeAlignment(XMLElement* Element, Text* Text)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Alignment"))
		{
			if (std::strcmp("Left", attribute->Value()) == 0)
			{
				Text->SetAlignment(TextAlignmentMode::Left);
			}
			else if (std::strcmp("Center", attribute->Value()) == 0)
			{
				Text->SetAlignment(TextAlignmentMode::Center);
			}
			else if (std::strcmp("Right", attribute->Value()) == 0)
			{
				Text->SetAlignment(TextAlignmentMode::Right);
			}
		}
	}

	void LayoutParser::ParseAttributeTimeSteps(XMLElement* Element, Histogram* Histogram)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("TimeSteps"))
		{
			Histogram->SetTimeSteps(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeScale(XMLElement* Element, Histogram* Histogram)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Scale"))
		{
			Histogram->SetScale(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeInterval(XMLElement* Element, Histogram* Histogram)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Interval"))
		{
			Histogram->SetInterval(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeValue(XMLElement* Element, Histogram* Histogram)
	{
		if (XMLAttribute const* attribute = Element->FindAttribute("Value"))
		{
			Histogram->SetValueName(attribute->Value());
		}
	}
}