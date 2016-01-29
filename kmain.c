#include "io.h"
#include "serial.h"
#include "fb.h"

/**************************************************************************/

int sum_of_three(int arg1, int arg2, int arg3)
{
	return arg1 + arg2 + arg3;
}

/***************************************************************************/

void os_main()
{
	fb_write_simple();
	test_serial_port();
}


