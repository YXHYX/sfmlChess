#pragma once
#include <SFML/Graphics.hpp>
#include <xmemory>
#include <vector>
#include <iostream>
#include <fstream>


class Board
{
private:
	
	sf::Clock timer;
	
	sf::Sprite *boardSprite;
	sf::Texture boardTexture;

	sf::Texture piecesTexture;
	sf::Sprite* pieceSprite;
	
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
	
	void resetBoard();
	void renderPotentialMoves(sf::RenderTarget* target);
	void calculatePotentialMoves();
	//function used to calculate movesBoard for each turn for each team to see where the king can roam
	void calcKingSafeSpace(bool team);

	void lookForChecks();

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

