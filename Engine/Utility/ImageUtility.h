#pragma once

#include <filesystem>
#include <vector>

#include <Engine/Common/Types.h>

namespace hyperion
{
	class ImageUtility
	{
	public:

		static void ReadPngFromFile(std::filesystem::path const& File, std::vector<U8>& Data, U32* Width, U32* Height, U32* Channels);
		static void ReadPngFromMemory(std::vector<U8> const& Buffer, std::vector<U8>& Data, U32* Width, U32* Height, U32* Channels);
		static void ReadPngFromMemory(void* Buffer, U64 Size, std::vector<U8>& Data, U32* Width, U32* Height, U32* Channels);

		static std::vector<U8> ReadPngFromFile(std::filesystem::path const& File, U32* Width, U32* Height, U32* Channels);
		static std::vector<U8> ReadPngFromMemory(std::vector<U8> const& Buffer, U32* Width, U32* Height, U32* Channels);
		static std::vector<U8> ReadPngFromMemory(void* Buffer, U64 Size, U32* Width, U32* Height, U32* Channels);

		static void WritePngToFile(U32 Width, U32 Height, std::vector<U8> const& Data, std::filesystem::path const& File);
	};
}