// 1. Configuration Setting Command
// ===============================================
#define RF_CONFIG       0x80
#define RF_CONFIG_EL    0x80 // enable TX register
#define RF_CONFIG_EF    0x40 // enable FIFO

//                            b0 b1
#define RF_FFREQ_433   0x10 // 0  1
#define RF_FFREQ_866   0x20 // 1  0
#define RF_FFREQ_915   0x30 // 1  1

//                              x3 x2 x1 x0 pF
#define RF_CAP_85pF     0x0 //  0  0  0  0  8.5
#define RF_CAP_90pF     0x1 //  0  0  0  1  9.0
#define RF_CAP_95pF     0x2 //  0  0  1  0  9.5
#define RF_CAP_100pF    0x3 // 0  0  1  1  10.0
#define RF_CAP_105pF    0x4 // 0  1  0  0  10.5
#define RF_CAP_110pF    0x5 // 0  1  0  1  11.0
#define RF_CAP_115pF    0x6 // 0  1  1  0  11.5
#define RF_CAP_120pF    0x7 // 0  1  1  1  12.0
#define RF_CAP_125pF    0x8 // 1  0  0  0  12.5
#define RF_CAP_130pF    0x9 // 1  0  0  1  13.0
#define RF_CAP_135pF    0xA // 1  0  1  0  13.5
#define RF_CAP_140pF    0xB // 1  0  1  1  14.0
#define RF_CAP_145pF    0xC // 1  1  0  0  14.5
#define RF_CAP_150pF    0xD // 1  1  0  1  15.0
#define RF_CAP_155pF    0xE // 1  1  1  0  15.5
#define RF_CAP_160pF    0xF // 1  1  1  1  16.0

// 2. Power Management Command
// ===============================================
#define RF_PWR_MGMT       0x82
#define RF_PWR_ER  0x80 //  Enables the whole receiver chain: RF front end, baseband, synthesizer, crystal oscillator
#define RF_PWR_EBB 0x40 // The receiver baseband circuit can be separately switched on
#define RF_PWR_ET  0x20 // Switches on the PLL, pamp, and starts the transmission (if RF_CONFIG_EL is set)
#define RF_PWR_ES  0x10 // Turns on the synthesizer
#define RF_PWR_EX  0x08 // Turns on the crystal oscillator
#define RF_PWR_EB  0x04 // Enables the low battery detector
#define RF_PWR_EW  0x02 // Enables the wake-up timer
#define RF_PWR_DC  0x01 // Disables the clock output (pin 8)

// 3. Frequency Setting Command
// ===============================================
#define RF_FREQ_CFG       0xA6

// 4. Data Rate Command
// ===============================================
#define RF_DRATE_CFG      0xC6

// 5. Receiver Control Command
// ===============================================
#define RF_RX_CTRL        0x90
#define RF_RX_VDI_OUT     0x04

// Bits 9-8 (d1 to d0): VDI (valid data indicator) signal response time setting
//                               d1 d0
#define RF_RX_RESP_FAST   0x0 // 0  0
#define RF_RX_RESP_MED    0x1 // 0  1
#define RF_RX_RESP_LOW    0x2 // 1  0
#define RF_RX_RESP_ON     0x3 // 1  1

// Bits 7-5 (i2 to i0): Receiver baseband bandwidth (BW) select
//                          i2 i1 i0 BW [kHz]
#define RF_RX_BW_400 0x2 // 0  0  1  400
#define RF_RX_BW_340 0x3 // 0  1  0  340
#define RF_RX_BW_270 0x6 // 0  1  1  270
#define RF_RX_BW_200 0x8 // 1  0  0  200
#define RF_RX_BW_134 0xA // 1  0  1  134
#define RF_RX_BW_67  0xC // 1  1  0   67

// Bits 4-3 (g1 to g0): LNA gain select:
//                              g1 g0  Gain relative to maximum [dB]
#define RF_RX_GAIN_0   0x00  // 0  0    0
#define RF_RX_GAIN_M6  0x08  // 0  1   -6
#define RF_RX_GAIN_M14 0x10  // 1  0  -14
#define RF_RX_GAIN_M20 0x18  // 1  1  -20

// Bits 2-0 (r2 to r0): RSSI detector threshold:
//                              r2 r1 r0 RSSI
#define RF_RX_RSSI_M103 0x00 // 0  0  0  -103
#define RF_RX_RSSI_M97  0x01 // 0  0  1  -97
#define RF_RX_RSSI_M91  0x02 // 0  1  0  -91
#define RF_RX_RSSI_M85  0x03 // 0  1  1  -85
#define RF_RX_RSSI_M79  0x04 // 1  0  0  -79
#define RF_RX_RSSI_M73  0x05 // 1  0  1  -73

// 6. Data Filter Command
// ===============================================
#define RF_DF 0xC2

// Bit 7 (al): Clock recovery (CR) auto lock control:
#define RF_DF_AL 0x80 // auto lock

// Bit 6 (ml): Clock recovery lock control:
#define RF_DF_ML 0x40 // fast mode: 4 to 8-bit preamble (1010...) is recommended

// Bit 5 and 3 are always set
#define RF_DF_SBITS 0x28

// Bit 4 (s): Select the type of the data filter:
#define RF_DF_S 0x00 // digital

// Bits 2-0 (f2 to f0): DQD threshold parameter.
#define RF_DF_DQD4 0x0C // digital

// 7. FIFO and Reset Mode Command
// ===============================================
#define RF_FIFO 0xCA

// 8. Synchron Pattern Command
// ===============================================
#define RF_PATTERN 0xCE

// 10. AFC Command
// ===============================================
#define RF_AFC 0xC4

// 11. TX Configuration Control Command
// ===============================================
#define RF_TX_CFG 0x98

// 12. PLL Setting Command
// ===============================================
#define RF_PLL_CFG 0xCC

// 14. Wake-Up Timer Command
// ===============================================
#define RF_WAKEUP_CFG 0xE0

// 15. Low Duty-Cycle Command
// ===============================================
#define RF_DUTY_CFG 0xC8

// 16. Low Battery Detector and Microcontroller Clock Divider Command
// ===============================================
#define RF_BATT_CFG 0xC0
