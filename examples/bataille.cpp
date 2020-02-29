#include <stdio.h>
#include <stdlib.h>

#include "CGame.h"
#include "font/font.h"

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768
#define NUM_PLAYERS		4
#define MAX_PLAYERS		4


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

	// TODO: X Coordinates to be reviewed
	{ (SCREEN_WIDTH - CARDWIDTH) / 2 -120, 40, 2, 2}, // Player 3 "hand" card region
	{ (SCREEN_WIDTH - CARDWIDTH) / 2, 40, 2, 2}, // Player 3 "played" card region

	// TODO: X Coordinates to be reviewed
	{ (SCREEN_WIDTH - CARDWIDTH) / 2 -120, (SCREEN_HEIGHT - CARDHEIGHT) - 40, 2, 2}, // Player 4 "hand" card region
	{ (SCREEN_WIDTH - CARDWIDTH) / 2, (SCREEN_HEIGHT - CARDHEIGHT) - 40, 2, 2}, // Player 4 "played" card region}
};

struct BatailleCardRegion {
	unsigned int id;
	unsigned int attribs;
	unsigned int x;
	unsigned int y;
	unsigned int x_offset;
	unsigned int y_offset;
};

// Up to 2 players
struct Player {
	BatailleCardRegion hand;
	BatailleCardRegion played;
};

struct BatailleCtx {
	CGame game;
	SDL_Surface *screen;
	SDLFont *font1;
	SDLFont *font2;
	unsigned int numPlayers;
};

static void InitializeCardsRegions(BatailleCtx *ctx) // must be called only once
{
	SDL_WM_SetCaption("Bataille", NULL); // Set the window title
	ctx->font1 = initFont("font/data/font1");
	ctx->font2 = initFont("font/data/font2");

	InitDeck(ctx->screen);
	ctx->game.Initialize(ctx->screen);

	for (int i=0; i < ctx->numPlayers; i++) {
		// Create Player X hand pile card region
		ctx->game.CreateRegion(
			i*2,
			CRD_VISIBLE|CRD_3D,
			CRD_DOALL,
			0,
			CRD_OSYMBOL,
			regions[i*2].x,
			regions[i*2].y,
			regions[i*2].x_offset,
			regions[i*2].x_offset);

		// Create Player X hand pile card region
		ctx->game.CreateRegion(
			i*2 + 1,
			CRD_VISIBLE|CRD_3D,
			CRD_DOALL,
			0,
			CRD_OSYMBOL,
			regions[i*2 + 1].x,
			regions[i*2 + 1].y,
			regions[i*2 + 1].x_offset,
			regions[i*2 + 1].x_offset);
	}
}

static void NewGame(BatailleCtx *ctx)
{
	// Empty the card regions from the previous game
	ctx->game.EmptyStacks();

 	// create then shuffle the deck
	ctx->game[0].NewDeck();
	ctx->game[0].Shuffle();

	// deal cards: split the deck as evenly as possible
	for (int card = 0; card < (52 / ctx->numPlayers); card ++) {
		for (int player = 1; player <  ctx->numPlayers; player++) {
			ctx->game[player*2].Push(ctx->game[0].Pop());
		}
	}

	for (int i = 0; i < ctx->numPlayers; i++) {
		printf("Player %d cards: %d\n", i, ctx->game[i*2].Size());
	}

    // initialize all card coordinates
	ctx->game.InitAllCoords();
}

int main(int argc, char *argv[])
{
	if( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	BatailleCtx TheCtx;

//	screen=SDL_SetVideoMode(800, 600, 32, SDL_SWSURFACE|SDL_HWPALETTE|SDL_FULLSCREEN);
	TheCtx.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE|SDL_HWPALETTE);
	if (TheCtx.screen == NULL) {
		printf("Unable to set %dx%d video: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	TheCtx.numPlayers = NUM_PLAYERS;

	InitializeCardsRegions(&TheCtx);
	NewGame(&TheCtx);
	TheCtx.game.DrawStaticScene();

	SDL_Event event;
	int done = 0;

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

	// perform cleaning up in here

	freeFont(TheCtx.font1);
	freeFont(TheCtx.font2);
	return 0;
}
