#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <xmemory>
#include "Board.h"


//score 
//make a good bot
//add sound and better UI
//one calculation for each potential move at each turn (necessary for bot and 

class Game
{
private:

	//main comps
	sf::RenderWindow window;
	sf::Font font;

	//raw pointers no need for complicated stuff
	sf::Text* turn;
	sf::Text* score;
	sf::Text* check;
	//sf::Sound;

	sf::Clock ticker;
	float dt;
	Board board;

	sf::Vector2i mouseCoords;

	void handleInput();
	void update(const float& dt);
	void render();

public:
	Game();
	~Game();

	void run();
};

