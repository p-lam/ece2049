/** deal.h
 *
 *  Created on: June 18, 2020
 *      Author: Prudence Lam
 */

#ifndef DEAL_H_
#define DEAL_H_

void populateDeck(Card* deck);
void shuffle(Card* deck, int cutNum);
int readVal(Card* deck);
void initHand(Player* player, Card* deck, Counter* counter);
int evalHand(Player* player);
void dealHand(Player* player, Card* deck, Counter* counter);
void displayCard(char suit, char face, int x, int y);
void resetHands(Player* player, Counter* counter);
void fullReset(Player* player, Counter* counter);

#endif
