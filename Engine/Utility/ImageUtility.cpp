#define STB_IMAGE_IMPLEMENTATION
#include <Engine/Stbimage/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Engine/Stbimage/stb_image_write.h>

#include <Engine/Utility/ImageUtility.h>

namespace hyperion
{
	void ImageUtility::ReadPngFromFile(std::filesystem::path const& File, std::vector<U8>& Data, U32* Width, U32* Height, U32* Channels)
	{
		U8* raw = stbi_load(File.string().data(), (I32*)Width, (I32*)Height, (I32*)Channels, 0);

		U64 size = (U64)(*Width) * (*Height) * (*Channels);

		Data.resize(size);

		std::memcpy(Data.data(), raw, size);

		stbi_image_free(raw);
	}

	void ImageUtility::ReadPngFromMemory(std::vector<U8> const& Buffer, std::vector<U8>& Data, U32* Width, U32* Height, U32* Channels)
	{
		U8* raw = stbi_load_from_memory(Buffer.data(), (U32)Buffer.size(), (I32*)Width, (I32*)Height, (I32*)Channels, 0);

		U64 size = (U64)(*Width) * (*Height) * (*Channels);

		Data.resize(size);

		std::memcpy(Data.data(), raw, size);

		stbi_image_free(raw);
	}

	void ImageUtility::ReadPngFromMemory(void* Buffer, U64 Size, std::vector<U8>& Data, U32* Width, U32* Height, U32* Channels)
	{
		U8* raw = stbi_load_from_memory((U8*)Buffer, (U32)Size, (I32*)Width, (I32*)Height, (I32*)Channels, 0);

		U64 size = (U64)(*Width) * (*Height) * (*Channels);

		Data.resize(size);

		std::memcpy(Data.data(), raw, size);

		stbi_image_free(raw);
	}

	std::vector<U8> ImageUtility::ReadPngFromFile(std::filesystem::path const& File, U32* Width, U32* Height, U32* Channels)
	{
		std::vector<U8> data = {};

		U8* raw = stbi_load(File.string().data(), (I32*)Width, (I32*)Height, (I32*)Channels, 0);

		U64 size = (U64)(*Width) * (*Height) * (*Channels);

		data.resize(size);

		std::memcpy(data.data(), raw, size);

		stbi_image_free(raw);

		return data;
	}

	std::vector<U8> ImageUtility::ReadPngFromMemory(std::vector<U8> const& Buffer, U32* Width, U32* Height, U32* Channels)
	{
		std::vector<U8> data = {};

		U8* raw = stbi_load_from_memory(Buffer.data(), (U32)Buffer.size(), (I32*)Width, (I32*)Height, (I32*)Channels, 0);

		U64 size = (U64)(*Width) * (*Height) * (*Channels);

		data.resize(size);

		std::memcpy(data.data(), raw, size);

		stbi_image_free(raw);

		return data;
	}

	std::vector<U8> ImageUtility::ReadPngFromMemory(void* Buffer, U64 Size, U32* Width, U32* Height, U32* Channels)
	{
		std::vector<U8> data = {};

		U8* raw = stbi_load_from_memory((U8*)Buffer, (U32)Size, (I32*)Width, (I32*)Height, (I32*)Channels, 0);

		U64 size = (U64)(*Width) * (*Height) * (*Channels);

		data.resize(size);

		std::memcpy(data.data(), raw, size);

		stbi_image_free(raw);

		return data;
	}

	void ImageUtility::WritePngToFile(U32 Width, U32 Height, std::vector<U8> const& Data, std::filesystem::path const& File)
	{
		stbi_write_png(File.string().data(), Width, Height, 1, Data.data(), Width);
	}
}