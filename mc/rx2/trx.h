#ifdef __cplusplus
extern "C" {
#endif

#define RFM_IRQ     2
#define SS_DDR      DDRB
#define SS_PORT     PORTB
#define SS_BIT      2     // for PORTB: 2 = d.10, 1 = d.9, 0 = d.8

#define SPI_SS      10    // PB2, pin 16
#define SPI_MOSI    11    // PB3, pin 17
#define SPI_MISO    12    // PB4, pin 18
#define SPI_SCK     13    // PB5, pin 19

#define RF12_SELECT   (PORTB &= ~_BV(PB2))
#define RF12_UNSELECT (PORTB |= _BV(PB2))
#define IRQ_HI (PIND & _BV(PD2))

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



