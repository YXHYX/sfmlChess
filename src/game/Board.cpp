#include "Board.h"
#include "Board.h"
/*
  0 empty
  1 pawn
  2 bishop
  3 rook
  4 knight
  5 queen
  6 king 
  
  negative for black, positive for white
*/


void Board::initSounds()
{
	this->soundBuffers["lose"] = sf::SoundBuffer("./res/lose.wav");
	this->soundBuffers["win"] = sf::SoundBuffer("./res/win.wav");
	this->soundBuffers["select"] = sf::SoundBuffer("./res/select.wav");
	this->soundBuffers["select2"] = sf::SoundBuffer("./res/select.wav");
	this->soundBuffers["unselectable"] = sf::SoundBuffer("./res/unselectable.wav");
	//this->soundBuffers["unselectable2"] = sf::SoundBuffer("./res/unselectable.wav");
	this->soundBuffers["move"] = sf::SoundBuffer("./res/move.wav");
	this->soundBuffers["take"] = sf::SoundBuffer("./res/take.wav");
	this->soundBuffers["check"] = sf::SoundBuffer("./res/check.wav");


	this->soundEffect = new sf::Sound(this->soundBuffers["select"]);
	
}

void Board::resetBoard()
{
	//read directly from file
	/*debug 
	std::ifstream layoutFile("./src/game/layout.txt");
	int i = 0, j = 0;
	std::string stream;
	if (layoutFile.is_open()) {
		while (layoutFile.good()) {
			layoutFile >> stream;


			board[i][j] = std::stoi(stream);
			
			j++;
			if(j == 8)
			{
				j = 0;

				i++;
			}
		}
	}*/

	//default board
	for (int i = 0; i < 8; i++)
	{
		//white side pawns
		board[i][1] = 1;
		board[i][2] = 0;
		board[i][3] = 0;
		board[i][4] = 0;
		board[i][5] = 0;
		//black pawns
		board[i][6] = -1;
	}
	//white side
	board[0][0] = 3;	board[1][0] = 4;	board[2][0] = 2; 	board[3][0] = 5; 	board[4][0] = 6; 	board[5][0] = 2; 	board[6][0] = 4;	board[7][0] = 3;
	//black side
	board[0][7] = -3;	board[1][7] = -4;	board[2][7] = -2; 	board[3][7] = -5; 	board[4][7] = -6; 	board[5][7] = -2; 	board[6][7] = -4;	board[7][7] = -3;
	
}

void Board::renderPotentialMoves(sf::RenderTarget* target)
{
	if (!target || !this->isPieceSelected)
		return;
	
	auto [x, y] = this->selectedPieceCoords;

	sf::RectangleShape box(this->pieceSprite->getGlobalBounds().size);
	//Greenish
	box.setFillColor(sf::Color(0, 200, 0, 128));

	//current position
	sf::Vector2f nextPos(box.getSize().componentWiseMul(sf::Vector2f(x, y)));
	box.setPosition(nextPos);
	target->draw(box);
	//Orangish
	box.setFillColor(sf::Color(255, 187, 0, 128));

	//Loop through all potential moves and draw them
	for (auto& e : this->boardMoves[x][y]) {
		box.setPosition(box.getSize().componentWiseMul(sf::Vector2f(e)));
		target->draw(box);
	}
}

void Board::calcPotentialMoves()
{
	// 1 for white -1 for black
	int team = (2 * turn - 1);

	//clear the boardMoves which stores every potential move for the corresponding team
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			this->boardMoves[i][j].clear();
	this->potentialMoves.clear();

	//for pawns
	int pawnDir = turn ? 1 : -1;

	//variable to check if the player can continue playing or not
	bool movesAvailable = false;

	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
		{
			//only calculate the potential moves for the current team 
			if (this->board[x][y] * team < 1)
				continue;
			
			//for pawns
			bool pawnIn = y + pawnDir > -1 && y + pawnDir < 8;// out of boundary
			bool pawnMoved = this->turn ? (y != 1) : (y != 6);
			//diagonal/horizontal/vertical flags check
			bool ul = true, ur = true, bl = true, br = true;
			bool up = true, bot = true, right = true, left = true;

			switch(abs(this->board[x][y]))
			{
			case 1:
				//check for promotion
				/// TODO
				if (!pawnIn)
					break;
				//side check for enemy on right or left, eat them 
				if (x - 1 > -1 && board[x - 1][y + pawnDir]* pawnDir <= -1)
					potentialMoves.emplace_back(sf::Vector2i(x-1, y + pawnDir));
				if (x + 1 < 8 && board[x + 1][y + pawnDir]*pawnDir <= -1)
					potentialMoves.emplace_back(sf::Vector2i(x+1, y + pawnDir));
		
				if (board[x][y + pawnDir] != 0)
					break;
		
				potentialMoves.emplace_back(sf::Vector2i(x, y + pawnDir));
		
				// if pawn moved, u cant move it to 2 spots ahead
				if (pawnMoved || (y + 2 * pawnDir > -1 && y + 2 * pawnDir < 8 && board[x][y + 2 * pawnDir] != 0))
					break;

				potentialMoves.emplace_back(sf::Vector2i(x, y + 2*pawnDir));

				//add en passant ( check the previous move, if it is a pawn that moved 2 tiles ahead then it can be eaten with en passant
				/*
				if (x - 1 > -1 && board[x - 1][y + 2*pawnDir] * pawnDir <= -1)
					potentialMoves.emplace_back(sf::Vector2i(x - 1, y + 2 * pawnDir));
				if (x + 1 < 8 && board[x + 1][y + 2 * pawnDir] * pawnDir <= -1)
					potentialMoves.emplace_back(sf::Vector2i(x + 1, y + 2 * pawnDir));
				*/
					break;
			//bishop
			case 2:
				// check flags for
				// up left, up right, bottom left, bottom right
				for(int i = 1; i < 8; i++){
					//ul
					if (x - i < 0 || y + i > 7 || board[x - i][y + i] * team > 0)
						ul = false;
					//ur
					if (x + i > 7 || y + i > 7 || board[x + i][y + i] * team > 0 )
						ur = false;
					//bl
					if (x - i < 0 || y - i < 0 || board[x - i][y - i] * team > 0)
						bl = false;
					//br
					if (x + i > 7 || y - i < 0 || board[x + i][y - i] * team > 0)
						br = false;

					if(ul)
						potentialMoves.emplace_back(sf::Vector2i(x - i, y + i));
					if (ur)
						potentialMoves.emplace_back(sf::Vector2i(x + i, y + i));
					if (bl)
						potentialMoves.emplace_back(sf::Vector2i(x - i, y - i));
					if (br)
						potentialMoves.emplace_back(sf::Vector2i(x + i, y - i));

					// if opposite team then mark them as "edible" by the piece and disable the flag ( you cant eat any pieces stacked, just the first one)		
					ul = board[x - i][y + i] * team < 0 ? false : ul;
					ur = board[x + i][y + i] * team < 0 ? false : ur;
					bl = board[x - i][y - i] * team < 0 ? false : bl;
					br = board[x + i][y - i] * team < 0 ? false : br;
			
					if (!ul && !ur && !br && !bl)
						break;
				}

				break;
			//rook
			case 3:
				for (int i = 1; i < 8; i++) {

					//left
					if (x - i < 0 || board[x - i][y] * team > 0 )
						left = false;
					//right
					if (x + i > 7 || board[x + i][y] * team > 0)
						right = false;
					//up
					if (y - i < 0 || board[x][y - i] * team > 0)
						up = false;
					//bot
					if (y + i > 7 || board[x][y + i] * team > 0)
						bot = false;

					if (left)
						potentialMoves.emplace_back(sf::Vector2i(x - i, y));
					if (right)
						potentialMoves.emplace_back(sf::Vector2i(x + i, y));
					if (up)
						potentialMoves.emplace_back(sf::Vector2i(x, y - i));
					if (bot)
						potentialMoves.emplace_back(sf::Vector2i(x, y + i));

					left = board[x - i][y] * team < 0 ? false : left;
					right = board[x + i][y] * team < 0 ? false : right;
					up = board[x][y - i] * team < 0 ? false : up;
					bot = board[x][y + i] * team < 0 ? false : bot;

					if (!left && !right && !bot && !up)
						break;
				}
				break;
			//knight
			case 4:
				//check all 8 corners
				if (x + 1 < 8 && y + 2 < 8 && board[x + 1][y + 2] * team <= 0)
					potentialMoves.emplace_back(sf::Vector2i(x + 1, y + 2));
				if (x + 2 < 8 && y + 1 < 8 && board[x + 2][y + 1] * team <= 0)
					potentialMoves.emplace_back(sf::Vector2i(x + 2, y + 1));
				if (x - 1 > -1 && y + 2 < 8 && board[x - 1][y + 2] * team <= 0)
					potentialMoves.emplace_back(sf::Vector2i(x - 1, y + 2));
				if (x - 2 > -1 && y + 1 < 8 && board[x - 2][y + 1] * team <= 0)
					potentialMoves.emplace_back(sf::Vector2i(x - 2, y + 1));

		
				if (x + 1 < 8 && y - 2 > -1 && board[x + 1][y - 2] * team <= 0)
					potentialMoves.emplace_back(sf::Vector2i(x + 1, y - 2));
				if (x + 2 < 8 && y - 1 > -1 && board[x + 2][y - 1] * team <= 0)
					potentialMoves.emplace_back(sf::Vector2i(x + 2, y - 1));
				if (x - 1 > -1 && y - 2 > -1 && board[x - 1][y - 2] * team <= 0)
					potentialMoves.emplace_back(sf::Vector2i(x - 1, y - 2));
				if (x - 2 > -1 && y - 1 > -1 && board[x - 2][y - 1] * team <= 0)
					potentialMoves.emplace_back(sf::Vector2i(x - 2, y - 1));
		
				break;
			//queen
			case 5:
				for (int i = 1; i < 8; i++) {

					//combine rook and bishop checks
			
					//ul
					if (x - i < 0 || y + i > 7 || board[x - i][y + i] * team > 0)
						ul = false;
					//ur
					if (x + i > 7 || y + i > 7 || board[x + i][y + i] * team > 0)
						ur = false;
					//bl
					if (x - i < 0 || y - i < 0 || board[x - i][y - i] * team > 0)
						bl = false;
					//br
					if (x + i > 7 || y - i < 0 || board[x + i][y - i] * team > 0)
						br = false;

			
					//left
					if (x - i < 0 || board[x - i][y] * team > 0)
						left = false;
					//right
					if (x + i > 7 || board[x + i][y] * team > 0)
						right = false;
					//up
					if (y - i < 0 || board[x][y - i] * team > 0)
						up = false;
					//bot
					if (y + i > 7 || board[x][y + i] * team > 0)
						bot = false;

					if (left)
						potentialMoves.emplace_back(sf::Vector2i(x - i, y));
					if (right)
						potentialMoves.emplace_back(sf::Vector2i(x + i, y));
					if (up)
						potentialMoves.emplace_back(sf::Vector2i(x, y - i));
					if (bot)
						potentialMoves.emplace_back(sf::Vector2i(x, y + i));
					if (ul)
						potentialMoves.emplace_back(sf::Vector2i(x - i, y + i));
					if (ur)
						potentialMoves.emplace_back(sf::Vector2i(x + i, y + i));
					if (bl)
						potentialMoves.emplace_back(sf::Vector2i(x - i, y - i));
					if (br)
						potentialMoves.emplace_back(sf::Vector2i(x + i, y - i));

					left = board[x - i][y] * team < 0 ? false : left;
					right = board[x + i][y] * team < 0 ? false : right;
					up = board[x][y - i] * team < 0 ? false : up;
					bot = board[x][y + i] * team < 0 ? false : bot;

					ul = board[x - i][y + i] * team < 0 ? false : ul;
					ur = board[x + i][y + i] * team < 0 ? false : ur;
					bl = board[x - i][y - i] * team < 0 ? false : bl;
					br = board[x + i][y - i] * team < 0 ? false : br;
					if (!left && !right && !bot && !up && !ul && !ur && !br && !bl)
						break;
				}
				break;
	
			//king
				//superposed board to check where can the king move freely and check it in its entourage
			case 6:
				this->calcKingSafeSpace(this->turn);
				for (int i = -1; i <= 1; i++)
					for (int j = -1; j <= 1; j++)
					{
						if (!sf::IntRect({ 0,0 }, { 8,8 }).contains({x+i,y+j})
							|| this->movesBoard[x + i][y + j] == -1 || (!i && !j))
							continue;

						potentialMoves.emplace_back(sf::Vector2i(x + i, y + j));
					}
				break;

			default:
				break;
			}

			//PIN CHECK

			// cant move pieces that are pinned right?
			// delete any potential move that may cause an in direct check?
			// if checked delete any potential move which does not change the 
			// fate of our beloved king?

			bool tempCheck = this->check;
			for (auto it = this->potentialMoves.begin(); it != this->potentialMoves.end();)
			{
				bool removeMove = false;

				int tempVal = this->board[x][y];
				int tempVal2 = this->board[(*it).x][(*it).y];
				this->board[(*it).x][(*it).y] = this->board[x][y];
				this->board[x][y] = 0;
				this->check = false;
				this->lookForChecks();

				this->board[(*it).x][(*it).y] = tempVal2;
				this->board[x][y] = tempVal;

				removeMove = this->check;

				if (removeMove)
					it = this->potentialMoves.erase(it);
				else
					it++;
			}
			this->check = tempCheck;

			if (!potentialMoves.empty())
			{
				//copy the vector into the board moves and clear then continue
				std::copy(potentialMoves.begin(), potentialMoves.end(), std::back_inserter(this->boardMoves[x][y]));
				movesAvailable = true;
			}

			potentialMoves.clear();
		}
	/*
	if (potentialMoves.empty()) {
		this->soundPlayed = "unselectable";
	}
	*/

		if (!movesAvailable)
			this->checkmate = true;
}

void Board::calcKingSafeSpace(bool team)
{
	int whichTeam = team ? 1 : -1;
	//reset the moves board
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
			this->movesBoard[x][y] = 0;
	for(int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
		{
			// same whichTeam?
			if (this->board[x][y] * whichTeam > 0 && abs(this->board[x][y]) != 6)
			{
				this->movesBoard[x][y] = -1; //occupied
				continue;
			}
			if (this->board[x][y] == 0) //we dont care about vacant places
				continue;

			//for pawns
			int pawnDir = this->board[x][y] > 0 ? 1 : -1;
			bool pawnIn = y + pawnDir > -1 && y + pawnDir < 8;// out of boundary
			//diagonal/horizontal/vertical checks
			bool ul = true, ur = true, bl = true, br = true;
			bool up = true, bot = true, right = true, left = true;

			switch (this->board[x][y] * -whichTeam)
			{
			case 1:
				//check for promotion
				if (!pawnIn)
					break;
				//eat
				if (x - 1 > -1)
					this->movesBoard[x - 1][ y + pawnDir] = -1;
				if (x + 1 < 8)
					this->movesBoard[x + 1][ y + pawnDir] = -1;
				break;
				//bishop
			case 2:
				// check flags for
				// up left, up right, bottom left, bottom right
				for (int i = 1; i < 8; i++) {
					//ul
					if (x - i < 0 || y + i > 7 || board[x - i][y + i] * -whichTeam > 0)
						ul = false;
					//ur
					if (x + i > 7 || y + i > 7 || board[x + i][y + i] * -whichTeam > 0)
						ur = false;
					//bl
					if (x - i < 0 || y - i < 0 || board[x - i][y - i] * -whichTeam > 0)
						bl = false;
					//br
					if (x + i > 7 || y - i < 0 || board[x + i][y - i] * -whichTeam > 0)
						br = false;

					if (ul)

						this->movesBoard[x - i][y + i] = -1;
					if (ur)
						this->movesBoard[x + i][ y + i] = -1;
					if (bl)
						this->movesBoard[x - i][y - i] = -1;
					if (br)
						this->movesBoard[x + i][ y - i] = -1;


					ul = board[x - i][y + i] * -whichTeam < 0 ? false : ul;
					ur = board[x + i][y + i] * -whichTeam < 0 ? false : ur;
					bl = board[x - i][y - i] * -whichTeam < 0 ? false : bl;
					br = board[x + i][y - i] * -whichTeam < 0 ? false : br;

					if (!ul && !ur && !br && !bl)
						break;
				}

				break;
				//rook
			case 3:
				for (int i = 1; i < 8; i++) {

					//left
					if (x - i < 0 || board[x - i][y] * -whichTeam > 0)
						left = false;
					//right
					if (x + i > 7 || board[x + i][y] * -whichTeam > 0)
						right = false;
					//up
					if (y - i < 0 || board[x][y - i] * -whichTeam > 0)
						up = false;
					//bot
					if (y + i > 7 || board[x][y + i] * -whichTeam > 0)
						bot = false;

					if (left)
						this->movesBoard[x - i][y] = -1;
					if (right)
						this->movesBoard[x + i][y] = -1;
					if (up)
						this->movesBoard[x][y - i] = -1;
					if (bot)
						this->movesBoard[x][y + i] = -1;

					left = board[x - i][y] * -whichTeam < 0 ? false : left;
					right = board[x + i][y] * -whichTeam < 0 ? false : right;
					up = board[x][y - i] * -whichTeam < 0 ? false : up;
					bot = board[x][y + i] * -whichTeam < 0 ? false : bot;

					if (!left && !right && !bot && !up)
						break;
				}
				break;
				//knight
			case 4:
				//check all 8 corners
				if (x + 1 < 8 && y + 2 < 8 && board[x + 1][y + 2] * -whichTeam <= 0)
					this->movesBoard[x + 1][y + 2] = -1;
				if (x + 2 < 8 && y + 1 < 8 && board[x + 2][y + 1] * -whichTeam <= 0)
					this->movesBoard[x + 2][y + 1] = -1;
				if (x - 1 > -1 && y + 2 < 8 && board[x - 1][y + 2] * -whichTeam <= 0)
					this->movesBoard[x - 1][y + 2] = -1;
				if (x - 2 > -1 && y + 1 < 8 && board[x - 2][y + 1] * -whichTeam <= 0)
					this->movesBoard[x - 2][y + 1] = -1;


				if (x + 1 < 8 && y - 2 > -1 && board[x + 1][y - 2] * -whichTeam <= 0)
					this->movesBoard[x + 1][y - 2] = -1;
				if (x + 2 < 8 && y - 1 > -1 && board[x + 2][y - 1] * -whichTeam <= 0)
					this->movesBoard[x + 2][y - 1] = -1;
				if (x - 1 > -1 && y - 2 > -1 && board[x - 1][y - 2] * -whichTeam <= 0)
					this->movesBoard[x - 1][y - 2] = -1;
				if (x - 2 > -1 && y - 1 > -1 && board[x - 2][y - 1] * -whichTeam <= 0)
					this->movesBoard[x - 2][y - 1] = -1;

				break;
				//queen
			case 5:
				for (int i = 1; i < 8; i++) {
					//ul
					if (x - i < 0 || y + i > 7 || board[x - i][y + i] * -whichTeam > 0 )
						ul = false;
					//ur
					if (x + i > 7 || y + i > 7 || board[x + i][y + i] * -whichTeam > 0)
						ur = false;
					//bl
					if (x - i < 0 || y - i < 0 || board[x - i][y - i] * -whichTeam > 0)
						bl = false;
					//br
					if (x + i > 7 || y - i < 0 || board[x + i][y - i] * -whichTeam > 0)
						br = false;


					//left
					if (x - i < 0 || board[x - i][y] * -whichTeam > 0)
						left = false;
					//right
					if (x + i > 7 || board[x + i][y] * -whichTeam > 0)
						right = false;
					//up
					if (y - i < 0 || board[x][y - i] * -whichTeam > 0)
						up = false;
					//bot
					if (y + i > 7 || board[x][y + i] * -whichTeam > 0)
						bot = false;

					if (left)
						this->movesBoard[x - i][y] = -1;
					if (right)
						this->movesBoard[x + i][y] = -1;
					if (up)
						this->movesBoard[x][y - i] = -1;
					if (bot)
						this->movesBoard[x][y + i] = -1;
					if (ul)
						this->movesBoard[x - i][y + i] = -1;
					if (ur)
						this->movesBoard[x + i][y + i] = -1;
					if (bl)
						this->movesBoard[x - i][y - i] = -1;
					if (br)
						this->movesBoard[x + i][y - i] = -1;

					left = board[x - i][y] * -whichTeam < 0 ? false : left;
					right = board[x + i][y] * -whichTeam < 0 ? false : right;
					up = board[x][y - i] * -whichTeam < 0 ? false : up;
					bot = board[x][y + i] * -whichTeam < 0 ? false : bot;

					ul = board[x - i][y + i] * -whichTeam < 0 ? false : ul;
					ur = board[x + i][y + i] * -whichTeam < 0 ? false : ur;
					bl = board[x - i][y - i] * -whichTeam < 0 ? false : bl;
					br = board[x + i][y - i] * -whichTeam < 0 ? false : br;
					if (!left && !right && !bot && !up && !ul && !ur && !br && !bl)
						break;
				}
				break;

				//king

			case 6:
				for (int i = -1; i <= 1; i++)
					for (int j = -1; j <= 1; j++)
					{
						if (!sf::IntRect({ 0,0 }, { 8,8 }).contains({ x + i,y + j }) || (i == 0 && j == 0))
							continue;

						this->movesBoard[x + i][y + j] = -1;
					}
				break;

			default:
				break;
			}

		}

}

void Board::lookForChecks()
{
	//if a potential move coincides with the king of the opposing team
	//declare a check and see if its a checkmate

	this->calcKingSafeSpace(turn);

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (this->board[i][j]*(2*turn-1) == 6 && this->movesBoard[i][j] == -1)
			{
				this->check = true;
			}


	if (check) {
		this->soundPlayed = "check";
	}

}

void Board::playSounds()
{
	//std::cout << "prev:  " << this->prevSoundPlayed << " |  current:  " << this->soundPlayed<< '\n';

	//only play the sound if we change state
	//this ensures that it plays once
	if (this->soundPlayed != this->prevSoundPlayed) {
		this->prevSoundPlayed = this->soundPlayed;

		if (this->soundPlayed == "move")
			int a = 0;
		//if the sound doesnt exist dont play it !!!
		if (!this->soundBuffers.count(soundPlayed))
			return;
		if (this->soundEffect->getStatus() != sf::Sound::Status::Playing)
		{
			this->soundEffect->setBuffer(this->soundBuffers[soundPlayed]);
			this->soundEffect->play();
		}
	}
}

Board::Board()
{
	this->initSounds();
	if (!this->boardTexture.loadFromFile("./res/board.png"))
		throw "ERROR::TEXTURE_NOT_FOUND::BOARD";
	if (!this->piecesTexture.loadFromFile("./res/pieces.png"))
		throw "ERROR::TEXTURE_NOT_FOUND::PIECES";

	this->boardSprite = new sf::Sprite(this->boardTexture);
	this->boardSprite->scale({ 4.f , 4.f});

	this->pieceSprite = new sf::Sprite(this->piecesTexture);
	this->pieceSprite->scale({ 4.f , 4.f });

	this->resetBoard();
}

Board::~Board()
{
	delete this->boardSprite;
}

void Board::nextTurn()
{
	this->turn = !this->turn;
	this->calculatePotentialMoves = true;
}

void Board::updateMouseCoordinates(sf::Vector2i mouseCoordinates)
{
	this->mouseCoords = mouseCoordinates;
}

bool Board::getTurn()
{
	return turn;
}

std::pair<bool, bool> Board::getCheck()
{
	return std::pair<bool, bool>(this->check, this->checkmate);
}

std::pair<int, int> Board::getScore()
{
	return std::pair<int, int>(this->whiteScore, this->blackScore);
}

void Board::handleInput()
{
	bool changePlayerTurn = false;
	if (this->boardSprite->getGlobalBounds().contains(sf::Vector2f(this->mouseCoords)) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		//if there was a selected piece check if we are moving it or not
		sf::Vector2i coords = sf::Vector2i(sf::Vector2f(this->mouseCoords).componentWiseDiv(this->boardSprite->getGlobalBounds().size) * 8.f);
		if (!this->isPieceSelected)
		{

			this->selectedPieceCoords = coords;
			this->selectedPieceType = this->board[this->selectedPieceCoords.x][this->selectedPieceCoords.y];
			this->isPieceSelected = this->selectedPieceType * (2 * this->turn - 1) > 0;

			this->soundPlayed = "select";

		}
		else {
			bool playedMove = false;
			int pieceEaten = false;
			if (!this->boardMoves[selectedPieceCoords.x][selectedPieceCoords.y].empty())
				for (auto move : this->boardMoves[selectedPieceCoords.x][selectedPieceCoords.y])
				{
					// move it
					if (move == coords)
					{
						//add score!
						pieceEaten = abs(this->board[coords.x][coords.y]);
						playedMove = true;
						this->board[coords.x][coords.y] = this->board[this->selectedPieceCoords.x][this->selectedPieceCoords.y];
						this->board[this->selectedPieceCoords.x][this->selectedPieceCoords.y] = 0;
						//change the turn and log the move
						
						std::string output;
						int pieceMoved = abs(this->board[coords.x][coords.y]);
						if (pieceMoved > 1)
							output = "BRNQK"[pieceMoved - 2];
						output += "abcdefgh"[coords.x] + std::to_string(coords.y); // y number, x letter

						movesLog.push_back(output);
						std::cout << output << '\n';
						break; // no need to continue over the rest
					}
				}

			if (coords != this->selectedPieceCoords)
			{
				this->selectedPieceCoords = coords;
				bool piece = this->board[this->selectedPieceCoords.x][this->selectedPieceCoords.y] * (2 * this->turn - 1) > 0;
				/*if (piece)
				{
					this->selectedPieceType = this->board[this->selectedPieceCoords.x][this->selectedPieceCoords.y];
					this->isPieceSelected = 1;
				}
				else {
					
				}*/
				this->selectedPieceType = 0;
				this->isPieceSelected = 0;
				if (this->prevSoundPlayed == "unselectable")
					this->prevSoundPlayed = "unselectable2";
				
				this->soundPlayed = "select2";
			}
			if (playedMove)
			{

				//play the move sound
				this->soundPlayed = pieceEaten ? "take" : "move";
				//add score if a piece was eaten
				if (turn)
					this->whiteScore += pieceEaten;
				else
					this->blackScore += pieceEaten;
				
				this->nextTurn();

			}
		}
	}
	else
	{
		soundPlayed = "none";
	}
}


void Board::update(const float& dt)
{

	this->check = false;
	//calculate all moves every turn
	if (this->calculatePotentialMoves)
	{
		this->calcPotentialMoves();
		this->calculatePotentialMoves = false;
	}
	this->lookForChecks();

	
	
	//done processing the board? play the corresponding sound
	playSounds();
}

void Board::drawBoard(sf::RenderTarget* targ)
{
	targ->draw(*this->boardSprite);
}

void Board::drawPieces(sf::RenderTarget* targ)
{
	for(int i =0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			if (this->board[i][j] != 0)
			{
				this->pieceSprite->setTextureRect(sf::IntRect({ 16*(abs(this->board[i][j])-1), this->board[i][j] > 0 ? 0 : 16}, {16, 16}));
				this->pieceSprite->setPosition(sf::Vector2f(i*64,j*64)); // 16 x 4 scale
				targ->draw(*this->pieceSprite);
			}

}

void Board::render(sf::RenderTarget* targ)
{
	if (!targ)
		return;
	this->drawBoard(targ);
	this->drawPieces(targ);
	this->renderPotentialMoves(targ);
}

