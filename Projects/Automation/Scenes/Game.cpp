#include <Automation/Scenes/Game.h>

namespace automation
{
	Game::Game(U32 NumChunksX, U32 NumChunksY) : Scene()
	{
		mNumChunksX = NumChunksX;
		mNumChunksY = NumChunksY;
	}

	Game::~Game()
	{

	}
}