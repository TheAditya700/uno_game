#ifndef UNO_GAME_H
#define UNO_GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_HAND_SIZE 64
#define MIN_PLAYERS 2
#define MAX_PLAYERS 10

#ifdef _WIN32
#include <windows.h>
#define clearScreen() system("cls")
#else
#include <unistd.h>
#define clearScreen() printf("\033[2J\033[H")
#endif

enum colors
{
    BLUE = 1,
    GREEN,
    RED,
    YELLOW,
    BLACK
};

enum cardtype
{
    NUMBER = 1,
    SKIP,
    REVERSE,
    DRAW_TWO,
    WILD,
    WILD_DRAW_FOUR
};

struct card
{
    enum colors color;
    int digit;
    enum cardtype type;
};

struct player
{
    char name[50];
    struct card *hand;
    int hand_size;
};

struct pile
{
    struct card data;
    struct pile *next;
};

extern struct gameFlags
{
    bool draw2;
    bool draw4;
    bool skip;
    bool clockwise;
    bool canDrawAgain;
    bool missedUno;
} flags;

extern struct pile *drawPile;
extern struct pile *discardPile;

extern struct player players[MAX_PLAYERS];

void initializePlayer(struct player *player);
void initializeGame(int num_players);
void initializeDrawPile();
void addCardToDrawPile(struct card new_card);
struct card draw();
void discard(struct player *player, struct card card_to_discard);
void freePlayer(struct player *player);
void freePile(struct pile **pile);
void shufflePile(struct pile **pile);

int isCardPlayable(struct card current_card, struct card to_play, bool consider_special);
void handlePlayerTurn(struct player *current_player, int num_players);
struct card *checkPlayable(struct player *current_player, int *num_playable_cards);
struct card getCardChoice(struct player *current_player, int num_playable_cards, struct card *playable_cards);
int chooseColor();
void printCard(struct card card);
const char *getColorName(enum colors color);
void displayGameState(struct player *current_player, int num_players);
void enterPrompt(char *msg);
char *toUpper(char *input);

#endif
