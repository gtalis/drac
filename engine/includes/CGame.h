/*
	filename:		CGame.h
	author:			Rico Roberto Zuñiga
	last modified:	9/12/03

	description:	Interface for game data strucuture.
*/

#ifndef CGAME_H
#define CGAME_H

#include <vector>
#include <string>

#include "CCardRegion.h"
#include "SDL_ttf.h"

typedef std::vector<CCardRegion>::iterator rVI;

struct DRAGCARD
{
	int x;
	int y;
	int width;
	int height;
};

class CTextField
{
	SDL_Surface *textSurface;
	SDL_Surface *destSurface;
	SDL_Color 	fontColor;
	std::string	text;
	SDL_Rect offset;
	TTF_Font *font;

public:
	CTextField(int x, int y, SDL_Surface *dest, TTF_Font *font);
	~CTextField() {}
	void setTextFieldText(std::string text);
	void Draw();
	void FreeTextField();
};

class CGame : public std::vector<CCardRegion>
{
public:

	CGame() {}
	~CGame() { SDL_FreeSurface(background); }

	void Initialize(SDL_Surface *s)
 	{
  		screen = s;
		background = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 32, 0, 0, 0, 0);
		//initialize more values here

		InitText();
    }

	void Clear()	{ this->clear(); }
	int Size()		{ return this->size(); }

//----------------------------------------------------------------------------------------
//Defined in CGame.cpp

	void CreateRegion (int id, unsigned int attribs, unsigned int amode, int dmode, int symbol, int x, int y, int xoffset, int yoffset);
	bool DeleteRegion(CCardRegion *pRegion);

   	void EmptyStacks();
	void InitAllCoords();

	int InitText();
	void CreateTextField (int *id, int x, int y);
	void SetTextField(int id, std::string text);

//----------------------------------------------------------------------------------------
//Drag and Drop

	CCardRegion *OnMouseDown(int x, int y);

	bool InitDrag(int x, int y);
	bool InitDrag(CCardStack *CargoStack, int x, int y);

	void DoDrag(int x, int y);

	void DoDrop();
	void DoDrop(CCardRegion *DestRegion);

	CCardRegion *FindDropRegion(int Id, CCard card);
	CCardRegion *FindDropRegion(int Id, CCardStack stack);

	void ZoomCard(int &sx, int &sy, int &dx, int &dy, int width, int height, SDL_Surface *bg, SDL_Surface *fg);

	void DrawStaticScene();
	void DrawBackground();

//	void AnimateRegions(int Id);

	CCardRegion *GetBestStack(int x, int y, int width, int height, CCardStack *stack);
//----------------------------------------------------------------------------------------

private:

	CCardStack DragStack;
	CCardRegion *SourceRegion;

	SDL_Surface *screen;
	SDL_Surface *background;
	SDL_Surface *dragface;

	TTF_Font *textFont;
	std::vector <CTextField> textFields;

	DRAGCARD dcard;

	int oldx;
	int oldy;
};

#endif //CGAME_H
