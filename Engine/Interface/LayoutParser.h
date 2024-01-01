#pragma once

#include <string_view>
#include <filesystem>
#include <map>

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

#include <Engine/Xml/tinyxml2.h>

#define DECLARE_INTERFACE_NODE(TYPE, BASE) \
public: \
	static constexpr inline std::string_view NodeName = #TYPE; \
	static constexpr inline std::string_view BaseName = #BASE; \
public: \
	static inline Node* CreateInstance(Node* Parent) { return new TYPE{ Parent }; } \
public: \
	TYPE(Node* Parent) : BASE(Parent) {} \

namespace hyperion
{
	using CreateProc = Node* (*)(Node*);

	struct NodeInfo
	{
		std::string_view BaseName;
		CreateProc CreateProc;
	};

	extern std::map<std::string_view, NodeInfo> gNodeDeclarations;

	class LayoutParser
	{
	public:

		template<typename N>
		static void Register();

	public:

		static Node* Parse(std::filesystem::path const& File);

	private:

		static Node* ParseRecursive(tinyxml2::XMLElement* Element, Node* Parent, R32 Depth);

	private:

		static void ParseAttributeId(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeName(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeColor(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeBackgroundColor(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeHorizontalAlignment(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeVerticalAlignment(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeHorizontalOffset(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeVerticalOffset(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeWidth(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeHeight(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeRow(tinyxml2::XMLElement* Element, Node* Node);
		static void ParseAttributeColumn(tinyxml2::XMLElement* Element, Node* Node);

		static void ParseAttributeRows(tinyxml2::XMLElement* Element, GridPanel* GridPanel);
		static void ParseAttributeColumns(tinyxml2::XMLElement* Element, GridPanel* GridPanel);

		static void ParseAttributeOrientation(tinyxml2::XMLElement* Element, StackPanel* StackPanel);

		static void ParseAttributeFont(tinyxml2::XMLElement* Element, Text* Text);
		static void ParseAttributeFontSize(tinyxml2::XMLElement* Element, Text* Text);
		static void ParseAttributeText(tinyxml2::XMLElement* Element, Text* Text);
		static void ParseAttributeValue(tinyxml2::XMLElement* Element, Text* Text);
		static void ParseAttributeAlignment(tinyxml2::XMLElement* Element, Text* Text);

		static void ParseAttributeTimeSteps(tinyxml2::XMLElement* Element, Histogram* Histogram);
		static void ParseAttributeScale(tinyxml2::XMLElement* Element, Histogram* Histogram);
		static void ParseAttributeInterval(tinyxml2::XMLElement* Element, Histogram* Histogram);
		static void ParseAttributeValue(tinyxml2::XMLElement* Element, Histogram* Histogram);
	};
}

namespace hyperion
{
	template<typename N>
	static void LayoutParser::Register()
	{
		gNodeDeclarations[N::NodeName] = NodeInfo{ N::BaseName, N::CreateInstance };
	}
};