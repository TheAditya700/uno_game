#include "uno_game.h"

int main()
{
    printf("Welcome to Uno!\n");

    int num_players;
    bool enteredOne = false;

    while (1)
    {
        printf("Enter the number of players: ");
        scanf("%d", &num_players);
        getchar();
        if (num_players == 1)
        {
            printf("Get some friends... :_(\n");
            enteredOne = true;
        }
        else if (num_players < MIN_PLAYERS || num_players > MAX_PLAYERS)
        {
            printf("Number of players has to be between %d and %d\n", MIN_PLAYERS, MAX_PLAYERS);
        }
        else
        {
            if (enteredOne)
            {
                printf("Wow, you got friends!\n");
            }
            break;
        }
    }

    initializeGame(num_players);

    enterPrompt("Press Enter to start the game...");

    int current_player_index = 0;
    while (1)
    {
        struct player *current_player = &players[current_player_index];

        handlePlayerTurn(current_player, num_players);

        if (current_player->hand_size == 0)
        {
            printf("%s wins!\n", current_player->name);
            break;
        }
        else
        {
            enterPrompt("Press Enter to go to next player...");
        }

        if (flags.clockwise)
        {
            current_player_index = (current_player_index + 1) % num_players;
        }
        else
        {
            current_player_index = (current_player_index - 1 + num_players) % num_players;
        }
    }

    for (int i = 0; i < num_players; i++)
    {
        freePlayer(&players[i]);
    }

    freePile(&drawPile);
    freePile(&discardPile);

    return 0;
}
