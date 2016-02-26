/* ------------------------------------------------------------------------- */
/* ---------------------- Serial ports --------------------------------------*/
/* ------------------------------------------------------------------------- */

/* The I/O ports */

/* All the I/O ports are calculated relative to the data port. This is because
 * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
 * order, but they start at different values.
 */

#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* The I/O port commands */

/* SERIAL_LINE_ENABLE_DLAB:
 * Tells the serial port to expect first the highest 8 bits on the data port,
 * then the lowest 8 bits will follow
 */
#define SERIAL_LINE_ENABLE_DLAB         0x80

void serial_configure_baud_rate(unsigned short com, unsigned short divisor);

void serial_configure_line(unsigned short com);

int serial_is_transmit_fifo_empty(unsigned int com);

void serial_write_char(unsigned int com, char a);

void serial_write(const char* data, int len);

void serial_printf(char *fmt, int args);
 
/* ------------------------------------------------------------------------- */

void test_serial_port();

