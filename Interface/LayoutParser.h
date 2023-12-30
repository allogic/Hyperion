#pragma once

#include <string_view>
#include <filesystem>
#include <map>

#include <Common/Types.h>

#include <Engine/Forward.h>

#include <Interface/Forward.h>

#include <Interface/Xml/tinyxml2.h>

#include <Vulkan/Forward.h>

#define DECLARE_INTERFACE_NODE(TYPE, BASE) \
public: \
	static constexpr inline std::string_view NodeName = #TYPE; \
	static constexpr inline std::string_view BaseName = #BASE; \
public: \
	static inline Node* CreateInstance(Node* Parent) { return new TYPE{ Parent }; } \
public: \
	TYPE(Node* Parent) : BASE(Parent) {} \

namespace interface
{
	using namespace common;
	using namespace engine;
	using namespace vulkan;
	using namespace tinyxml2;

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

		static Node* Parse(Scene* Scene, std::filesystem::path const& File);

	private:

		static Node* ParseRecursive(XMLElement* Element, Scene* Scene, Node* Parent, R32 Depth);

	private:

		static void ParseAttributeId(XMLElement* Element, Node* Node);
		static void ParseAttributeName(XMLElement* Element, Node* Node);
		static void ParseAttributeColor(XMLElement* Element, Node* Node);
		static void ParseAttributeBackgroundColor(XMLElement* Element, Node* Node);
		static void ParseAttributeHorizontalAlignment(XMLElement* Element, Node* Node);
		static void ParseAttributeVerticalAlignment(XMLElement* Element, Node* Node);
		static void ParseAttributeHorizontalOffset(XMLElement* Element, Node* Node);
		static void ParseAttributeVerticalOffset(XMLElement* Element, Node* Node);
		static void ParseAttributeWidth(XMLElement* Element, Node* Node);
		static void ParseAttributeHeight(XMLElement* Element, Node* Node);
		static void ParseAttributeRow(XMLElement* Element, Node* Node);
		static void ParseAttributeColumn(XMLElement* Element, Node* Node);

		static void ParseAttributeRows(XMLElement* Element, GridPanel* GridPanel);
		static void ParseAttributeColumns(XMLElement* Element, GridPanel* GridPanel);

		static void ParseAttributeOrientation(XMLElement* Element, StackPanel* StackPanel);

		static void ParseAttributeFont(XMLElement* Element, Text* Text);
		static void ParseAttributeFontSize(XMLElement* Element, Text* Text);
		static void ParseAttributeText(XMLElement* Element, Text* Text);
		static void ParseAttributeValue(XMLElement* Element, Text* Text);
		static void ParseAttributeAlignment(XMLElement* Element, Text* Text);

		static void ParseAttributeTimeSteps(XMLElement* Element, Histogram* Histogram);
		static void ParseAttributeScale(XMLElement* Element, Histogram* Histogram);
		static void ParseAttributeInterval(XMLElement* Element, Histogram* Histogram);
		static void ParseAttributeValue(XMLElement* Element, Histogram* Histogram);
	};
}

namespace interface
{
	template<typename N>
	static void LayoutParser::Register()
	{
		gNodeDeclarations[N::NodeName] = NodeInfo{ N::BaseName, N::CreateInstance };
	}
};