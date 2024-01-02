#include <Engine/Common/Macros.h>
#include <Engine/Common/Config.h>

#include <Engine/Interface/Interface.h>
#include <Engine/Interface/LayoutParser.h>
#include <Engine/Interface/FontAtlas.h>
#include <Engine/Interface/Node.h>

namespace hyperion
{
	static std::vector<Node*> sLayoutNodes = {};

	bool Interface::LoadFont(std::filesystem::path const& File)
	{
		return FontAtlas::Load(File);
	}

	bool Interface::ParseLayout(std::filesystem::path const& File)
	{
		Node* layout = LayoutParser::Parse(File);

		if (layout)
		{
			sLayoutNodes.emplace_back(layout);
		}

		return layout;
	}

	void Interface::Update()
	{
		for (auto const& layout : sLayoutNodes)
		{
			layout->Update();
		}
	}

	void Interface::Render()
	{
		for (auto const& layout : sLayoutNodes)
		{
			layout->Render();
		}
	}

	void Interface::Destroy()
	{
		for (auto const& layout : sLayoutNodes)
		{
			delete layout;
		}
	}
}