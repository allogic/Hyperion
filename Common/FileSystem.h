#pragma once

#include <filesystem>
#include <vector>
#include <fstream>

#include <Common/Types.h>

namespace common
{
	class FileSystem
	{
	public:

		static std::vector<U8> ReadBinary(std::filesystem::path const& File);
	};
}