/**
 * @brief Hires graphics functions
 * @author Oliver Schmidt
 */

#ifndef HIRES_H
#define HIRES_H

#define ROP_CONST(val)          0xA980|(val)
#define ROP_BLACK               0xA980
#define ROP_WHITE               0xA9FF
#define ROP_CPY                 0x4980
#define ROP_INV                 0x49FF
#define ROP_A                   0x2900
#define ROP_O                   0x0900
#define ROP_AND(val)            ROP_A|(val)
#define ROP_OR(val)             ROP_O|(val)

void hires_Init(void);
void hires_Done(void);
void hires_Text(char flag);
void hires_Draw(char xpos,  char ypos,
                char xsize,   char ysize,
                unsigned rop, unsigned char *src);
void hires_Mask(char xpos,    char ypos,
                char xsize,   char ysize,
                unsigned rop);

#endif /* HIRES_H */
