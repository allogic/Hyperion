#include <Engine/Common/FileSystem.h>

namespace hyperion
{
	std::vector<U8> FileSystem::ReadBinary(std::filesystem::path const& File)
	{
		std::vector<U8> bytes = {};

		std::ifstream stream = std::ifstream{ File, std::ios::ate | std::ios::binary };

		if (stream.is_open())
		{
			bytes.resize(stream.tellg());

			stream.seekg(std::ios::beg);
			stream.read((I8*)bytes.data(), bytes.size());
			stream.close();
		}

		return bytes;
	}
}