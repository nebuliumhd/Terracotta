#include <iostream>
#include "Foobar.hpp"

void GameInit(GameState* state)
{
	std::cout << "Initialized the game!\n";
}
void GameUpdate(GameState* state)
{
	// std::cout << "Old function!\n";
}
void GameShutdown(GameState* state)
{
	std::cout << "Shut down the game!\n";
}