#ifndef TABLE_H
#define TABLE_H

void drawTable();
void redrawDealer();
void redrawSeat(unsigned char p);
unsigned char layoutMoveMenu();
unsigned char defaultStop();
void drawMoveCursor(unsigned char stop, bool on);
char *stopMove(unsigned char stop);
void showChosenMove(unsigned char stop);
void drawStatusTimeLeft();
void drawGameStatus();

#endif /* TABLE_H */
