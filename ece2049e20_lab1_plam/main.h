/** main.h
 *
 * Created on: June 18, 2020
 *      Author: Prudence Lam
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "peripherals.h"
#include "utils/debug_assert.h"

#define BUTTON_RIGHT 0x01
#define BUTTON_LEFT 0x02
#define DECK_SIZE 52
#define NSUITS 4
#define NFACES 13
#define MAX_PLAYERS 2 //CPU and Player
#define MAX_CARDS 11 //{2 2 2 2 3 A A A A 3 3}
#define CPU_MAX 17 //CPU must bet if hand is less than 17
#define BUST 21
#define TRUE 1 //Boolean types
#define FALSE 0

typedef enum gameStates { // Declare game states
    START = 0,
    GAMBLE = 1,
    READGAMBLE = 2,
    CUT = 3,
    READCUT = 4,
    DEAL = 5,
    PLAYER_BET = 6,
    CPU_BET = 7,
    WIN = 8,
    LOSE = 9,
    DRAW = 10,
    RESET = 11,
    TOTAL_RESET = 12,
} gameStates;

#endif
