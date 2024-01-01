#pragma once

#include <Engine/Scene.h>

namespace automation
{
	using namespace engine;

	class Game : public Scene
	{
	public:

		Game(U32 NumChunksX, U32 NumChunksY);
		virtual ~Game();

	private:

		U32 mNumChunksX = 0;
		U32 mNumChunksY = 0;
	};
}