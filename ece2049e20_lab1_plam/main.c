/**
 * Program to simulate a simplified one-player version of Blackjack on the MSP430
 * Player wins if they have the most coins. Game ends after three rounds
 *
 *  Created on: June 18, 2020
 *      Author: Prudence Lam
 */

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"
#include "utils/debug_assert.h"
#include "main.h"
#include "strucs.h"
#include "deal.h"

void swDelay(char numLoops);

void main(void) {

    gameStates state = START;
    int cutNum = 0; //Number to cut the deck
    int numRounds = 1; //Number of rounds played
    Card deck[DECK_SIZE];
    Player player[MAX_PLAYERS] = {{{0,0,0,0,0,0,0,0,0,0,0},4,0,0,0}, //player[0] = human player
                                 {{0,0,0,0,0,0,0,0,0,0,0},4,0,0,0}}; //player[1] = CPU
    Counter counter = {0,0,0,35,35};

    //Frames for display
    unsigned char buffer[3];
    buffer[0] = '>';
    buffer[2] = '<';
    unsigned char buffer2[3];
    buffer2[0] = '>';
    buffer2[2] = '<';

    int h_center = LCD_HORIZONTAL_MAX/2; //X param for center of LCD screen

    WDTCTL = WDTPW | WDTHOLD;

    //Initializations
    initLaunchpadButtons();
    initLaunchpadLeds();
    configDisplay();
    Graphics_clearDisplay(&g_sContext);

    while (1)
        {
            char button_state = readLaunchpadButtons(); //Reads the state of the buttons
            switch(state)
            {
                case START:
                    //Things to write to the display
                    Graphics_drawStringCentered(&g_sContext, "MSP430",  AUTO_STRING_LENGTH, h_center, 45, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Blackjack",  AUTO_STRING_LENGTH, h_center, 55, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Push to Start", AUTO_STRING_LENGTH, h_center, 75, TRANSPARENT_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    resetLaunchpadLeds();

                    while(button_state == 0) //While no buttons are pressed
                    {
                        button_state = readLaunchpadButtons(); //Read the state of the button

                        if (button_state != 0) { //If any of the buttons are pressed
                            Graphics_clearDisplay(&g_sContext);
                            state = CUT; //Next state
                        }
                    }
                    break;
                case CUT: //Cut and shuffle deck
                    //Things to write to the display
                    Graphics_drawStringCentered(&g_sContext, "Select Cut:",  AUTO_STRING_LENGTH, h_center, 45, TRANSPARENT_TEXT);
                    buffer[1] = cutNum + '0';
                    Graphics_drawStringCentered(&g_sContext, buffer, 3, h_center, 55, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);

                    state = READCUT;
                    break;
                case READCUT: //Reads cut number
                    button_state = readLaunchpadButtons();

                    if (button_state & BUTTON_RIGHT) {
                        Graphics_clearDisplay(&g_sContext);
                        populateDeck(deck);
                        shuffle(deck, cutNum);
                        state = GAMBLE;
                    } else {
                        cutNum = (cutNum + 1) % 10; //Cycles between 1-9
                        state = CUT;
                    }
                    break;
                case GAMBLE: //Shows player and CPU coins. Allows player to choose bet.
                    //Things to write to the display
                    Graphics_drawStringCentered(&g_sContext, "Your coins:",  AUTO_STRING_LENGTH, 55, 35, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "CPU coins:",  AUTO_STRING_LENGTH, 55, 45, TRANSPARENT_TEXT);
                    buffer[1] = player[0].coins + '0';
                    buffer2[1] = player[1].coins + '0';
                    Graphics_drawStringCentered(&g_sContext, buffer, 3, 95, 35, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, buffer2, 3, 95, 45, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Select Bet:",  AUTO_STRING_LENGTH, h_center, 65, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "All", AUTO_STRING_LENGTH, 95, 115, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Half", AUTO_STRING_LENGTH, 35, 115, TRANSPARENT_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    state = READGAMBLE;
                    break;
                case READGAMBLE:
                    button_state = readLaunchpadButtons();
                    player[1].bet = player[1].coins / 2; //CPU will always bet half their coins

                    if (button_state & BUTTON_RIGHT) { //If right button is pressed
                        Graphics_clearDisplay(&g_sContext);
                        player[0].bet = player[0].coins; //Player bets all their coins
                        state = DEAL;
                    } else if (button_state & BUTTON_LEFT) { //If left button is pressed
                        Graphics_clearDisplay(&g_sContext);
                        player[0].bet = player[0].coins / 2; //Player bets half their coins
                        state = DEAL;
                    } else {
                        state = READGAMBLE;
                    }
                    break;
                case DEAL: //Deals two cards to player and one to CPU
                    //Things to write to the display
                    Graphics_drawStringCentered(&g_sContext, "Round",  AUTO_STRING_LENGTH, 35, 10, TRANSPARENT_TEXT);
                    buffer[1] = numRounds + '0';
                    Graphics_drawStringCentered(&g_sContext, buffer,  3, 65, 10, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "of 3",  AUTO_STRING_LENGTH, 95, 10, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Player:",  AUTO_STRING_LENGTH, 35, 25, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "CPU:",  AUTO_STRING_LENGTH, 95, 25, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Hit", AUTO_STRING_LENGTH, 95, 115, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Stay", AUTO_STRING_LENGTH, 35, 115, TRANSPARENT_TEXT);

                    initHand(player,deck,&counter); //Initialize player and CPU hands
                    Graphics_flushBuffer(&g_sContext);
                    state = PLAYER_BET;
                    break;
                case PLAYER_BET: //Player can choose to hit or stay
                    resetLaunchpadLeds();
                    button_state = readLaunchpadButtons();
                    player[0].turn = TRUE; //It is the player's turn

                    P4OUT |= BIT7; //Enable right button as turn indicator

                    if (button_state & BUTTON_RIGHT) {
                          if (counter.yp > 95) { //Reset display if too many cards on the screen
                              Graphics_clearDisplay(&g_sContext);
                              counter.yp = 35, counter.yc = 35; //Reset counters for y parameter for displaying cards
                              Graphics_drawStringCentered(&g_sContext, "Playing Table (Cont'd)",  AUTO_STRING_LENGTH, 65, 10, TRANSPARENT_TEXT);
                              Graphics_drawStringCentered(&g_sContext, "Player:",  AUTO_STRING_LENGTH, 35, 25, TRANSPARENT_TEXT);
                              Graphics_drawStringCentered(&g_sContext, "CPU:",  AUTO_STRING_LENGTH, 95, 25, TRANSPARENT_TEXT);
                              Graphics_drawStringCentered(&g_sContext, "Hit", AUTO_STRING_LENGTH, 95, 115, TRANSPARENT_TEXT);
                              Graphics_drawStringCentered(&g_sContext, "Stay", AUTO_STRING_LENGTH, 35, 115, TRANSPARENT_TEXT);
                          }

                          dealHand(player,deck,&counter); //Deal card and read value of hand
                          player[0].sum = evalHand(player);

                          Graphics_flushBuffer(&g_sContext);
                          swDelay(1);

                          if (player[0].sum <= BUST) { //If player has less than 10 cards and sum of hand is less than 21
                             state = CPU_BET;
                          } else if (player[0].sum > BUST) { //If player has more than 10 cards or sum of hand is greater than 21
                             Graphics_clearDisplay(&g_sContext);
                             state = LOSE;
                          }
                   } else if (button_state & BUTTON_LEFT) { //Player decides to stay
                       player[0].sum = evalHand(player);
                       state = CPU_BET;
                   } else { //No buttons are pressed
                       state = PLAYER_BET;
                   }
                   break;
                case CPU_BET: //CPU bets if sum is less than 17, stays otherwise
                    resetLaunchpadLeds();

                    if (player[1].sum < CPU_MAX) { //If CPU hand is less than 17

                        player[0].turn = FALSE; //It is the CPU's turn
                        P1OUT |= BIT0; //Enable left LED as turn indicator

                        dealHand(player,deck,&counter);
                        player[1].sum = evalHand(player);

                        Graphics_flushBuffer(&g_sContext);
                        swDelay(1);

                        if (player[1].sum <= BUST) { //If CPU hand is not full and has not bust
                            state = PLAYER_BET;
                        } else if (player[1].sum > BUST) {
                            Graphics_clearDisplay(&g_sContext);
                            state = WIN;
                        }
                    } else { //If value of CPU hand is greater than or equal to 17
                        if (player[1].sum < player[0].sum) { //Player wins if hand is greater than CPU
                            Graphics_clearDisplay(&g_sContext);
                            state = WIN;
                        } else if (player[1].sum == player[0].sum) { //Draws if hands are equal
                            Graphics_clearDisplay(&g_sContext);
                            state = DRAW;
                        } else { //Player loses if hand is less than CPU
                            Graphics_clearDisplay(&g_sContext);
                            state = LOSE;
                        }
                   }
                   break;
                case WIN:
                    resetLaunchpadLeds();

                    Graphics_drawStringCentered(&g_sContext, "Won Round!", AUTO_STRING_LENGTH, h_center, 45, TRANSPARENT_TEXT);

                    P4OUT |= BIT7; //Enable right LED

                    if (numRounds < 3) { //If three rounds haven't been played
                        Graphics_drawStringCentered(&g_sContext, "Continue?", AUTO_STRING_LENGTH, h_center, 55, TRANSPARENT_TEXT);
                        Graphics_drawStringCentered(&g_sContext, "Yes", AUTO_STRING_LENGTH, 95, 115, TRANSPARENT_TEXT);
                        Graphics_drawStringCentered(&g_sContext, "No", AUTO_STRING_LENGTH, 35, 115, TRANSPARENT_TEXT);

                        button_state = readLaunchpadButtons();
                        Graphics_flushBuffer(&g_sContext);

                        if (button_state & BUTTON_RIGHT) { //If player wants to continue the match
                           Graphics_clearDisplay(&g_sContext);
                             player[0].coins += player[1].bet; //Player gains CPU bet
                             player[1].coins -= player[1].bet; //CPU loses coins
                           numRounds++; //Next round
                           state = RESET;
                         } else if (button_state & BUTTON_LEFT) { //If player discontinues the match
                             player[0].coins += player[1].bet; //Player gains CPU bet coins
                             player[1].coins -= player[1].bet; //CPU loses coins
                           Graphics_clearDisplay(&g_sContext);
                           state = TOTAL_RESET;
                         } else {
                           state = WIN;
                         }
                    } else {
                        player[0].coins += player[1].bet; //Player gains CPU bet coins
                        player[1].coins -= player[1].bet;
                        Graphics_flushBuffer(&g_sContext);
                        swDelay(1);
                        Graphics_clearDisplay(&g_sContext);
                        state = TOTAL_RESET;
                    }
                    break;
                case LOSE:
                    resetLaunchpadLeds();
                    Graphics_drawStringCentered(&g_sContext, "Lost Round!", AUTO_STRING_LENGTH, h_center, 45, TRANSPARENT_TEXT);

                    P1OUT |= BIT0; //Enable left LED

                    if (numRounds < 3) { //If three rounds haven't been played
                    Graphics_drawStringCentered(&g_sContext, "Continue?", AUTO_STRING_LENGTH, h_center, 55, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Yes", AUTO_STRING_LENGTH, 95, 115, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "No", AUTO_STRING_LENGTH, 35, 115, TRANSPARENT_TEXT);

                    Graphics_flushBuffer(&g_sContext);
                    button_state = readLaunchpadButtons();

                        if (button_state & BUTTON_RIGHT) {
                           player[0].coins -= player[0].bet; //Player loses bet coins
                           player[1].coins += player[0].bet; //CPU gains coins
                           numRounds++;
                           Graphics_clearDisplay(&g_sContext);
                           state = RESET;
                         } else if (button_state & BUTTON_LEFT) {
                           player[0].coins -= player[0].bet;
                           player[1].coins += player[0].bet;
                           Graphics_clearDisplay(&g_sContext);
                           state = TOTAL_RESET;
                         } else {
                           state = LOSE;
                         }
                    } else {
                        player[0].coins -= player[0].bet;
                        player[1].coins += player[0].bet;
                        Graphics_flushBuffer(&g_sContext);
                        swDelay(1);
                        Graphics_clearDisplay(&g_sContext);
                        state = TOTAL_RESET;
                    }
                    break;
                case DRAW:
                    resetLaunchpadLeds();
                    Graphics_drawStringCentered(&g_sContext, "Draw", AUTO_STRING_LENGTH, h_center, 45, TRANSPARENT_TEXT);

                    P4OUT |= BIT7; P1OUT |= BIT0; //Enable both LEDS

                    if (numRounds < 3) {
                    Graphics_drawStringCentered(&g_sContext, "Continue?", AUTO_STRING_LENGTH, h_center, 55, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "Yes", AUTO_STRING_LENGTH, 95, 115, TRANSPARENT_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "No", AUTO_STRING_LENGTH, 35, 115, TRANSPARENT_TEXT);

                    Graphics_flushBuffer(&g_sContext);
                    button_state = readLaunchpadButtons();

                        if (button_state & BUTTON_RIGHT) { //Players keep their coins
                            Graphics_clearDisplay(&g_sContext);
                            numRounds++;
                            state = RESET;
                        } else if (button_state & BUTTON_LEFT) {
                            Graphics_clearDisplay(&g_sContext);
                            state = TOTAL_RESET;
                        } else {
                            state = DRAW;
                        }
                    } else {
                        Graphics_flushBuffer(&g_sContext);
                        swDelay(1);
                        Graphics_clearDisplay(&g_sContext);
                        state = TOTAL_RESET;
                    }
                    break;
                case RESET: //If "continue" is selected and 3 rounds have not been played
                   resetLaunchpadLeds();
                   resetHands(player,&counter);
                   player[1].sum = 0;
                   state = CUT;
                   break;
                case TOTAL_RESET: //Resets player stats
                   resetLaunchpadLeds();

                   if (player[0].coins > player[1].coins) { //If player has more coins
                       P4OUT |= BIT7; //Enable left LED
                       Graphics_drawStringCentered(&g_sContext, "You Won!", AUTO_STRING_LENGTH, h_center, 45, TRANSPARENT_TEXT);
                   } else if (player[0].coins < player[1].coins) { //If CPU has more coins
                       P1OUT |= BIT0; //Enable right LED
                       Graphics_drawStringCentered(&g_sContext, "You Lost!", AUTO_STRING_LENGTH, h_center, 45, TRANSPARENT_TEXT);
                   } else { //Draw
                       P4OUT |= BIT7; P1OUT |= BIT0; //Turn both LEDs on
                       Graphics_drawStringCentered(&g_sContext, "Draw!", AUTO_STRING_LENGTH, h_center, 45, TRANSPARENT_TEXT);
                   }

                   Graphics_flushBuffer(&g_sContext);
                   swDelay(1);
                   fullReset(player,&counter);
                   numRounds = 1;
                   Graphics_clearDisplay(&g_sContext);

                   state = START;
                   break;
            }
        }
}

/**
 * Software delay. Performs useless loops to waste time.
 */
void swDelay(char numLoops) {
    volatile unsigned int i,j;  // Volatile to prevent removal in optimization
                                // by compiler.
    for (j=0; j<numLoops; j++)
    {
        i = 50000 ;                 // SW Delay
        while (i > 0)               // could also have used while (i)
           i--;
    }
}
