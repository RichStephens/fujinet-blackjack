#ifdef _CMOC_VERSION_
#include <cmoc.h>
#include <coco.h>
#else
#include <stdlib.h>
#include <string.h>
#endif /* _CMOC_VERSION_ */

#include "misc.h"
#include "table.h"

/* Seat 0 is the client, bottom center. Seats 1-2 run up the left side and
   3-4 down the right, which align to the screen edge. */
#if WIDTH >= 40
#define SEAT_W 12
#define DEALER_W 14
static const unsigned char seatX[] = { 14, 1, 1, 27, 27 };
#else
#define SEAT_W 10
#define DEALER_W 12
static const unsigned char seatX[] = { 11, 0, 0, 22, 22 };
#endif
static const unsigned char seatY[] = { SEAT_BOTTOM_Y, SEAT_BOTTOM_Y, SEAT_TOP_Y, SEAT_TOP_Y, SEAT_BOTTOM_Y };

/* Seats used at each player count */
static const unsigned char seatOrder[5][5] = {
  { 0, 0, 0, 0, 0 }, { 0, 2, 0, 0, 0 }, { 0, 2, 3, 0, 0 },
  { 0, 1, 2, 3, 0 }, { 0, 1, 2, 3, 4 }
};

#if WIDTH >= 40
#define SURRENDER_LABEL "surr"
#else
#define SURRENDER_LABEL "fold"
#endif

#define DEALER_Y 2

// Atari card edges already space split hands apart
#define HAND_GAP (1 - CARD_EDGE)
#define MENU_END (WIDTH - 5 - STATUS_TIMER_WIDTH)

static unsigned char step;

static void centerText(unsigned char row, const char *s) {
  drawText((unsigned char)(WIDTH/2 - strlen(s)/2), row, s);
}

static const char *handLabel(Hand *hd) {
  switch (hd->result) {
    case RESULT_WIN: return "win";
    case RESULT_PUSH: return "push";
    case RESULT_BLACKJACK: return "bj";
    case RESULT_SURRENDER: return SURRENDER_LABEL;
  }
  if (hd->flags & FLAG_BLACKJACK)
    return "bj";
  if (hd->flags & FLAG_BUST)
    return "bust";
  if (hd->flags & FLAG_SURRENDERED)
    return SURRENDER_LABEL;
  if (hd->result == RESULT_LOSE)
    return "lose";

  tempBuffer[0] = 's';
  itoa(hd->total, tempBuffer + 1, 10);
  return (hd->flags & FLAG_SOFT) ? tempBuffer : tempBuffer + 1;
}

// Columns a hand of n cards takes, with room for its label when fanned
static unsigned char handWidth(unsigned char n) {
  static unsigned char w;

  if (step == 2)
    return n * 2 + CARD_EDGE * 2;
  w = n + 1 + CARD_EDGE * 2;
  return w < 4 ? 4 : w;
}

void redrawDealer() {
  static unsigned char n, w, dx, c;

  clearArea(WIDTH/2 - DEALER_W/2 - 1, DEALER_Y - 1, DEALER_W + 2, CARD_HEIGHT + 1);

  n = (unsigned char)(strlen(state.dealer) / 2);
  step = 2;
  w = handWidth(n);
  if (w > DEALER_W) {
    step = 1;
    w = handWidth(n);
  }
  dx = (unsigned char)(WIDTH/2 - w/2);

  strcpy(tempBuffer + 64, "dealer");
  if (n) {
    strcat(tempBuffer + 64, " ");
    if (state.dealerFlags & FLAG_BLACKJACK)
      strcat(tempBuffer + 64, "bj");
    else if (state.dealerFlags & FLAG_BUST)
      strcat(tempBuffer + 64, "bust");
    else {
      if (state.dealerFlags & FLAG_SOFT)
        strcat(tempBuffer + 64, "s");
      itoa(state.dealerTotal, tempBuffer, 10);
      strcat(tempBuffer + 64, tempBuffer);
    }
  }
  centerText(DEALER_Y - 1, tempBuffer + 64);

  for (c = 0; c < n; c++)
    drawCard(dx + c * step, DEALER_Y, (step == 1 && c) ? FANNED_CARD : FULL_CARD, state.dealer + c * 2);
}

static void drawSeat(unsigned char p) {
  static Player *pl;
  static Hand *hd;
  static unsigned char s, sx, sy, n, c, d, w, hx, len, right;
  static unsigned char shown[2];
  static uint16_t bet;

  pl = &state.players[p];
  s = seatOrder[state.playerCount - 1][p];
  sx = seatX[s];
  sy = seatY[s];
  right = s > 2;

  itoa(pl->purse, tempBuffer, 10);
  len = (unsigned char)strlen(tempBuffer);
  if (right) {
    drawChip(sx + SEAT_W - len - 1, sy - 3);
    drawText(sx + SEAT_W - len, sy - 3, tempBuffer);
  } else {
    drawChip(sx, sy - 3);
    drawText(sx + 1, sy - 3, tempBuffer);
  }

  // The bet, or the net once settled, sits on the table side of the seat
  bet = 0;
  for (c = 0; c < pl->handCount; c++)
    bet += pl->hands[c].bet;

  w = 0;
  if (state.round == ROUND_OVER && pl->move[0]) {
    strcpy(tempBuffer, pl->move);
  } else if (bet) {
    itoa(bet, tempBuffer, 10);
    w = 1;
  } else {
    tempBuffer[0] = 0;
  }
  len = (unsigned char)strlen(tempBuffer);
  if (len) {
    w += len;
    hx = !s ? sx + (SEAT_W - w) / 2 : right ? sx : sx + SEAT_W - w;
    if (w > len)
      drawChip(hx++, sy - 4);
    drawText(hx, sy - 4, tempBuffer);
  }

  len = (unsigned char)strlen(pl->name);
  hx = right ? sx + SEAT_W - len : sx;
  drawText(hx, sy - 2, pl->name);
  if (state.activePlayer == (int8_t)p)
    drawUnderline(hx, sy - 2, len);

  if (!pl->handCount) {
    if (pl->move[0] && state.round != ROUND_OVER)
      drawText(sx, sy - 1, pl->move);
    return;
  }
  if (!pl->hands[0].cards[0])
    return;

  // Full width cards if every hand fits, otherwise fan them all
  for (step = 2; ; step = 1) {
    w = (pl->handCount - 1) * HAND_GAP;
    for (c = 0; c < pl->handCount; c++) {
      shown[c] = (unsigned char)(strlen(pl->hands[c].cards) / 2);
      w += handWidth(shown[c]);
    }
    if (w <= SEAT_W || step == 1)
      break;
  }

  // Two long split hands can overflow even fanned: drop their oldest cards
  while (w > SEAT_W) {
    c = shown[1] > shown[0];
    w -= handWidth(shown[c]);
    w += handWidth(--shown[c]);
  }

  hx = (right && w < SEAT_W) ? sx + SEAT_W - w : sx;

  for (c = 0; c < pl->handCount; c++) {
    hd = &pl->hands[c];
    n = (unsigned char)(strlen(hd->cards) / 2);

    drawText(hx, sy - 1, handLabel(hd));
    if (pl->handCount > 1 && state.activePlayer == (int8_t)p && state.activeHand == c)
      drawUnderline(hx, sy - 1, (unsigned char)strlen(handLabel(hd)));

    for (d = 0; d < shown[c]; d++)
      drawCard(hx + d * step, sy, (step == 1 && d) ? FANNED_CARD : FULL_CARD, hd->cards + (n - shown[c] + d) * 2);

    hx += handWidth(shown[c]) + HAND_GAP;
  }
}

void redrawSeat(unsigned char p) {
  static unsigned char s;

  s = seatOrder[state.playerCount - 1][p];
  clearArea(seatX[s], seatY[s] - 4, SEAT_W, CARD_HEIGHT + 4);
  drawSeat(p);
}

void drawTable() {
  static unsigned char p;

  resetScreen();

  centerText(9, "blackjack");
  centerText(10, "pays 3 to 2");
  centerText(12, "stands on 17");

  // The shoe
  drawCard(WIDTH - 4, 1, FULL_CARD, "??");

  redrawDealer();
  for (p = 0; p < state.playerCount; p++)
    drawSeat(p);
}

/* Menu stops, in screen order. Bets show as bare amounts after a "bet"
   label; "same as last" is left out and the cursor starts on that amount
   instead. */
static unsigned char stopX[5], stopW[5], stopRef[5], stopCount;

static const char *moveLabel(ValidMove *v) {
  static unsigned char c;

  if (v->move[0] == 'b' || v->move[0] == 'r') {
    for (c = (unsigned char)strlen(v->name); c && v->name[c - 1] != ' '; c--)
      ;
    return v->name + c;
  }
#if WIDTH < 40
  switch (v->move[0]) {
    case 'h': return "hit";
    case 'd': return "double";
    case 's':
      switch (v->move[1]) {
        case 't': return "stay";
        case 'p': return "split";
      }
      return SURRENDER_LABEL;
  }
#endif
  return v->name;
}

unsigned char layoutMoveMenu() {
  static unsigned char total, gap, c, mx;

  clearStatusBar();
  mx = 0;
  if (state.round == ROUND_BETTING) {
    drawStatusTextAt(0, "bet");
    mx = 4;
  }

  stopCount = 0;
  total = 0;
  for (c = 0; c < state.validMoveCount; c++) {
    if (state.validMoves[c].move[0] == 'r')
      continue;
    stopRef[stopCount] = c;
    stopW[stopCount] = (unsigned char)strlen(moveLabel(&state.validMoves[c]));
    total += stopW[stopCount++];
  }
  gap = (mx + total + 2 * (stopCount - 1) <= MENU_END + 1) ? 2 : 1;

  for (c = 0; c < stopCount; c++) {
    stopX[c] = mx;
    drawStatusTextAt(mx, moveLabel(&state.validMoves[stopRef[c]]));
    mx += stopW[c] + gap;
  }

  drawStatusTimeLeft();
  return stopCount;
}

// Last hand's bet when it can be repeated, otherwise stand
unsigned char defaultStop() {
  static unsigned char c;

  if (!stopCount)
    return 0;
  if (state.round == ROUND_BETTING) {
    if (state.validMoves[0].move[0] != 'r')
      return 0;
    strcpy(tempBuffer, moveLabel(&state.validMoves[0]));
    for (c = 0; c < stopCount; c++)
      if (!strcmp(moveLabel(&state.validMoves[stopRef[c]]), tempBuffer))
        return c;
    return 0;
  }
  for (c = 0; c < stopCount; c++)
    if (state.validMoves[stopRef[c]].move[1] == 't')
      return c;
  return 0;
}

void drawMoveCursor(unsigned char stop, bool on) {
  if (on)
    drawLine(stopX[stop], HEIGHT - 1, stopW[stop]);
  else
    hideLine(stopX[stop], HEIGHT - 1, stopW[stop]);
}

char *stopMove(unsigned char stop) {
  return state.validMoves[stopRef[stop]].move;
}

// Leaves only the chosen move on the status bar
void showChosenMove(unsigned char stop) {
  clearStatusBar();
  drawStatusTextAt(stopX[stop], moveLabel(&state.validMoves[stopRef[stop]]));
}

void drawStatusTimeLeft() {
  drawStatusTimer();
  tempBuffer[0] = ' ';
  itoa(state.moveTime, tempBuffer + 1, 10);
  drawStatusTextAt((unsigned char)(WIDTH - strlen(tempBuffer) - STATUS_TIMER_WIDTH), tempBuffer);
}

void drawGameStatus() {
  if (state.activePlayer > 0 || (state.activePlayer == 0 && state.viewing)) {
    strcpy(tempBuffer, "waiting on ");
    strcat(tempBuffer, state.players[state.activePlayer].name);
    drawStatusText(tempBuffer);
  } else if (state.round == ROUND_DEALER) {
    drawStatusText("dealer plays");
  } else {
    drawStatusText(state.lastResult);
  }
}
