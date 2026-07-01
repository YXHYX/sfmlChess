#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <xmemory>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string>


class Board
{
private:
	
	//RESOURCES
	sf::Clock timer;
	//sprites n textures
	sf::Sprite *boardSprite;
	sf::Texture boardTexture;

	sf::Texture piecesTexture;
	sf::Sprite* pieceSprite;

	//sound stuff
	std::map<std::string, sf::SoundBuffer> soundBuffers;
	sf::Sound *soundEffect;

	//encodes all the other sounds to be played after being done processing everything
	std::string soundPlayed;
	std::string prevSoundPlayed;

	//logic
	sf::Vector2i mouseCoords;
	bool mouseHold = false;

	sf::Vector2i selectedPieceCoords;
	int selectedPieceType;
	bool isPieceSelected;
	std::vector<sf::Vector2i> potentialMoves;
	

	int board[8][8];
	int movesBoard[8][8]; // board superposed with other moves ( for kings only lol)
	bool movableBoard[8][8]; // movable pieces ( to evade checks and other stuff)
	bool turn = 1; // 1 for white 0 for black
	bool check;
	bool checkmate;
	int whiteScore = 0;
	int blackScore = 0;
	
	void initSounds();

	void resetBoard();
	void renderPotentialMoves(sf::RenderTarget* target);
	void calculatePotentialMoves();
	//function used to calculate movesBoard for each turn for each team to see where the king can roam
	void calcKingSafeSpace(bool team);

	void lookForChecks();

	void playSounds();

public:
	Board();
	~Board();
	
	void nextTurn();
	void updateMouseCoordinates(sf::Vector2i mouseCoordinates);

	bool getTurn();
	std::pair<bool,bool> getCheck();
	std::pair<int,int> getScore();

	void handleInput();
	void update(const float &dt);
	void drawBoard(sf::RenderTarget* targ);
	void drawPieces(sf::RenderTarget* targ);
	void render(sf::RenderTarget* targ);

};

