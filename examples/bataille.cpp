#include <stdio.h>
#include <stdlib.h>

#include "CGame.h"
#include "font/font.h"

#include <iostream>

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768
#define NUM_PLAYERS		2
#define MAX_PLAYERS		2

typedef struct BatailleCardsRegions_s {
	unsigned int x;
	unsigned int y;
	unsigned int x_offset;
	unsigned int y_offset;
} BatailleCardsRegions_t;

static BatailleCardsRegions_t regions [MAX_PLAYERS * 2] = 
{
	{ 40, (SCREEN_HEIGHT - CARDHEIGHT) / 2, 2, 2}, // Player 1 "hand" card region
	{ 40 + CARDWIDTH + 40, (SCREEN_HEIGHT - CARDHEIGHT) / 2, 2, 2}, // Player 1 "played" card region
	{ SCREEN_WIDTH - CARDWIDTH - 40, (SCREEN_HEIGHT - CARDHEIGHT) / 2, 2, 2}, // Player 2 "hand" card region
	{ SCREEN_WIDTH - CARDWIDTH*2 - 80, (SCREEN_HEIGHT - CARDHEIGHT) / 2, 2, 2}, // Player 2 "played" card region
};

/* A Bataille Player handles two piles:
	- his hand
	- the cards she/he plays

Cards travel from player's hand to the region she/he plays to
and often/sometimes back to his hand when she/he wins a trick

*/
class Player {
public:
	Player();
	~Player();
	void initialize(CGame *gameRegions, int startId, BatailleCardsRegions_t *hand, BatailleCardsRegions_t *played);
	void pushToHand(CCardStack stack)	{ game->at(startId).Push(stack); }
	void pushToHand(CCard card)			{ game->at(startId).Push(card); }
	CCard playOneCard();
	void newDeck();
	CCard getOneCardFromHand()		{ return game->at(startId).Pop(); }
	bool isHandEmpty() 				{ return game->at(startId).Empty(); }
	int getNumOfCardsInHand()		{ return game->at(startId).Size(); }
	CCardStack *getHandCardStack()	{ return game->at(startId).GetCardStack(); }

private:
	CGame *game;
	int startId;
};


Player::Player(): game(0)
{
}

Player::~Player()
{
}

void Player::initialize(CGame *game, int startId, BatailleCardsRegions_t *handRegion, BatailleCardsRegions_t *playedRegion)
{
	// Create region for "hand" pile
	game->CreateRegion(
			startId,
			CRD_VISIBLE|CRD_3D,
			CRD_DOALL,
			0,
			CRD_OSYMBOL,
			handRegion->x,
			handRegion->y,
			handRegion->x_offset,
			handRegion->x_offset);

	// Create region for "played" pile
	game->CreateRegion(
			startId + 1,
			CRD_VISIBLE|CRD_3D,
			CRD_DOALL,
			0,
			CRD_OSYMBOL,
			playedRegion->x,
			playedRegion->y,
			playedRegion->x_offset,
			playedRegion->x_offset);

	this->startId = startId;
	this->game = game;
}

void Player::newDeck()
{
	game->at(startId).NewDeck();
	game->at(startId).Shuffle();
}

CCard Player::playOneCard()
{
	// Get card from hand
	CCard card = game->at(startId).Pop();
	// Push it into played hand
	game->at(startId + 1).Push(card);

	printf("Player %d just played: %d cards left in hand\n", startId / 2, game->at(startId).Size());
	return card;	
}

class Bataille {
public:
	Bataille();
	~Bataille();

	void initialize(SDL_Surface *screen);
	void resetGame();
	void startNewGame();
	void simulate();

	void TestCardStack();

	void playOneTrick(CCardStack *trick);
	uint8_t getTrickWinner(CCardStack *trick);
	void playOneRound(CCardStack *trick);

	bool isGameOver();

private:
	std::vector<Player> players;
	CGame game;
	//SDL_Surface *screen;
};


Bataille::Bataille()
{
}

Bataille::~Bataille()
{
	players.clear();
}

void Bataille::initialize(SDL_Surface *screen)
{
	//this->screen = screen;
	game.Initialize(screen);	

	// Create Players
	for (int p = 0; p < NUM_PLAYERS; p++) {
		Player *player = new Player;
		player->initialize(&game, p*2, &regions[p*2], &regions[p*2 + 1]);
		players.push_back(*player);
	}
}

void Bataille::resetGame()
{
	// Empty the card regions from the previous game
	game.EmptyStacks();

 	// create then shuffle the deck
	players[0].newDeck();

	// deal cards: split the deck evenly
	for (int card = 0; card < (52 / NUM_PLAYERS); card ++) {
		players[1].pushToHand(players[0].getOneCardFromHand());
	}

	for (int i = 0; i < NUM_PLAYERS; i++) {
		printf("Player %d cards: %d\n", i, players[i].getNumOfCardsInHand());

		printf("===============================================\n");
		CCardStack *cs = players[i].getHandCardStack();
		for (int c=0;c<cs->size(); c++)
			printf("card %d: %s of %s\n", c+1, cs->at(c).Rank_str(), cs->at(c).Suit_str());

		printf("===============================================\n");
	}

    // initialize all card coordinates
	game.InitAllCoords();
	game.DrawStaticScene();	
}

void Bataille::playOneRound(CCardStack *trick)
{
	// A round is a set of tricks.
	// One trick is when 2 players play one card
	// if we hit "BATTLE", players get to play another card and so on
	// When we finally find a winner for all the tricks, all the cards
	// that have been played during this round go to the winner

	int trickwinner;
	int round_done = 0;
	while (!round_done) {
		playOneTrick(trick);
		trickwinner = getTrickWinner(trick);
		if (trickwinner != 2) {
			round_done = 1;
		} else {
			// BATTLE: both players play again if they both still have cards left
			if (isGameOver()) {
				round_done = 1;
			}
		}
	} // while (!round_done)

	if (trickwinner < 2) {
		// Push all played cards to winner's hand
		players[trickwinner].pushToHand(*trick);
	}
}


void Bataille::playOneTrick(CCardStack *trick)
{
	// Get both players to play one card
	for (int p = 0; p < NUM_PLAYERS; p++) {
		CCard card = players[p].playOneCard();
		trick->Push(card);
	}
}

uint8_t Bataille::getTrickWinner(CCardStack *trick)
{
	// As we go along, the trick can grow more than 2 cards
	// if we hit a "BATTLE" situation.
	// The last two cards from the trick are always the last
	// ones that have been played by the players
	int p1_card_pos = trick->size() - 2;
	int p2_card_pos = trick->size() - 1;

	std::cout << "\nTrick: " << trick->at(p1_card_pos) << " vs. " << trick->at(p2_card_pos) << std::endl;
	if (trick->at(p1_card_pos).Rank() > trick->at(p2_card_pos).Rank()) {
		// WINNER is player 1
		printf("Trick winner is player 1 (%lu cards won)\n", trick->size());
		return 0;
	} else if (trick->at(p1_card_pos).Rank() < trick->at(p2_card_pos).Rank()) {
		printf("Trick winner is player 2 (%lu cards won)\n", trick->size());
		// Winner is player 2
		return 1;
	} else {
		printf("BATTLE\n");
		// BATTLE
		return 2;
	}
}

bool Bataille::isGameOver()
{
	return  ((players[0].isHandEmpty()) || (players[1].isHandEmpty()));
}

void Bataille::startNewGame()
{
}

void Bataille::simulate()
{
	int done = 0;
	CCardStack trick;

	// This simulates a party
	while (!done) {

		// The game is a simple loop
		// 1 - Each player plays a card
		// 2 - highest ranked card wins then both cards go to winner's pile
		// 3 - in case two cards have same rank, both players play again
		// 4 - same rule as above applies
		// 5 - Winner is the player that has all the cards

		playOneRound(&trick);
		trick.clear();

		printf("Player 1 has now have %d cards\n", players[0].getNumOfCardsInHand());
		printf("Player 2 has now have %d cards\n", players[1].getNumOfCardsInHand());

		if (isGameOver()) {
			done = 1;
		}
	}	
}

static void initGraphics (SDL_Surface **screen, 	SDLFont **font1, SDLFont **font2)
{
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);

	*screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE|SDL_HWPALETTE);
	if (*screen == NULL) {
		printf("Unable to set %dx%d video: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	// Initialize screen
	SDL_WM_SetCaption("Bataille", NULL); // Set the window title
	*font1 = initFont("font/data/font1");
	*font2 = initFont("font/data/font2");

	InitDeck(*screen);
}

int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	SDLFont *font1;
	SDLFont *font2;
	Bataille b;

	initGraphics(&screen, &font1, &font2);

	b.initialize(screen);
	b.resetGame();

	SDL_Event event;
	int done = 0;

	b.simulate();

	while(done == 0)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
     				return 0;

				case SDL_KEYDOWN:
     				if(event.key.keysym.sym == SDLK_ESCAPE) { done = 1; }
					//HandleKeyDownEvent(event);
         			break;

				case SDL_MOUSEBUTTONDOWN:
					//HandleMouseDownEvent(event);
  					break;

				case SDL_MOUSEMOTION:
					//HandleMouseMoveEvent(event);
					break;

				case SDL_MOUSEBUTTONUP:
					//HandleMouseUpEvent(event);
 					break;
			}
		}
	}

	freeFont(font1);
	freeFont(font2);

	return 0;
}
