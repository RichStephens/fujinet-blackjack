#ifdef __APPLE2__

/**
 * @brief Text routines
 * @author Oliver Schmidt
 */
#include "hires.h"

#define CHAR_HEIGHT 8

extern unsigned char charset[];

void hires_putc(unsigned char x, unsigned char y, unsigned rop, unsigned char c)
{
    hires_Draw(x, y, 1, CHAR_HEIGHT, rop, &charset[c << 3]);
}

void hires_putcc(unsigned char x, unsigned char y, unsigned rop, unsigned cc)
{
    hires_putc(x, y, rop, cc >> 8);
    hires_putc(++x, y, rop, cc);
}

#endif /* __APPLE2__ */
