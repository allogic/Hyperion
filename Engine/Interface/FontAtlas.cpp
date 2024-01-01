#include <Engine/Common/Macros.h>
#include <Engine/Common/Config.h>

#include <Engine/Interface/FontAtlas.h>

#include <Engine/FreeType/ft2build.h>
#include <Engine/FreeType/freetype/freetype.h>

#include <Engine/Vulkan/Image.h>
#include <Engine/Vulkan/ImageVariance.h>
#include <Engine/Vulkan/Renderer.h>
#include <Engine/Vulkan/Window.h>

namespace hyperion
{
	static U32 sAtlasWidth = 0;
	static U32 sAtlasHeight = 0;
	static U32 sFontIndex = 0;

	static std::vector<U8> sAtlas = {};

	bool FontAtlas::Load(std::filesystem::path const& File)
	{
		FT_Error result = 0;

		std::string fontName = File.stem().string();

		FT_Library library = 0;
		result = FT_Init_FreeType(&library);

		if (result == 0)
		{
			FT_Face face = 0;
			result = FT_New_Face(library, File.string().data(), 0, &face);

			if (result == 0)
			{
				FT_Set_Pixel_Sizes(face, 0, FontSize);

				FontInfo fontInfo = {};

				U32 characterWidth = FontSize;
				U32 characterHeight = FontSize;

				U32 fontWidth = FontSize * 128;
				U32 fontHeight = FontSize;

				R32V2 fontSize = { fontWidth, fontHeight };
				R32V2 characterSize = { characterWidth, characterHeight };

				sAtlasWidth = fontWidth;
				sAtlasHeight += fontHeight;

				sAtlas.resize(fontWidth * fontHeight * (sFontIndex + 1));

				for (U8 c = 0; c < 128; ++c)
				{
					result = FT_Load_Char(face, c, FT_LOAD_RENDER);

					if (result == 0)
					{
						U32 bitmapWidth = face->glyph->bitmap.width;
						U32 bitmapHeight = face->glyph->bitmap.rows;
						U32 bitmapLeft = face->glyph->bitmap_left;
						U32 bitmapTop = face->glyph->bitmap_top;
						U32 bitmapSize = bitmapWidth * bitmapHeight;
						U32 bitmapAdvance = face->glyph->advance.x;

						R32V2 size = { bitmapWidth, bitmapHeight };
						R32V2 bearing = { bitmapLeft, bitmapTop };

						fontInfo.CharacterInfos[c] = CharacterInfo{ size, bearing, bitmapAdvance };

						std::vector<U8> bitmap = {};
						bitmap.resize(bitmapSize);

						for (U32 i = 0; i < bitmapSize; ++i)
						{
							bitmap[i] = face->glyph->bitmap.buffer[i];
						}

						AddToAtlas(c, fontWidth, fontHeight, bitmap, bitmapWidth, bitmapHeight);
					}
				}

				fontInfo.FontIndex = sFontIndex;
				fontInfo.FontSize = fontSize;
				fontInfo.CharacterSize = characterSize;

				gRenderer->SetFontInfo(fontName, fontInfo);
				gRenderer->UpdateFontAtlas(sAtlasWidth, sAtlasHeight, sAtlas);
				gRenderer->UpdateTextDescriptorSets(0);

				sFontIndex++;

				FT_Done_Face(face);
			}

			FT_Done_FreeType(library);
		}

		return result == 0;
	}

	void FontAtlas::AddToAtlas(U8 Character, U32 FontWidth, U32 FontHeight, std::vector<U8>& Bitmap, U32 BitmapWidth, U32 BitmapHeight)
	{
		for (U32 i = 0; i < BitmapWidth; ++i)
		{
			for (U32 j = 0; j < BitmapHeight; ++j)
			{
				U32 atlasIndex = (Character * FontSize + i) + (sFontIndex * FontHeight + j) * FontWidth;
				U32 bitmapIndex = i + j * BitmapWidth;

				sAtlas[atlasIndex] = Bitmap[bitmapIndex];
			}
		}
	}
}