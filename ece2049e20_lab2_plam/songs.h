/** songs.h
 *
 *  Created on: July 4, 2020
 *      Author: Prudence Lam
 */

#include "note.h"

#ifndef SONGS_H_
#define SONGS_H_

#define BLUE 0x1
#define RED 0x2
#define YELLOW 0x4
#define GREEN 0x8
#define NULL 0x0

#define ARRAY_SIZE(start_up) (sizeof(start_up)/sizeof(start_up[0]))
#define ARRAY2_SIZE(astronomia) (sizeof(astronomia)/sizeof(astronomia[0]))
#define ARRAY3_SIZE(win) (sizeof(win)/sizeof(win[0]))
#define ARRAY4_SIZE(shut_down) (sizeof(shut_down)/sizeof(shut_down[0]))

Note win[] =  {{NOTE_B5, 50, BLUE},
              {NOTE_E6, 60, RED},
              {NOTE_F6s, 50, YELLOW},
              {NOTE_B6, 60, GREEN}};

Note shut_down[] = {{NOTE_G6, 64, GREEN},
                   {NOTE_D6, 64, YELLOW},
                   {NOTE_G5, 64, RED},
                   {NOTE_A5, 64, BLUE}};

Note start_up[] = {{NOTE_D6s, 64, RED},
                   {NOTE_A5s, 64, GREEN},
                   {NOTE_G5s, 64, YELLOW},
                   {NOTE_D6s, 64, RED},
                   {NOTE_A5s, 64, GREEN}};

Note astronomia[] = {{NOTE_B4, 77, GREEN},
                     {NOTE_A4, 77, YELLOW},
                     {NOTE_G4s, 77, RED},
                     {NOTE_E4, 77, YELLOW},
                     {NOTE_F4s, 150, BLUE},
                     {NOTE_C5s, 77, RED},
                     {NOTE_B4, 120, GREEN},
                     {NOTE_A4, 120, YELLOW},
                     {NOTE_G4s, 180, RED},
                     {NOTE_B4, 150, GREEN},
                     {NOTE_A4, 77, YELLOW},
                     {NOTE_G4s, 77, RED},
                     {NOTE_F4s, 150, BLUE},
                     {NOTE_A5, 77, GREEN},
                     {NOTE_G5s, 77, YELLOW},
                     {NOTE_A5, 77, GREEN},
                     {NOTE_G5s, 77, YELLOW},
                     {NOTE_A5, 77, GREEN},
                     {NOTE_F4s, 180, BLUE},
                     {NOTE_A5, 77, GREEN},
                     {NOTE_G5s, 77, YELLOW},
                     {NOTE_A5, 77, GREEN},
                     {NOTE_G5s, 77, YELLOW},
                     {NOTE_A5, 77, GREEN}};


#endif
