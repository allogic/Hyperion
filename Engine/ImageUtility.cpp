#include <Engine/ImageUtility.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/stbimage/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Vendor/stbimage/stb_image_write.h>

namespace engine
{
	std::vector<U8> ImageUtility::ReadPng(std::filesystem::path const& File)
	{
		std::vector<U8> data = {};

		I32 width = 0;
		I32 height = 0;
		I32 channels = 0;

		U8* raw = stbi_load(File.string().data(), &width, &height, &channels, 0);

		U64 size = (U64)width * height * channels;

		data.resize(size);

		for (U64 i = 0; i < size; ++i)
		{
			data[i] = raw[i];
		}

		stbi_image_free(raw);

		return data;
	}

	void ImageUtility::WritePng(U32 Width, U32 Height, std::vector<U8> const& Data, std::filesystem::path const& File)
	{
		stbi_write_png(File.string().data(), Width, Height, 1, Data.data(), Width);
	}
}