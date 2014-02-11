//#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__AVR_ATmega328P__)
#	define IRQ_DDR DDRD
#	define IRQ_PORT    PIND
#	define IRQ_PIN     PD2
#	define SELECT_DDR  DDRB
#	define SELECT_PORT PORTB
#	define LED_PIN     PB1 // 9
#	define SELECT_PIN  PB2 // 10
#	define SPI_MOSI    PB3 // 11
#	define SPI_MISO    PB4 // 12
#	define SPI_SCK     PB5 // 13
#else
#	define IRQ_DDR     DDRB
#	define IRQ_PORT    PINB
#	define IRQ_PIN     PB3
#	define SELECT_DDR  DDRB
#	define SELECT_PORT PORTB
#	define SELECT_PIN  PB4
#	define LED_PIN     0
#	define SPI_SS      4    // PB4, pin 3
#	define SPI_MOSI    0    // PB0, pin 5
#	define SPI_MISO    1    // PB1, pin 6
#	define SPI_SCK     2    // PB2, pin 7
#endif

void rf12_spi_init(void);
void rf12_initialize(uint8_t id, uint8_t g);
void rf12_send(uint8_t len);
uint8_t rf12_cmd(uint8_t highbyte, uint8_t lowbyte);
void rf12_rx_on(void);
void rf12_rx_off(void);
void print_buf(void);
uint16_t rf12_read_status(void);
void rf12_debug(uint8_t flag);
void rf12_use_interrupts(uint8_t flag);
void rf12_reset_fifo(void);
uint8_t rf12_read_status_MSB(void);

extern uint8_t* rf12_data;
extern volatile uint8_t receiving;
extern volatile uint8_t rcv_done;
extern volatile uint8_t rf12_len;

#ifdef __cplusplus
}
#endif
