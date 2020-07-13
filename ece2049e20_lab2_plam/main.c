/** Program for a simplified Guitar Hero on the MSP430. A buzzer, a Sharp 128x128 LCD screen, and
 *  four external LEDs and buzzers are used for implementation. Only one song (astronomia) can be played.
 *  Once the song begins, the user is prompted to press the button with the color or position corresponding
 *  to the LED that lights up. Any mistakes, including both wrong and missed notes, are reflected in the
 *  final score. If there are more than ten mistakes, the song ends.
 *
 *  Created on: June 18, 2020
 *      Author: Prudence Lam
 */

#include <msp430.h>
#include "peripherals.h"
#include "note.h"
#include "songs.h"
#include "utils/ustdlib.h"
#include "utils/debug_assert.h"

typedef enum {START, TEST, COUNTDOWN, PLAY, WIN, LOSE} gameStates;

//Function Prototypes
void swDelay(char numLoops);
void startTimerA2(void);
void playTune(Note tune[],int size);
void Welcome(void);
int playSong(Note song[], int size);
int countdown(int elapsed_time);
void playerWin(void);
void playerLose(void);
void resetGlobals(void);

//Global time count, storing number of timer ticks at 0.005 s/tick
volatile unsigned long timer = 0;

#pragma vector = TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR(void) {
    timer++;
}

//Global variables
unsigned long previous_time = 0; //Time mark for reference
int current_note = 0; //Index in songs
char loop; //Counter to determine if the intro song needs to be played
int count = 0; //Counter for the count down screen
int miss = 0; //Missed notes
int error = 0; //Wrong notes
int a_score = ARRAY2_SIZE(astronomia); //Total possible score. Will decrement provided errors.


void main(void)
{
    gameStates state = START;
    char ebutton_state; //Stores state of external buttons
    char lbutton_state; //Stores state of Launchpad board buttons

    WDTCTL = WDTPW | WDTHOLD; //Stop watchdog timer
    _BIS_SR(GIE);  //Enable global interrupt

    //System initialization
    initButtons();
    initLeds();
    initLaunchpadButtons();
    initLaunchpadLeds();
    configDisplay();

    startTimerA2(); //Start Timer

    while (1)
    {
        switch(state) { //Enter state machine
        case START:
            resetGlobals();
            Welcome();
            state = TEST;
            break;
        case TEST:
            lbutton_state = readLaunchpadButtons(); //Read Launchpad buttons
            ebutton_state = readButtons(); //Read external buttons
            configLeds(ebutton_state);
            if (lbutton_state & BUTTON_RIGHT) { //If right Launchpad button is pressed, start game
                Graphics_clearDisplay(&g_sContext);
                previous_time = timer;
                state = COUNTDOWN;
            }
            break;
        case COUNTDOWN:
            BuzzerOff();
            if (countdown(timer - previous_time) == 1) { //Displays a count down
                Graphics_clearDisplay(&g_sContext);
                swDelay(1);
                previous_time = timer;
                state = PLAY;
            }
            break;
        case PLAY:
            if (playSong(astronomia, ARRAY2_SIZE(astronomia)) == 1) {
                swDelay(3);
                state = WIN;
            } else if (playSong(astronomia, ARRAY2_SIZE(astronomia)) == 2) {
                swDelay(3);
                state = LOSE;
            }
        break;
        case WIN:
            playerWin();
            state = START;
        break;
        case LOSE:
            playerLose();
            state = START;
        break;
        }
    }
}

/*
 * Starts the timer
 */
void startTimerA2(void) {
    TA2CTL = TASSEL_1 | ID_0 | MC_1; //Using ACLK, divide by 1, UP mode
    TA2CCR0 = 163; //Max count = 32768 Hz * 0.005 s - 1
    TA2CCTL0 = CCIE; //Enable capture/compare interrupt
}

/*
 * Displays welcome screen and plays intro song to test the buzzer
 */
void Welcome(void) {
    if (loop == 0) {
        playTune(start_up,ARRAY_SIZE(start_up));
        loop++;
    }
    Graphics_drawStringCentered(&g_sContext, "MSP430 Hero",       AUTO_STRING_LENGTH, 64, 55, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Push Right to Start", AUTO_STRING_LENGTH, 64, 65, TRANSPARENT_TEXT);

    Graphics_flushBuffer(&g_sContext);
}

/*
 * Plays a song on the buzzer
 */
void playTune(Note tune[],int size) {
    for (current_note = 0; current_note < size; current_note++) { //Index through the song

        while (timer - previous_time < tune[current_note].duration) { //Switches on the buzzer and LEDs during note duration
            BuzzerOn(tune[current_note].pitch);
            configLeds(tune[current_note].led);
        }
        previous_time = timer; //Get time mark
    }
    configLeds(0); //Switch off LEDs and buzzer once song is complete
    BuzzerOff();
}

/*
 * Displays count down screen. Uses the timer to count down from 3 to 1.
 * It is configured to switch display on the LCD screen every second.
 */
int countdown(int elapsed_time) {
    if (count == 0 && elapsed_time > 0) {
        Graphics_drawStringCentered(&g_sContext, "3", 1, 64, 64, TRANSPARENT_TEXT);
        configLeds(GREEN);
        Graphics_flushBuffer(&g_sContext);
        count++;
    } else if (count == 1 && elapsed_time > 200) {
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "2", 1, 64, 64, TRANSPARENT_TEXT);
        configLeds(YELLOW);
        Graphics_flushBuffer(&g_sContext);
        count++;
    } else if (count == 2 && elapsed_time > 400) {
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "1", 1, 64, 64, TRANSPARENT_TEXT);
        configLeds(RED);
        Graphics_flushBuffer(&g_sContext);
        count++;
    } else if (count == 3 && elapsed_time > 600) {
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "START", 5, 64, 64, TRANSPARENT_TEXT);
        configLeds(BLUE);
        Graphics_flushBuffer(&g_sContext);
        swDelay(2);
        return 1;
    }
    return 0;
}

/*
 * Plays a song on the buzzer. Prompts user to press the correct button according to
 * theLED. LEDs light up first. If the correct button is pressed, the correct
 * note is played. Otherwise, an off-sounding note is played. Points are deducted if
 * the wrong button or if no buttons are pressed during a note.
 */
int playSong(Note song[], int size) {
    char button_state;
    int pass = 0;

    for (current_note = 0; current_note < size; current_note++) { //Index through the song
        pass = 0;

        while (timer - previous_time < song[current_note].duration) { //Read the state of the buttons while note is played
            configLeds(song[current_note].led); //Set LEDs to notes
            button_state = readButtons();
                if (button_state == song[current_note].led) { //If the correct button is pressed
                    setLaunchpadLeds(0x01); //Enable the green Launchpad LED
                    setLaunchpadLeds(0x00);
                    BuzzerOn(song[current_note].pitch); //Play correct note
                    pass = 1;
                } else if (button_state != song[current_note].led && button_state != 0) { //If the wrong button is pressed
                    BuzzerOn(NOTE_G6); //Play bad note
                    pass = 2;
                }
            }

            if (pass == 2) { //Make note of error if wrong button is pressed
                error++;
            } else if (pass == 0 && button_state == 0 && miss < 10) { //Make sure miss is not off by one
                miss++; //Keep track of missed notes
            }

            if ((error + miss) >= 10) { //If total errors are greater than 10
                BuzzerOff();
                configLeds(0);
                return 2; //Break out of loop
            }

        previous_time = timer; //Get time mark
    }
    configLeds(0);
    BuzzerOff();
    return 1;
}

/*
 * Displays win screen, including total score calculated based on number of correct notes pressed
 */
void playerWin(void) {

    unsigned char str[3]; //Create buffer to store and display score data
    a_score -= (error + miss);
    Graphics_drawStringCentered(&g_sContext, "Song Complete!", AUTO_STRING_LENGTH, 64, 45, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Score (/24):", AUTO_STRING_LENGTH, 64, 65, TRANSPARENT_TEXT);
    usnprintf(str,3,"%d",a_score);
    Graphics_drawStringCentered(&g_sContext, str, 3, 64, 75, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);

    previous_time = timer;
    playTune(win, ARRAY3_SIZE(win)); //Play celebratory song

    swDelay(10); //Write delay to allow time to read screen
}

/*
 * Displays lose screen, including total errors and missed notes
 */
void playerLose(void) {

    unsigned char str[3];
    unsigned char str2[3];
    a_score -= (error + miss);
    Graphics_drawStringCentered(&g_sContext, "Try again", AUTO_STRING_LENGTH, 64, 35, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Missed notes:", AUTO_STRING_LENGTH, 64, 55, TRANSPARENT_TEXT);
    usnprintf(str, 3, "%d", miss);
    Graphics_drawStringCentered(&g_sContext, str, 2, 64, 65, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Wrong notes:", AUTO_STRING_LENGTH, 64, 75, TRANSPARENT_TEXT);
    usnprintf(str2, 3, "%d", error);
    Graphics_drawStringCentered(&g_sContext, str2, 2, 64, 85, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);

    previous_time = timer;
    playTune(shut_down, ARRAY4_SIZE(shut_down)); //Play sad song

    swDelay(10); //Write delay to allow time to read screen
}

/*
 * Resets all global variables
 */
void resetGlobals() {
    previous_time = 0;
    count = 0;
    miss = 0;
    error = 0;
    a_score = ARRAY2_SIZE(astronomia);
    Graphics_clearDisplay(&g_sContext);
}

/*
 * Software delay. Performs useless loops to waste a bit of time
 * Input: numLoops = number of delay loops to execute
 * Output: none
 */
void swDelay(char numLoops)
{
	volatile unsigned int i,j;	// volatile to prevent removal in optimization
			                    // by compiler. Functionally this is useless code

	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}
