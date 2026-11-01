#ifdef __ADAM__

/**
 * @brief   Adam Graphics Routines for blackjack, from 5cardstud's
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details
 * @verbose TMS9918 GRAPHICS II via the z88dk generic console; card and
 *          frame art from 5cardstud (see udg.h), text set in the Namco
 *          arcade font via the CRT_FONT redirect.
 */

#include <stdbool.h>
#include <string.h>
#include <video/tms99x8.h>
#include <conio.h>
#include <sys/ioctl.h>
#include <eos.h>
#include "udg.h"
#include "../misc.h"
#include "../platform-specific/graphics.h"

#define CORNER_TOP 0
#define CORNER_BOTTOM 18
#define STATUS_Y (HEIGHT - 1)

// Table palette, from 5cardstud
#define C_TABLE   VDP_INK_DARK_GREEN   // the felt
#define C_ACCENT  VDP_INK_MEDIUM_RED   // card outlines/backs, frames
#define C_CARD_BG VDP_INK_WHITE        // card face
#define C_TEXT    VDP_INK_WHITE        // table text
#define C_BORDER  VDP_INK_BLACK        // VDP border / status bar paper

#define FELT ((C_TEXT << 4) | C_TABLE)
#define STATUS ((VDP_INK_WHITE << 4) | VDP_INK_BLACK)

#define PATTERN(x, y) ((uint16_t)(y) * 256 + (x) * 8)

// VDP interrupt hook living in util.c (bumps the timer + vsync flag)
extern void myInt(void);

unsigned char colorMode;
static bool tallStatus;

void initGraphics()
{
    void *param = &udg;
    vdp_set_mode(2);
    console_ioctl(IOCTL_GENCON_SET_UDGS,&param);
    vdp_color(C_TEXT,C_TABLE,C_BORDER);
    add_raster_int(myInt);
    clrscr();
}

void resetGraphics()
{
}

uint8_t cycleNextColor()
{
    return 0;
}

void setColorMode(unsigned char mode)
{
    (void)mode;
}

void drawBuffer()
{
}

// Upper case on the fly: the strings are the game's state. The bottom right
// cell is left alone, as printing there scrolls the console.
static void putText(unsigned char x, unsigned char y, const char *s)
{
    static char c;

    gotoxy(x,y);
    while ((c = *s++) && x < WIDTH)
    {
        if (c >= 'a' && c <= 'z')
            c -= 32;
        if (x == WIDTH-1 && y == HEIGHT-1)
            break;
        cputc(c);
        x++;
    }
}

void drawText(unsigned char x, unsigned char y, const char* s)
{
    vdp_color(C_TEXT,C_TABLE,C_BORDER);
    putText(x,y,s);
}

void drawTextAt(unsigned char x, unsigned char y, const char* s)
{
    drawText(x,y,s);
}

void drawChip(unsigned char x, unsigned char y)
{
    vdp_color(C_ACCENT,C_TABLE,C_BORDER);
    gotoxy(x,y);
    cputc(UDG_CHIP);
}

void drawBox(unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
    unsigned char i=0;
    // Correct coordinates
    w++;
    h++;

    vdp_color(C_ACCENT,C_TABLE,C_BORDER);

    // Put box corners at coordinate extents
    gotoxy(x,y);
    cputc(UDG_BOX_TL);
    gotoxy(x+w,y);
    cputc(UDG_BOX_TR);
    gotoxy(x,y+h);
    cputc(UDG_BOX_BL);
    gotoxy(x+w,y+h);
    cputc(UDG_BOX_BR);

    x++;
    w--;

    // Put horizontal rules
    gotoxy(x,y);
    for (i=0;i<w;i++)
        cputc(UDG_BOX_H);

    gotoxy(x,y+h);
    for (i=0;i<w;i++)
        cputc(UDG_BOX_H);

    // Correct again
    x--;
    y++;
    h--;
    w++;

    // Put vertical rules
    for (i=0;i<h;i++)
    {
        gotoxy(x,y+i);
        cputc(UDG_BOX_V);
    }

    for (i=0;i<h;i++)
    {
        gotoxy(x+w,y+i);
        cputc(UDG_BOX_V);
    }
}

void drawLogo()
{
    static unsigned char i;
    i=4;
    drawText(WIDTH/2-5,++i, "           ");
    drawText(WIDTH/2-5,++i, " FUJI  NET ");
    drawText(WIDTH/2-5,++i, "           ");
    drawText(WIDTH/2-5,++i, " BLACKJACK ");
    drawText(WIDTH/2-5,++i, "           ");
}

void clearArea(unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
    while (h--)
    {
        vdp_vfill(PATTERN(x,y),0x00,w*8);
        vdp_vfill(MODE2_ATTR+PATTERN(x,y),FELT,w*8);
        y++;
    }
}

// Redrawing the bar is slow enough to flicker, so an unchanged message stays
static char shownStatus[41];

void clearStatusBar()
{
    shownStatus[0] = 0;
    vdp_vfill(PATTERN(0,STATUS_Y),0x00,0x100);

    // A two-line message borrowed the row above
    if (tallStatus)
    {
        clearArea(0,STATUS_Y-1,WIDTH,1);
        tallStatus = false;
    }
}

/**
 * @brief Clear the screen
 */
void resetScreen()
{
    vdp_color(C_TEXT,C_TABLE,C_BORDER);
    clrscr();
    tallStatus = false;
    shownStatus[0] = 0;

    // Persistent status bar: row 23 renders white on black.
    vdp_vfill(MODE2_ATTR+PATTERN(0,STATUS_Y),STATUS,0x100);

    // Rounded felt corners; card draws may overlay these
    vdp_color(C_TABLE,VDP_INK_BLACK,C_BORDER);
    gotoxy(0,0);
    cputc(UDG_SCREEN_TL);
    gotoxy(WIDTH-1,0);
    cputc(UDG_SCREEN_TR);
    gotoxy(0,STATUS_Y-1);
    cputc(UDG_SCREEN_BL);
    gotoxy(WIDTH-1,STATUS_Y-1);
    cputc(UDG_SCREEN_BR);
}

void drawStatusTextAt(unsigned char x, const char* s)
{
    shownStatus[0] = 0;
    vdp_color(VDP_INK_WHITE,VDP_INK_BLACK,C_BORDER);
    putText(x,STATUS_Y,s);
}

// A message too long for the bar breaks at its comma onto the row above
void drawStatusText(const char* s)
{
    static unsigned char i;

    if (!strcmp(s, shownStatus))
        return;
    clearStatusBar();
    if (strlen(s) <= WIDTH)
    {
        drawStatusTextAt(0, s);
        strcpy(shownStatus, s);
        return;
    }

    for (i=0; s[i] && s[i]!=',' && i<WIDTH; i++)
        tempBuffer[i]=s[i];
    tempBuffer[i]=0;

    tallStatus = true;
    vdp_vfill(PATTERN(0,STATUS_Y-1),0x00,0x100);
    vdp_vfill(MODE2_ATTR+PATTERN(0,STATUS_Y-1),STATUS,0x100);
    vdp_color(VDP_INK_WHITE,VDP_INK_BLACK,C_BORDER);
    putText(0,STATUS_Y-1,tempBuffer);

    if (s[i]==',')
        i++;
    while (s[i]==' ')
        i++;
    drawStatusTextAt(0, s+i);
    strcpy(shownStatus, s);
}

void drawStatusTimer()
{
}

static void cardRow(unsigned char x, unsigned char y, unsigned char style, unsigned char left, unsigned char mid, unsigned char midInk)
{
    static uint16_t a;
    static uint8_t r;

    gotoxy(x,y);
    if (style != FANNED_CARD)
    {
        vdp_color(C_ACCENT,C_CARD_BG,C_BORDER);
        cputc(left);
    }
    else
        gotoxy(x+1,y);
    vdp_color(midInk,C_CARD_BG,C_BORDER);
    cputc(mid);

    // 5cardstud never fans, so it has no edge for one; draw it in the ink
    if (style == FANNED_CARD)
    {
        a = PATTERN(x+1,y);
        for (r=0;r<8;r++,a++)
            vdp_vpoke(a,vdp_vpeek(a)|0x80);
        gotoxy(x+2,y);
    }

    // Right border (if no existing char)
    if (x+2 < WIDTH && cvpeek(x+2,y)==' ')
    {
        vdp_color(C_ACCENT,C_TABLE,C_BORDER);
        cputc(UDG_CARD_VERT);
    }
}

static void cardEdge(unsigned char x, unsigned char y, unsigned char style, unsigned char corner, unsigned char edge, unsigned char stub)
{
    vdp_color(C_ACCENT,C_TABLE,C_BORDER);
    gotoxy(x,y);
    if (style != FANNED_CARD)
        cputc(corner);
    else
        gotoxy(x+1,y);
    cputc(edge);
    if (x+2 < WIDTH && cvpeek(x+2,y)==' ')
        cputc(stub);
}

/**
 * @brief Draw card at position x,y
 * @param x Horizontal card position (0-31)
 * @param y Vertical card position (0-23)
 * @param style FULL_CARD, or FANNED_CARD to cover only the right column of
 *              the card beneath; its left edge would cover that card's rank
 * @param s String indicating number and suit. (e.g. "as" for ace of spades)
 */
void drawCard(unsigned char x, unsigned char y, unsigned char style, const char* s)
{
    static unsigned char val, suitColor, suit;

    if (s[0]=='?')
    {
        cardEdge(x,y,style,UDG_CARD_TL,UDG_CARD_TOP,UDG_CARD_TR_STUB);
        cardRow(x,y+1,style,UDG_BACK_L_TOP,UDG_BACK_R_TOP,C_ACCENT);
        cardRow(x,y+2,style,UDG_BACK_L_MID,UDG_BACK_R_MID,C_ACCENT);
        cardRow(x,y+3,style,UDG_BACK_L_BOT,UDG_BACK_R_BOT,C_ACCENT);
        cardEdge(x,y+4,style,UDG_CARD_BL,UDG_CARD_BOT,UDG_CARD_BR_STUB);
        return;
    }

    switch (s[1])
    {
    case 'h' :
        suit=UDG_SUIT_HEART;
        suitColor=C_ACCENT;
        break;
    case 'd' :
        suit=UDG_SUIT_DIAMOND;
        suitColor=C_ACCENT;
        break;
    case 'c' :
        suit=UDG_SUIT_CLUB;
        suitColor=VDP_INK_BLACK;
        break;
    default:
        suit=UDG_SUIT_SPADE;
        suitColor=VDP_INK_BLACK;
        break;
    }

    switch (s[0])
    {
    case 't':
        val=UDG_RANK_T; // 10
        break;
    case 'j':
        val=UDG_RANK_J;
        break;
    case 'q':
        val=UDG_RANK_Q;
        break;
    case 'k':
        val=UDG_RANK_K;
        break;
    case 'a':
        val=UDG_RANK_A;
        break;
    default:
        val=UDG_RANK_2+(s[0]-'2');
        break;
    }

    cardEdge(x,y,style,UDG_CARD_TL,UDG_CARD_TOP,UDG_CARD_TR_STUB);
    cardRow(x,y+1,style,UDG_CARD_VERT,val,suitColor);
    cardRow(x,y+2,style,UDG_CARD_VERT,' ',suitColor);
    cardRow(x,y+3,style,UDG_CARD_VERT,suit,suitColor);
    cardEdge(x,y+4,style,UDG_CARD_BL,UDG_CARD_BOT,UDG_CARD_BR_STUB);
}

void hideLine(unsigned char x, unsigned char y, unsigned char w)
{
    uint8_t i;
    if (y==STATUS_Y)
    {
        vdp_vfill(MODE2_ATTR+PATTERN(x,y),STATUS,8*w);
    }
    else
    {
        for(i=0;i<w*8;i+=8)
        {
            vdp_vfill(MODE2_ATTR+PATTERN(x,y)+i,FELT,2);
        }
    }

}

void drawLine(unsigned char x, unsigned char y, unsigned char w)
{
    uint8_t i;
    if (y==STATUS_Y)
    {
        for(i=0;i<w*8;i+=8)
        {
            vdp_vpoke(MODE2_ATTR+PATTERN(x,y)+7+i,(VDP_INK_WHITE<<4)|C_ACCENT);
        }
    }
    else
    {
        for(i=0;i<w*8;i+=8)
        {
            vdp_vfill(MODE2_ATTR+PATTERN(x,y)+i,(VDP_INK_WHITE<<4)|C_ACCENT,2);
        }
    }
}

// The bottom pixel row of each cell, in the text color
void drawUnderline(unsigned char x, unsigned char y, unsigned char w)
{
    uint8_t i;
    for(i=0;i<w*8;i+=8)
        vdp_vpoke(MODE2_ATTR+PATTERN(x,y)+7+i,(C_TEXT<<4)|C_TEXT);
}

void drawBorder()
{
    drawCard(1,CORNER_TOP,FULL_CARD, "as");
    drawCard(WIDTH-3,CORNER_TOP,FULL_CARD, "ah");
    drawCard(1,CORNER_BOTTOM,FULL_CARD, "ad");
    drawCard(WIDTH-3,CORNER_BOTTOM,FULL_CARD, "ac");
}

#endif /* __ADAM__ */
