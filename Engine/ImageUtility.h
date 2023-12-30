#pragma once

#include <filesystem>
#include <vector>

#include <Common/Types.h>

namespace engine
{
	using namespace common;

	class ImageUtility
	{
	public:

		static std::vector<U8> ReadPng(std::filesystem::path const& File);

		static void WritePng(U32 Width, U32 Height, std::vector<U8> const& Data, std::filesystem::path const& File);
	};
}