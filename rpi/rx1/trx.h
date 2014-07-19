void trx_init();
void trx_reset();
void trx_close();
uint8_t trx_recv();

uint16_t rf12_xfer(uint16_t cmd);
void rf12_cmd(uint8_t cmd, uint8_t d);
void calc_crc(uint8_t group, uint8_t id, uint8_t len, uint8_t* b);

