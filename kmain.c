#include "io.h"
#include "serial.h"

/* ------------------------------------------------------------------------- */
/* ----------------------  Framebuffer --------------------------------------*/
/* ------------------------------------------------------------------------- */

#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos)
{
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
        outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
        outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
        outb(FB_DATA_PORT,    pos & 0x00FF);
}

/**************************************************************************/
int sum_of_three(int arg1, int arg2, int arg3)
{
	return arg1 + arg2 + arg3;
}

/********************** Framebuffer ****************************************/

#define FB_GREEN     2
#define FB_DARK_GREY 8
#define FRAMEBUFFER_ADDRESS 0x000B8000

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	char *fb = (char *) FRAMEBUFFER_ADDRESS;
	fb[i] = c;
        fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void fb_write_simple()
{
	fb_write_cell(0, 'A', FB_GREEN, FB_DARK_GREY);
}

/***************************************************************************/

void os_main()
{
	fb_write_simple();
	test_serial_port();
}


