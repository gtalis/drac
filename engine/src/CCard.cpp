#include <CCard.h>
#include <iostream>


/*
enum eSuit  { Spades = 0, Hearts = 1, Clubs = 2, Diamonds = 3 };
enum eValue { Ace = 1, Two = 2, Three = 3, Four = 4, Five = 5, Six = 6, Seven = 7,
              Eight = 8, Nine = 9, Ten = 10, Jack = 11, Queen = 12, King = 13 };
*/

static const char* SuitStr[] = { "Spades", "Hearts", "Clubs", "Diamonds" };
static const char* ValueStr[] =
{
	"invalid",
	"Ace",
	"Two",
	"Three",
	"Four",
	"Five",
	"Six",
	"Seven",
	"Eight",
	"Nine",
	"Ten",
	"Jack",
	"Queen",
	"King"
};


const char* CCard::Suit_str()
{
	int suit = Suit();
	return SuitStr[suit];
}

const char* CCard::Rank_str()
{
	int rank = Rank();
	return ValueStr[rank];
}