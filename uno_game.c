#include "uno_game.h"

struct pile *drawPile;
struct pile *discardPile;

struct gameFlags flags;
struct player players[MAX_PLAYERS];

void initializePlayer(struct player *player)
{
    char pname[100];
    printf("Enter player name: ");
    scanf("%s", pname);

    strncpy(player->name, pname, sizeof(player->name));
    player->hand = (struct card *)malloc(MAX_HAND_SIZE * sizeof(struct card));
    player->hand_size = 0;
}

void initializeGame(int num_players)
{
    // Initialize and shuffle the draw pile
    initializeDrawPile();
    shufflePile(&drawPile);

    // Ensure the discard pile is initially empty
    discardPile = NULL;

    // Initialize players
    printf("\nPlayer name entry: \n");
    for (int i = 0; i < num_players; i++)
    {
        initializePlayer(&players[i]);

        // draw seven cards for each player
        struct player *current_player = &players[i];
        for (int j = 0; j < 7; j++)
        {
            struct card drawn_card = draw();
            current_player->hand[current_player->hand_size] = drawn_card;
            current_player->hand_size++;
        }
    }
    getchar();

    // takes one card from draw pile to discard pile to start the game
    struct card drawncard;
    do
    {
        struct pile *top = drawPile;

        drawncard = top->data;
        drawPile = top->next;

        // Free the memory for the discarded card
        free(top);

        struct pile *new_discard = (struct pile *)malloc(sizeof(struct pile));
        new_discard->data = drawncard;
        new_discard->next = discardPile;
        discardPile = new_discard;
    } while (drawncard.type != NUMBER);

    flags.draw2 = false;
    flags.draw4 = false;
    flags.skip = false;
    flags.clockwise = true;
    flags.canDrawAgain = true;
    flags.missedUno = false;
}

void initializeDrawPile()
{
    for (int color = BLUE; color <= YELLOW; color++)
    {
        for (int digit = 0; digit <= 9; digit++)
        {
            struct card new_card = {color, digit, NUMBER};
            addCardToDrawPile(new_card);
            addCardToDrawPile(new_card);
        }
        for (int special = SKIP; special <= DRAW_TWO; special++)
        {
            struct card new_card = {color, -1, special};
            addCardToDrawPile(new_card);
            addCardToDrawPile(new_card);
        }
    }

    for (int i = 0; i < 4; i++)
    {
        struct card new_card = {BLACK, -1, WILD};
        addCardToDrawPile(new_card);
    }

    for (int i = 0; i < 4; i++)
    {
        struct card new_card = {BLACK, -1, WILD_DRAW_FOUR};
        addCardToDrawPile(new_card);
    }
}

void addCardToDrawPile(struct card new_card)
{
    struct pile *new_pile = (struct pile *)malloc(sizeof(struct pile));
    new_pile->data = new_card;
    new_pile->next = drawPile;
    drawPile = new_pile;
}

void freePlayer(struct player *player)
{
    if (player->hand != NULL)
    {
        free(player->hand);
        player->hand = NULL;
    }
}

void shufflePile(struct pile **pile)
{
    // Shuffle the pile (draw pile or discard pile) using a random algorithm
    srand(time(NULL));
    for (int i = 0; i < 1000; i++)
    {
        struct pile *current = *pile;
        struct pile *temp = NULL;
        struct card temp_card;

        while (current != NULL)
        {
            int random_index = rand() % 100; // Adjust the range as needed
            temp = *pile;
            for (int j = 0; j < random_index; j++)
            {
                temp = temp->next;
            }

            // Swap the cards in the pile
            temp_card = temp->data;
            temp->data = current->data;
            current->data = temp_card;

            current = current->next;
        }
    }
}

struct card draw()
{
    if (drawPile == NULL)
    {
        if (discardPile == NULL)
        {
            printf("Both the draw pile and discard pile are empty. Unable to draw a card.\n");
            struct card empty_card = {BLUE, 0, NUMBER};
            return empty_card;
        }

        printf("Reshuffling the discard pile to replenish the draw pile...\n");

        // Re-shuffle the discard pile to create a new draw pile
        shufflePile(&discardPile);

        // Swap the draw pile and discard pile
        struct pile *temp = drawPile;
        drawPile = discardPile;
        discardPile = temp;
    }

    struct pile *top = drawPile;
    struct card drawncard = top->data;

    drawPile = top->next;
    free(top);

    return drawncard;
}

void discard(struct player *player, struct card card_to_discard)
{
    int card_index = -1;
    for (int i = 0; i < player->hand_size; i++)
    {
        if ((player->hand[i].color == card_to_discard.color &&
             player->hand[i].digit == card_to_discard.digit &&
             player->hand[i].type == card_to_discard.type) ||
            player->hand[i].type == WILD || player->hand[i].type == WILD_DRAW_FOUR)
        {
            card_index = i;
            break;
        }
    }

    if (card_index == -1)
    {
        return;
    }

    for (int i = card_index; i < player->hand_size - 1; i++)
    {
        player->hand[i] = player->hand[i + 1];
    }
    player->hand_size--;

    struct pile *new_discard = (struct pile *)malloc(sizeof(struct pile));
    new_discard->data = card_to_discard;
    new_discard->next = discardPile;
    discardPile = new_discard;
}

int isCardPlayable(struct card current_card, struct card to_play, bool consider_special)
{
    if (consider_special)
    {
        return (
            current_card.color == to_play.color ||
            ((current_card.digit == to_play.digit) && to_play.digit != -1) ||
            ((current_card.type == to_play.type) && (to_play.type != NUMBER)) ||
            to_play.type == WILD);
    }
    else
    {
        if (to_play.type != NUMBER)
        {
            return false;
        }
        else
        {
            return (
                current_card.color == to_play.color ||
                ((current_card.digit == to_play.digit) && to_play.digit != -1) ||
                ((current_card.type == to_play.type) && (to_play.type != NUMBER)) ||
                to_play.type == WILD);
        }
    }
}

void displayGameState(struct player *current_player, int num_players)
{
    clearScreen(); // Clear the screen

    // Display the current player's name
    printf("Current player: %s\n\n", current_player->name);

    // Display the current card at the top of the discard pile
    struct card top_card = discardPile->data;
    printf("Top card on discard pile: ");
    printCard(top_card);
    printf("\n\n");

    // Display the current player's hand and the number of cards in other players' hands
    printf("%s's hand (%d cards):\n", current_player->name, current_player->hand_size);
    for (int j = 0; j < current_player->hand_size; j++)
    {
        struct card card = current_player->hand[j];
        printCard(card);
        printf("\n");
    }
    printf("\n");

    for (int i = 0; i < num_players; i++)
    {
        if (&players[i] != current_player)
        {
            struct player *other_player = &players[i];
            printf("%s (%d cards)\n", other_player->name, other_player->hand_size);
        }
    }
}

void printCard(struct card card)
{
    switch (card.type)
    {
    case NUMBER:
        printf("%d-%s", card.digit, getColorName(card.color));
        break;
    case SKIP:
        printf("skip-%s", getColorName(card.color));
        break;
    case REVERSE:
        printf("reverse-%s", getColorName(card.color));
        break;
    case DRAW_TWO:
        printf("draw2-%s", getColorName(card.color));
        break;
    case WILD:
        printf("wild");
        break;
    case WILD_DRAW_FOUR:
        printf("draw4-wild");
        break;
    }
}

const char *getColorName(enum colors color)
{
    switch (color)
    {
    case BLUE:
        return "blue";
    case GREEN:
        return "green";
    case RED:
        return "red";
    case YELLOW:
        return "yellow";
    case BLACK:
        return "black";
    }
    return "";
}

void handlePlayerTurn(struct player *current_player, int num_players)
{
    displayGameState(current_player, num_players);

    // Check for the flags to determine the current effect
    if (flags.skip)
    {
        printf("Your turn is skipped due to the previous Skip card.\n");
        flags.skip = false; // Reset the skip flag
        return;
    }
    if (flags.draw2)
    {
        printf("You need to draw two cards due to the previous Draw Two card.\n");
        printf("Your turn is skipped.\n");
        for (int i = 0; i < 2; i++)
        {
            struct card drawn_card = draw();
            // Implement logic to add the drawn card to the player's hand
            current_player->hand[current_player->hand_size] = drawn_card;
            current_player->hand_size++;
        }
        flags.draw2 = false; // Reset the draw two flag
        return;
    }
    if (flags.draw4)
    {
        printf("You need to draw four cards due to the previous Draw Four card.\n");
        printf("Your turn is skipped.\n");
        for (int i = 0; i < 4; i++)
        {
            struct card drawn_card = draw();
            // Implement logic to add the drawn card to the player's hand
            current_player->hand[current_player->hand_size] = drawn_card;
            current_player->hand_size++;
        }
        flags.draw4 = false; // Reset the draw two flag
        return;
    }

    int num_playable_cards;
    struct card *playable_cards = checkPlayable(current_player, &num_playable_cards);

    if (num_playable_cards == 0)
    {
        if (flags.canDrawAgain)
        {
            flags.canDrawAgain = false;

            struct card drawn_card = draw();
            // Implement logic to add the drawn card to the player's hand
            current_player->hand[current_player->hand_size] = drawn_card;
            current_player->hand_size++;

            enterPrompt("No playable cards, drawing a card...\n");
            handlePlayerTurn(current_player, num_players);
            return;
        }
        else
        {
            flags.canDrawAgain = true;
            printf("\nNo playable cards, ending turn...\n");
            return;
        }
    }

    struct card chosen_card = getCardChoice(current_player, num_playable_cards, playable_cards);

    if (flags.missedUno)
    {
        flags.missedUno = false;
        return;
    }

    // Handle special card effects
    if (chosen_card.type == SKIP)
    {
        printf("Skipped the next player's turn.\n");
        flags.skip = true; // Set the skip flag for the next player
    }
    else if (chosen_card.type == DRAW_TWO)
    {
        printf("Next player needs to draw two cards.\n");
        flags.draw2 = true; // Set the draw two flag for the next player
    }
    else if (chosen_card.type == REVERSE)
    {
        printf("The direction of play is reversed.\n");
        flags.clockwise = !flags.clockwise; // Toggle the direction of play
    }
    else if (chosen_card.type == WILD)
    {
        chosen_card.color = chooseColor();
    }
    else if (chosen_card.type == WILD_DRAW_FOUR)
    {
        chosen_card.color = chooseColor();
        flags.draw4 = true;
    }

    discard(current_player, chosen_card);
}

struct card *checkPlayable(struct player *current_player, int *num_playable_cards)
{
    struct card *previous_card = &discardPile->data;
    struct card *playable_cards = (struct card *)malloc(current_player->hand_size * sizeof(struct card));
    *num_playable_cards = 0;

    for (int i = 0; i < current_player->hand_size; i++)
    {
        if (current_player->hand_size > 1)
        {
            if (isCardPlayable(*previous_card, current_player->hand[i], true))
            {
                // Add the playable card to the list
                playable_cards[*num_playable_cards] = current_player->hand[i];
                (*num_playable_cards)++;
            }
        }
        else
        {
            if (isCardPlayable(*previous_card, current_player->hand[i], false))
            {
                // Add the playable card to the list
                playable_cards[*num_playable_cards] = current_player->hand[i];
                (*num_playable_cards)++;
            }
        }
    }

    if (*num_playable_cards == 0)
    {
        for (int i = 0; i < current_player->hand_size; i++)
        {
            struct card current_card = current_player->hand[i];
            if (current_card.type == WILD_DRAW_FOUR)
            {
                playable_cards[*num_playable_cards] = current_card;
                (*num_playable_cards)++;
            }
        }
    }

    if (*num_playable_cards == 0)
    {
        free(playable_cards);
        return NULL;
    }

    // Allocate memory for the exact number of playable cards
    struct card *result = (struct card *)malloc(*num_playable_cards * sizeof(struct card));
    memcpy(result, playable_cards, *num_playable_cards * sizeof(struct card));
    free(playable_cards);

    return result;
}

struct card getCardChoice(struct player *current_player, int num_playable_cards, struct card *playable_cards)
{
    int card_choice = -1;
    bool checkUno;

    if (current_player->hand_size == 2)
    {
        checkUno = true;
    }
    else
    {
        checkUno = false;
    }

    if (playable_cards == NULL)
    {
        card_choice = -1; // Indicate that the player can't play any cards
    }
    else
    {
        // Display playable cards for the current player
        if (num_playable_cards > 0)
        {
            printf("\nPlayable cards for %s:\n", current_player->name);
            for (int i = 0; i < num_playable_cards; i++)
            {
                printCard(playable_cards[i]);
                printf(" [%d]\n", i);
            }
            printf("\n");
        }
        do
        {
            printf("Choose a card to play (0-%d): ", num_playable_cards - 1);

            if (checkUno)
            {
                char unoInput[10];
                scanf("%s", unoInput);
                getchar();

                if (strcmp(toUpper(unoInput), "UNO") == 0)
                {
                    printf("Uno called successfully!\n");
                    checkUno = false;
                    continue;
                }
                else
                {
                    printf("U forgot to call uno! Draw two cards...\n");
                    for (int i = 0; i < 2; i++)
                    {
                        struct card drawn_card = draw();
                        // Implement logic to add the drawn card to the player's hand
                        current_player->hand[current_player->hand_size] = drawn_card;
                        current_player->hand_size++;
                    }
                    flags.missedUno = true;
                    break;
                }
            }

            scanf("%d", &card_choice);
            getchar();
        } while (card_choice < 0 || card_choice >= num_playable_cards);
    }

    struct card chosen_card;

    if (card_choice == -1)
    {
        // No playable cards, create a placeholder card
        chosen_card = (struct card){BLUE, 0, NUMBER};
    }
    else
    {
        chosen_card = playable_cards[card_choice];
    }

    free(playable_cards);
    return chosen_card;
}

int chooseColor()
{
    int choice;
    while (1)
    {
        printf("Please choose a color (1: Blue, 2: Green, 3: Red, 4: Yellow): ");
        scanf("%d", &choice);
        getchar();
        switch (choice)
        {
        case 1:
        case 2:
        case 3:
        case 4:
            return choice;
        default:
            break;
        }
    }
}

char *toUpper(char *input)
{
    int i = 0;
    char x;
    do
    {
        x = *(input + i);

        if (x >= 'a' && x <= 'z')
        {
            *(input + i) = (char)(x - 32);
        }

        i++;
    } while (x != '\0');

    return input;
}

void freePile(struct pile **pile)
{
    struct pile *current = *pile;
    while (current != NULL)
    {
        struct pile *temp = current;
        current = current->next;
        free(temp);
    }
    *pile = NULL;
}

void enterPrompt(char *msg)
{
    printf("\n%s", msg);

    // Consume characters until Enter is pressed
    while (getchar() != '\n')
    {
        // Do nothing
    }
}