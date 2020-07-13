/** deal.c
 *
 *  Created on: June 18, 2020
 *      Author: Prudence Lam
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "strucs.h"
#include "deal.h"


extern char suits[NSUITS] = {'H', 'D', 'C', 'S'};
extern char faces[NFACES] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'J', 'Q', 'K'};


/**
 * Fills the deck
 * Assigns face, suit, and value to each card
 */
void populateDeck(Card* deck) {
  int i,j,k;
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 13; j++) {
      deck[(i * 13) + j].face = faces[j]; //Assigns face
      deck[(i * 13) + j].suit = suits[i]; //Assigns suit
    }
  }
  for (k = 0; k < DECK_SIZE + 1; k++) { //Determines card value
    deck[k].val = readVal(deck);
  }
}


/**
 * Shuffles the deck
 */
void shuffle(Card* deck, int cutNum) {
  int i, random;
  Card temp;

  srand((unsigned)cutNum);
  for (i = 0; i < DECK_SIZE; i++) {
    //get random number between 0 and 51
    random = rand() % DECK_SIZE;
    //perform swap
    temp = deck[i];
    deck[i] = deck[random];
    deck[random] = temp;
  }
}


/**
 * Assigns a value from 1 to 11 to each card according to their face
 */
int readVal(Card* deck) {
  int i;
  for (i = 0; i < DECK_SIZE; i++) {
    if (deck[i].face == '2' || deck[i].face == '3' || deck[i].face == '4' || deck[i].face == '5'|| deck[i].face == '6'|| deck[i].face == '7'|| deck[i].face == '8'||deck[i].face == '9') {
      deck[i].val = deck[i].face - 48; //Keep in range by subtracting '0'
    }
      else if (deck[i].face == 'X'|| deck[i].face == 'J'|| deck[i].face == 'Q'||deck[i].face == 'K') {
      deck[i].val = 10;
    } else if (deck[i].face == 'A') {
      deck[i].val = 11; //Default value for Ace
    }
    }
  return 0;
  }


/**
 * Deals initial CPU and Player hands
 */
void initHand(Player* player, Card* deck, Counter* counter) {
    for (counter->p = 0; counter->p < 2; counter->p++) { //Deals two cards to player's hand
        displayCard(deck[counter->d].suit, deck[counter->d].face, 35, counter->yp); //Display card on LCD
        player[0].hand[counter->p] = deck[counter->d].val; //Writes value of card to hand
        counter->d++; //Increment counters
        counter->yp += 10;
    }
    for (counter->c = 0; counter->c < 1; counter->c++) { //Deals one card to cpu's hand
        displayCard(deck[counter->d].suit,deck[counter->d].face,95,counter->yc);
        player[1].hand[counter->c] = deck[counter->d].val;
        counter->d++;
        counter->yc += 10;
    }
}


/**
 * Determines if looking at player or CPU hand, then returns value of hand
 * Determines if Ace is counted as 11 or 1
 */
int evalHand(Player* player) { //to determine total value of hand and if any ace cards should be 1 or 11
    int i;
    int total = 0;
    int numAce = 0; //Counts number of aces present in hand
    for (i = 0; i < MAX_CARDS; i++) {
        if (player[0].turn == TRUE){
            if (player[0].hand[i] == 11) {
                numAce++;
            }
            total += player[0].hand[i];
        } else {
            if (player[1].hand[i] == 11) {
                numAce++;
            }
            total += player[1].hand[i];
        }
        while (total > 21 && numAce > 0) { //If there are aces present and the hand busts
            total -= 10; //Evaluate ace as 1
            numAce--;
        }
   }
   return total;
}


/**
 * Checks if it is the player's or the CPU's turn. Deals card to corresponding hand
 */
void dealHand(Player* player, Card* deck, Counter* counter) {
    if (player[0].turn == TRUE) { //If it is the player's turn
        displayCard(deck[counter->d].suit, deck[counter->d].face, 35, counter->yp); //Display card on LCD
        player[0].hand[counter->p] = deck[counter->d].val; //Write value to hand
        counter->d++;
        counter->p++;
        counter->yp += 10;
    } else { //If it is the CPU's turn
        displayCard(deck[counter->d].suit, deck[counter->d].face, 95, counter->yc);
        player[1].hand[counter->c] = deck[counter->d].val;
        counter->d++;
        counter->c++;
        counter->yc += 10;
    }
}


/**
 * Creates a buffer to display a string on the LCD
 */
void displayCard(char suit, char face, int x, int y) {
  unsigned char str[4];
      str[0] = suit;
      str[1] = '-';
      str[2] = face;
      str[3] = '\0';

  Graphics_drawStringCentered(&g_sContext, str, AUTO_STRING_LENGTH, x, y, TRANSPARENT_TEXT);
}


/**
 * Restores hands and counters to initial values
 */
void resetHands(Player* player, Counter* counter) {
    int i;
    for (i = 0; i < MAX_CARDS; i++) {
        player[0].hand[i] = 0;
        player[1].hand[i] = 0;
    }
    counter->p = 0; counter->c = 0; counter->d = 0; counter->yp = 35; counter->yc = 35;
}


/**
 * Resets all game data
 */
void fullReset(Player* player, Counter* counter) {
    int i;
    resetHands(player,counter);
    for (i = 0; i < 2; i++) {
    player[i].coins = 4; player[i].bet = 0; player[i].sum = 0; player[i].turn = 0;
    }
}
