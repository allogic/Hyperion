#pragma once

#include <string>
#include <filesystem>
#include <array>

#include <Engine/Common/Types.h>

namespace hyperion
{
	constexpr U32 FontSize = 16;

	class FontAtlas
	{
	public:

		static bool Load(std::filesystem::path const& File);

	private:

		static void AddToAtlas(U8 Character, U32 FontWidth, U32 FontHeight, std::vector<U8>& Bitmap, U32 BitmapWidth, U32 BitmapHeight);
	};
}