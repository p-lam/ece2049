/** note.h
 *
 *  Created on: July 4, 2020
 *      Author: Prudence Lam
 */


#ifndef NOTE_H
#define NOTE_H

/* Structure to define a note in a song. Each note has a set pitch, length, and LED
 * that lights up when it is played.
 */
typedef struct {
    unsigned int pitch;
    unsigned int duration;
    char led;
} Note;

#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_F4s 370
#define NOTE_G4s 415
#define NOTE_A4 440
#define NOTE_A4s 466
#define NOTE_B4 494
#define NOTE_C5s 554
#define NOTE_D5 587
#define NOTE_D5s 622
#define NOTE_A5 880
#define NOTE_G5 784
#define NOTE_G5s 831
#define NOTE_A5s 932
#define NOTE_B5 988
#define NOTE_D6 1175
#define NOTE_D6s 1245
#define NOTE_E6 1318
#define NOTE_F6s 1480
#define NOTE_G6 1568
#define NOTE_A6 1760
#define NOTE_B6 1975

#endif
