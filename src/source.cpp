#include <iostream>
#include <windows.h>

#include "game/Game.h"

/*	todo
* add castling, en passant
* add ui for promotion 
* properly fix logging
* properly setup sounds
* add menu and exit screen
* 
* for later:
* add bots (configure algorithms)
* add hints
* 
*/

int main()
{
	//hide cmd
	//ShowWindow(GetConsoleWindow(), SW_HIDE); //SW_RESTORE 
	Game game;

	game.run();



	return 0;
};