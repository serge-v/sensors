#ifdef __cplusplus
extern "C" {
#endif


#if defined(__AVR_ATmega328P__)
#	define IRQ_DDR DDRD
#	define IRQ_PORT    PIND
#	define IRQ_PIN     PD2 // 2
#	define SELECT_DDR  DDRB
#	define SELECT_PORT PORTB
#	define SELECT_PIN  PB2 // 10
#	define SPI_MOSI    PB3 // 11
#	define SPI_MISO    PB4 // 12
#	define SPI_SCK     PB5 // 13
#elif defined(__AVR_ATtiny85__)
#	define IRQ_DDR     DDRB
#	define IRQ_PORT    PINB
#	define IRQ_PIN     PB3  // pin 2
#	define SELECT_DDR  DDRB
#	define SELECT_PORT PORTB
#	define SELECT_PIN  PB4  // pin 3
#	define SPI_MOSI    PB0  // pin 5
#	define SPI_MISO    PB1  // pin 6
#	define SPI_SCK     PB2  // pin 7
#elif defined(__AVR_ATtiny84__)
#	define IRQ_DDR     DDRB
#	define IRQ_PORT    PINB
#	define IRQ_PIN     PB3  // pin 2
#	define SELECT_DDR  DDRB
#	define SELECT_PORT PORTB
#	define SELECT_PIN  PB4  // pin 3
#	define SPI_MOSI    PB0  // pin 5
#	define SPI_MISO    PB1  // pin 6
#	define SPI_SCK     PB2  // pin 7
#else
#error "MCU is not supported"
#endif

enum rf12_state
{
	IDLE,
	TX_IN_PROGRESS,
	TX_DONE,
	RX_ON,
	RX_IN_PROGRESS,
	RX_DONE_OK,
	RX_DONE_OVERFLOW,
	RX_DONE_BADCRC
};

void rf12_initialize(uint8_t id, uint8_t group);
void rf12_debug(uint8_t flag);
void rf12_rx_on(void);
void rf12_rx_off(void);
void rf12_send_sync(const char* s, uint8_t n);
uint8_t rf12_wait_rx(void);
uint8_t rf12_read_rx(void);

extern uint8_t                   rf12_node;   // remote node id
extern uint8_t*                  rf12_data;   // receiver buffer
extern volatile uint8_t          rf12_len;    // received length
extern volatile enum rf12_state  rf12_state;

#ifdef __cplusplus
}
#endif
