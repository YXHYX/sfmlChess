#include <iostream>
#include <windows.h>

#include "game/Game.h"
int main()
{
	//hide cmd
	ShowWindow(GetConsoleWindow(), SW_HIDE); //SW_RESTORE 
	Game game;

	game.run();



	return 0;
};