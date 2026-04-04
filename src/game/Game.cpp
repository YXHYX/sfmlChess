#include "Game.h"

void Game::handleInput()
{
	this->dt = this->ticker.restart().asSeconds();

	while (const std::optional event = this->window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			this->window.close();
		}
		else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
				this->window.close();
		}
	}

	this->board.updateMouseCoordinates(sf::Mouse::getPosition(this->window));
	this->board.handleInput();
}


void Game::update(const float& dt)
{
	this->board.update(dt);
	this->turn->setString(this->board.getTurn() ? "Turn: White" : "Turn: Black");
	this->check->setString(this->board.getCheck().first ? (this->board.getTurn() ? "White King Check!" : "Black king Check!") : "");;
	this->score->setString("White Score: " + std::to_string(this->board.getScore().first)
		+ "\nBlack Score: " + std::to_string(this->board.getScore().second));
}

void Game::render()
{
	this->window.clear();

	this->board.render(&this->window);
	this->window.draw(*this->turn);
	this->window.draw(*this->check);
	this->window.draw(*this->score);

	this->window.display();
}

Game::Game(): dt(1.f), font("./res/font.ttf") 
{ 
	turn = new sf::Text(font,"Turn: ");
	turn->setPosition({ 530,30 });

	score = new sf::Text(font, "Score: ");
	score->setPosition({ 530,100 });

	check = new sf::Text(font, "");
	check->setPosition({ 530,180 });

	this->window = sf::RenderWindow(sf::VideoMode(sf::Vector2u(800, 600)), "SFML Chess");
}

Game::~Game()
{
	delete this->turn;
	delete this->score;
	delete this->check;
}

void Game::run()
{
	while(this->window.isOpen())
	{
		while (const std::optional event = this->window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				this->window.close();
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
					this->window.close();
			}
		}
		while (this->window.hasFocus())
		{
			this->handleInput();
			this->update(this->dt);
			this->render();
		}
	}
}
