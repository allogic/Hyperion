#include <Engine/Common/Macros.h>

#include <Engine/Interface/LayoutParser.h>
#include <Engine/Interface/Node.h>

#include <Engine/Interface/Nodes/GridPanel.h>
#include <Engine/Interface/Nodes/StackPanel.h>
#include <Engine/Interface/Nodes/Button.h>
#include <Engine/Interface/Nodes/Text.h>
#include <Engine/Interface/Nodes/Histogram.h>

#include <Engine/Platform/Window.h>

namespace hyperion
{
	std::map<std::string_view, NodeInfo> gNodeDeclarations = {};

	Node* LayoutParser::Parse(std::filesystem::path const& File)
	{
		tinyxml2::XMLDocument document = {};

		tinyxml2::XMLError result = document.LoadFile(File.string().data());

		if (result == tinyxml2::XML_SUCCESS)
		{
			tinyxml2::XMLElement* element = document.FirstChildElement();

			return ParseRecursive(element, 0, 0.0F);
		}

		return 0;
	}

	Node* LayoutParser::ParseRecursive(tinyxml2::XMLElement* Element, Node* Parent, R32 Depth)
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

		for (tinyxml2::XMLElement* child = Element->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			ParseRecursive(child, node, Depth + 0.01F);
		}

		return node;
	}

	void LayoutParser::ParseAttributeId(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Id"))
		{
			Node->SetId(attribute->Value());
		}
	}

	void LayoutParser::ParseAttributeName(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Name"))
		{
			Node->SetName(attribute->Value());
		}
		else
		{
			Node->SetName(Element->Value());
		}
	}

	void LayoutParser::ParseAttributeColor(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Color"))
		{
			Node->SetColor(std::stoul(attribute->Value(), 0, 16));
		}
	}
	
	void LayoutParser::ParseAttributeBackgroundColor(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("BackgroundColor"))
		{
			Node->SetBackgroundColor(std::stoul(attribute->Value(), 0, 16));
		}
	}

	void LayoutParser::ParseAttributeHorizontalAlignment(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("HorizontalAlignment"))
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

	void LayoutParser::ParseAttributeVerticalAlignment(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("VerticalAlignment"))
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

	void LayoutParser::ParseAttributeHorizontalOffset(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("HorizontalOffset"))
		{
			Node->SetHorizontalOffset(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeVerticalOffset(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("VerticalOffset"))
		{
			Node->SetVerticalOffset(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeWidth(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Width"))
		{
			Node->SetWidth(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeHeight(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Height"))
		{
			Node->SetHeight(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeRow(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Row"))
		{
			Node->SetRow(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeColumn(tinyxml2::XMLElement* Element, Node* Node)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Column"))
		{
			Node->SetColumn(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeRows(tinyxml2::XMLElement* Element, GridPanel* GridPanel)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Rows"))
		{
			GridPanel->SetRows(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeColumns(tinyxml2::XMLElement* Element, GridPanel* GridPanel)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Columns"))
		{
			GridPanel->SetColumns(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeOrientation(tinyxml2::XMLElement* Element, StackPanel* StackPanel)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Orientation"))
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

	void LayoutParser::ParseAttributeFont(tinyxml2::XMLElement* Element, Text* Text)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Font"))
		{
			Text->SetFont(attribute->Value());
		}
	}

	void LayoutParser::ParseAttributeFontSize(tinyxml2::XMLElement* Element, Text* Text)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("FontSize"))
		{
			Text->SetFontSize(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeText(tinyxml2::XMLElement* Element, Text* Text)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Text"))
		{
			Text->SetText(attribute->Value());
		}
	}

	void LayoutParser::ParseAttributeValue(tinyxml2::XMLElement* Element, Text* Text)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Value"))
		{
			Text->SetValue(attribute->Value());
		}
	}

	void LayoutParser::ParseAttributeAlignment(tinyxml2::XMLElement* Element, Text* Text)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Alignment"))
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

	void LayoutParser::ParseAttributeTimeSteps(tinyxml2::XMLElement* Element, Histogram* Histogram)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("TimeSteps"))
		{
			Histogram->SetTimeSteps(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeScale(tinyxml2::XMLElement* Element, Histogram* Histogram)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Scale"))
		{
			Histogram->SetScale(attribute->FloatValue());
		}
	}

	void LayoutParser::ParseAttributeInterval(tinyxml2::XMLElement* Element, Histogram* Histogram)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Interval"))
		{
			Histogram->SetInterval(attribute->IntValue());
		}
	}

	void LayoutParser::ParseAttributeValue(tinyxml2::XMLElement* Element, Histogram* Histogram)
	{
		if (tinyxml2::XMLAttribute const* attribute = Element->FindAttribute("Value"))
		{
			Histogram->SetValueName(attribute->Value());
		}
	}
}