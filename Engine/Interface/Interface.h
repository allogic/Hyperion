#pragma once

#include <string>
#include <filesystem>
#include <vector>

#include <Engine/Common/Types.h>

namespace hyperion
{
	class Interface
	{
	public:

		static bool LoadFont(std::filesystem::path const& File);

	public:

		static bool ParseLayout(std::filesystem::path const& File);

	public:

		static void Update();
		static void Render();
		static void Destroy();
	};
}