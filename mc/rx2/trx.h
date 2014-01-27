#ifdef __cplusplus
extern "C" {
#endif
#if defined(__AVR_ATmega328P__)
#	define IRQ_DDR DDRD
#	define IRQ_PORT PIND
#	define IRQ_PIN PD2
#	define SELECT_DDR PORTB
#	define SELECT_PORT PORTB
#	define SELECT_PIN PB2
#else
#	define IRQ_DDR DDRB
#	define IRQ_PORT PINB
#	define IRQ_PIN PB3
#	define SELECT_DDR PORTB
#	define SELECT_PORT PORTB
#	define SELECT_PIN PB4
#endif

#define SPI_SS      10    // PB2, pin 16
#define SPI_MOSI    11    // PB3, pin 17
#define SPI_MISO    12    // PB4, pin 18
#define SPI_SCK     13    // PB5, pin 19

void dot(void);

uint8_t rf12_wait_nirq(void);
uint16_t rf12_read_status(void);
uint8_t rf12_rx(void);
uint8_t rf12_rx_slow(void);
uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte);
void rf12_spi_init(void);
void rf12_reset_fifo(void);
void rf12_setup(void);

#ifdef __cplusplus
}
#endif



