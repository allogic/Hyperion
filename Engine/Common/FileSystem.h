#pragma once

#include <filesystem>
#include <vector>
#include <fstream>

#include <Engine/Common/Types.h>

namespace hyperion
{
	class FileSystem
	{
	public:

		static std::vector<U8> ReadBinary(std::filesystem::path const& File);
	};
}