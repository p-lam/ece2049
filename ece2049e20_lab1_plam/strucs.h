/** strucs.h
 *
 *  Created on: June 18, 2020
 *      Author: Prudence Lam
 */

#ifndef STRUCS_H_
#define STRUCS_H_

#include "main.h"

typedef struct card
{
    char face; //A, 2, 3, 4, 5, 6, 7, 8, 9, X, J, Q, K
    char suit; //H, D, C, S
    char val; //1, 2, 3, 4, 5, 6, 7, 8, 9, 10
} Card;

typedef struct player
{
    int hand[MAX_CARDS];
    unsigned int coins; //Amount of coins of player
    int bet; //Amount player bets
    int sum; //Sum of player hand
    char turn; //If it is the player's turn
} Player;

typedef struct counter
{
    unsigned char p; //Number of cards in player hand
    unsigned char c; //Number of cards in CPU and
    unsigned char d; //Determines card in deck
    unsigned int yp; //Y parameter to display player cards
    unsigned int yc; //Y parameter to display CPU cards
} Counter;

#endif
