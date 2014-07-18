void trx_init();
void trx_reset();
void trx_close();

uint16_t rf12_xfer(uint16_t cmd);
void rf12_cmd(uint8_t cmd, uint8_t d);
