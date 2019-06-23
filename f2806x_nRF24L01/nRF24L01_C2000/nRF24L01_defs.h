#ifndef NRF24L01_C2000_NRF24L01_DEFS_H_
#define NRF24L01_C2000_NRF24L01_DEFS_H_
#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------
// 	info and license
// ----------------------------------------------------------------------
//
//	filename:	nRF24L01_defs.h
//
//	MIT License
//
// 	Copyright (c) 2019 Benjamin Prescher
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
//
//	target:		Texas Instruments C2000
//
// ----------------------------------------------------------------------
// 	history
// ----------------------------------------------------------------------
// 	23.06.2019 - initial programming

// ----------------------------------------------------------------------
// 	header files
// ----------------------------------------------------------------------
#include "stdint.h"

// ----------------------------------------------------------------------
// 	#defines
// ----------------------------------------------------------------------
//  register map
#define nRF24_CONFIG                0x00
#define nRF24_EN_AA                 0x01
#define nRF24_EN_RXADDR             0x02
#define nRF24_SETUP_AW              0x03
#define nRF24_SETUP_RETR            0x04
#define nRF24_RF_CH                 0x05
#define nRF24_RF_SETUP              0x06
#define nRF24_STATUS                0x07
#define nRF24_OBSERVE_TX            0x08
#define nRF24_RPD                   0x09
#define nRF24_RX_ADDR_P0            0x0A
#define nRF24_RX_ADDR_P1            0x0B
#define nRF24_RX_ADDR_P2            0x0C
#define nRF24_RX_ADDR_P3            0x0D
#define nRF24_RX_ADDR_P4            0x0E
#define nRF24_RX_ADDR_P5            0x0F
#define nRF24_TX_ADDR               0x10
#define nRF24_RX_PW_P0              0x11
#define nRF24_RX_PW_P1              0x12
#define nRF24_RX_PW_P2              0x13
#define nRF24_RX_PW_P3              0x14
#define nRF24_RX_PW_P4              0x15
#define nRF24_RX_PW_P5              0x16
#define nRF24_FIFO_STATUS           0x17
#define nRF24_DYNPD                 0x1C
#define nRF24_FEATURE               0x1D

//  instructions
#define nRF24_R_REGISTER            0x00
#define nRF24_W_REGISTER            0x20
#define nRF24_R_RX_PAYLOAD          0x61
#define nRF24_W_TX_PAYLOAD          0xA0
#define nRF24_FLUSH_TX              0xE1
#define nRF24_FLUSH_RX              0xE2
#define nRF24_REUSE_TX_PL           0xE3
#define nRF24_R_RX_PL_WID           0x60
#define nRF24_W_ACK_PAYLOAD         0xA8
#define nRF24_W_TX_PAYLOAD_NOACK    0xB0
#define nRF24_NOP                   0xFF

//  helper
#define nRF24_PIPE_0                0
#define nRF24_PIPE_1                1
#define nRF24_PIPE_2                2
#define nRF24_PIPE_3                3
#define nRF24_PIPE_4                4
#define nRF24_PIPE_5                5
//
#define nRF24_PAYLOAD_WIDTH         32
#define nRF24_ADDRESS_WIDTH         5
//
#define nRF24_CE_PULSE_DURATION     10      // [us]
#define nRF24_VDD_UP_DURATION       100000  // [us]
#define nRF24_PWR_UP_DURATION       4500    // [us]
#define nRF24_CE_FOR_RX_DURATION    130     // [us]

// ----------------------------------------------------------------------
// 	global variables
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// nRF24L01 Config Register
// ----------------------------------------------------------------------
struct nRF24L01_CONFIG_REG_BITS
{                                   // bit      description
    uint16_t PRIM_RX:1;             // 0        1 = PRX // 0 = PTX
    uint16_t PWR_UP:1;              // 1        1 = pwr up // 0 = pwr dwn
    uint16_t CRC0:1;                // 2        CRC encode scheme ("0"-1byte;"1"-2byte)
    uint16_t EN_CRC:1;              // 3        enable CRC
    uint16_t RF24_MASK_MAX_RT:1;    // 4        0 = MAX_RT ISR reflected on IRQ pin
    uint16_t RF24_MASK_TX_DS:1;     // 5        0 = TX_DS reflected on IRQ pin
    uint16_t RF24_MASK_RX_DR:1;     // 6        0 = RX_DR reflected on IRQ pin
    uint16_t Rsvd:1;                // 7        reserved
};

typedef union
{
    uint16_t                        all;
    struct nRF24L01_CONFIG_REG_BITS bit;
} nRF24L01_CONFIG_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 Auto Acknowledgment Register
// ----------------------------------------------------------------------
struct nRF24L01_EN_AA_REG_BITS
{                                   // bit      description
    uint16_t ENAA_P0:1;             // 0        enable AA data pipe 0
    uint16_t ENAA_P1:1;             // 1        enable AA data pipe 1
    uint16_t ENAA_P2:1;             // 2        enable AA data pipe 2
    uint16_t ENAA_P3:1;             // 3        enable AA data pipe 3
    uint16_t ENAA_P4:1;             // 4        enable AA data pipe 4
    uint16_t ENAA_P5:1;             // 5        enable AA data pipe 5
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                       all;
    struct nRF24L01_EN_AA_REG_BITS bit;
} nRF24L01_EN_AA_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 enabled RX Addresses
// ----------------------------------------------------------------------
struct nRF24L01_EN_RXADDR_REG_BITS
{                                   // bit      description
    uint16_t ERX_P0:1;              // 0        enable data pipe 0
    uint16_t ERX_P1:1;              // 1        enable data pipe 1
    uint16_t ERX_P2:1;              // 2        enable data pipe 2
    uint16_t ERX_P3:1;              // 3        enable data pipe 3
    uint16_t ERX_P4:1;              // 4        enable data pipe 4
    uint16_t ERX_P5:1;              // 5        enable data pipe 5
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                           all;
    struct nRF24L01_EN_RXADDR_REG_BITS bit;
} nRF24L01_EN_RXADDR_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 setup of address width
// ----------------------------------------------------------------------
enum nRF24L01_fieldWitdh
{
    nRF_3Bytes = 1,
    nRF_4Bytes = 2,
    nRF_5Bytes = 3
};

struct nRF24L01_SETUP_AW_REG_BITS
{                                   // bit      description
    enum nRF24L01_fieldWitdh AW:2;  // 1:0      RX/TX address field width
    uint16_t Rsvd:6;                // 7:2      reserved
};

typedef union
{
    uint16_t                          all;
    struct nRF24L01_SETUP_AW_REG_BITS bit;
} nRF24L01_SETUP_AW_REG_t;

// ----------------------------------------------------------------------
//  nRF24L01 setup of automatic retransmission
//
//  For 250kbps data rate and 5byte address the following values apply:
//  ARD ACK         |       packet size (in bytes)
//------------------|----------------------------------------------------
//  1500us          |       All ACK payload sizes
//  1250us          |       < 24
//  1000us          |       < 16
//  750us           |       < 8
//  500us           |       Empty ACK with no payload
//
// ----------------------------------------------------------------------
enum nRF24L01_timeRetrans
{
    nRF_Wait_Retrans_250uS = 0,
    nRF_Wait_Retrans_500uS = 1,
    nRF_Wait_Retrans_750uS = 2,
    nRF_Wait_Retrans_1000uS = 3,
    nRF_Wait_Retrans_1250uS = 4,
    nRF_Wait_Retrans_1500uS = 5,
    nRF_Wait_Retrans_1750uS = 6,
    nRF_Wait_Retrans_2000uS = 7,
    nRF_Wait_Retrans_2250uS = 8,
    nRF_Wait_Retrans_2500uS = 9,
    nRF_Wait_Retrans_2750uS = 10,
    nRF_Wait_Retrans_3000uS = 11,
    nRF_Wait_Retrans_3250uS = 12,
    nRF_Wait_Retrans_3500uS = 13,
    nRF_Wait_Retrans_3750uS = 14,
    nRF_Wait_Retrans_4000uS = 15
};

struct nRF24L01_SETUP_RETR_REG_BITS
{                                   // bit      description
    uint16_t ARC:4;                 // 3:0      auto retransmit count
    enum nRF24L01_timeRetrans ARD:4;// 7:4      auto retransmit delay
};

typedef union
{
    uint16_t                            all;
    struct nRF24L01_SETUP_RETR_REG_BITS bit;
} nRF24L01_SETUP_RETR_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RF channel
// ----------------------------------------------------------------------
struct nRF24L01_RF_CH_REG_BITS
{                                   // bit      description
    uint16_t RF_CH:7;               // 6:0      frequency channel
    uint16_t Rsvd:1;                // 7        reserved
};

typedef union
{
    uint16_t                       all;
    struct nRF24L01_RF_CH_REG_BITS bit;
} nRF24L01_RF_CH_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RF setup register
// ----------------------------------------------------------------------
enum nRF24L01_rfPower
{
    nRF_minus18dBm = 0,
    nRF_minus12dBm = 1,
    nRF_minus6dBm = 2,
    nRF_minus0dBm = 3
};

enum nRF24L01_dataRate
{
    nRF_250kbit = 0,
    nRF_1Mbps = 1,
    nRF_2Mbps = 2
};

struct nRF24L01_RF_SETUP_REG_BITS
{                                   // bit      description
    uint16_t Obsolete:1;            // 0        don´t care
    enum nRF24L01_rfPower RF_PWR:2; // 2:1      sets RF pwr in TX mode
    uint16_t RF_DR_HIGH:1;          // 3        high speed modes of RF_DR_LOW not set
    uint16_t PLL_LOCK:1;            // 4        force pll lock signal
    uint16_t RF_DR_LOW:1;           // 5        sets RF data rate to 250kbps
    uint16_t Rsvd:1;                // 6        reserved
    uint16_t CONT_WAVE:1;           // 7        enables continous carrier transm.
};

typedef union
{
    uint16_t                          all;
    struct nRF24L01_RF_SETUP_REG_BITS bit;
} nRF24L01_RF_SETUP_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RF status register
// ----------------------------------------------------------------------
struct nRF24L01_STATUS_REG_BITS
{                                   // bit      description
    uint16_t TX_FULL:1;             // 0        TX FIFO full flag
    uint16_t RX_P_NO:3;             // 3:1      data pipe number for available payload on pipe num.
    uint16_t MAX_RT:1;              // 4        max nr. of retransmit interrupt -> write 1 to clr
    uint16_t TX_DS:1;               // 5        data sent TX FIFO interrupt -> write 1 to clr
    uint16_t RX_DR:1;               // 6        data rdy RX FIFO interrupt -> write 1 to clr
    uint16_t Rsvd:1;                // 7        reserved
};

typedef union
{
    uint16_t                        all;
    struct nRF24L01_STATUS_REG_BITS bit;
} nRF24L01_STATUS_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 TX observe register
// It is possible to use the information in the OBSERVE_TX register to
// make an overall assessment of the channel quality.
// ----------------------------------------------------------------------
struct nRF24L01_OBSERVE_TX_REG_BITS
{                                   // bit      description
    uint16_t ARC_CNT:4;             // 3:0      count retrans packages
    uint16_t PLOS_CNT:4;            // 7:4      total number of retransmissions
                                    //          since the last channel change
};

typedef union
{
    uint16_t                            all;
    struct nRF24L01_OBSERVE_TX_REG_BITS bit;
} nRF24L01_OBSERVE_TX_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RPD register
// ----------------------------------------------------------------------
struct nRF24L01_RPD_REG_BITS
{                                   // bit      description
    uint16_t RPD:1;                 // 0        received pwr detector (carrier detect)
    uint16_t Rsvd:7;                // 7:1      reserved
};

typedef union
{
    uint16_t                     all;
    struct nRF24L01_RPD_REG_BITS bit;
} nRF24L01_RPD_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RX_PW_P0 register
// ----------------------------------------------------------------------
struct nRF24L01_RX_PW_P0_REG_BITS
{                                   // bit      description
    uint16_t RX_PW_P0:6;            // 5:0      nr. of bytes in RX payload pipe0
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                          all;
    struct nRF24L01_RX_PW_P0_REG_BITS bit;
} nRF24L01_RX_PW_P0_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RX_PW_P1 register
// ----------------------------------------------------------------------
struct nRF24L01_RX_PW_P1_REG_BITS
{                                   // bit      description
    uint16_t RX_PW_P1:6;            // 5:0      nr. of bytes in RX payload pipe1
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                          all;
    struct nRF24L01_RX_PW_P1_REG_BITS bit;
} nRF24L01_RX_PW_P1_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RX_PW_P2 register
// ----------------------------------------------------------------------
struct nRF24L01_RX_PW_P2_REG_BITS
{                                   // bit      description
    uint16_t RX_PW_P2:6;            // 5:0      nr. of bytes in RX payload pipe2
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                          all;
    struct nRF24L01_RX_PW_P2_REG_BITS bit;
} nRF24L01_RX_PW_P2_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RX_PW_P3 register
// ----------------------------------------------------------------------
struct nRF24L01_RX_PW_P3_REG_BITS
{                                   // bit      description
    uint16_t RX_PW_P3:6;            // 5:0      nr. of bytes in RX payload pipe3
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                          all;
    struct nRF24L01_RX_PW_P3_REG_BITS bit;
} nRF24L01_RX_PW_P3_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RX_PW_P4 register
// ----------------------------------------------------------------------
struct nRF24L01_RX_PW_P4_REG_BITS
{                                   // bit      description
    uint16_t RX_PW_P4:6;            // 5:0      nr. of bytes in RX payload pipe4
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                          all;
    struct nRF24L01_RX_PW_P4_REG_BITS bit;
} nRF24L01_RX_PW_P4_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 RX_PW_P5 register
// ----------------------------------------------------------------------
struct nRF24L01_RX_PW_P5_REG_BITS
{                                   // bit      description
    uint16_t RX_PW_P5:6;            // 5:0      nr. of bytes in RX payload pipe5
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                          all;
    struct nRF24L01_RX_PW_P5_REG_BITS bit;
} nRF24L01_RX_PW_P5_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 FIFO status register
// ----------------------------------------------------------------------
struct nRF24L01_FIFO_STATUS_REG_BITS
{                                   // bit      description
    uint16_t RX_EMPTY:1;            // 0        1 = empty
    uint16_t RX_FULL:1;             // 1        1 = full
    uint16_t Rsvd1:2;               // 3:2      reserved
    uint16_t TX_EMPTY:1;            // 4        1 = empty
    uint16_t TX_FULL:1;             // 5        1 = full
    uint16_t TX_REUSE:1;            // 6        used for a PTX device
    uint16_t Rsvd0:1;               // 7        reserved
};

typedef union
{
    uint16_t                             all;
    struct nRF24L01_FIFO_STATUS_REG_BITS bit;
} nRF24L01_FIFO_STATUS_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 dynamic payload length register
// ----------------------------------------------------------------------
struct nRF24L01_DYNPD_REG_BITS
{                                   // bit      description
    uint16_t DPL_P0:1;              // 0        enable dpl pipe 0
    uint16_t DPL_P1:1;              // 1        enable dpl pipe 1
    uint16_t DPL_P2:1;              // 2        enable dpl pipe 2
    uint16_t DPL_P3:1;              // 3        enable dpl pipe 3
    uint16_t DPL_P4:1;              // 4        enable dpl pipe 4
    uint16_t DPL_P5:1;              // 5        enable dpl pipe 5
    uint16_t Rsvd:2;                // 7:6      reserved
};

typedef union
{
    uint16_t                       all;
    struct nRF24L01_DYNPD_REG_BITS bit;
} nRF24L01_DYNPD_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 feature register
// ----------------------------------------------------------------------
struct nRF24L01_FEATURE_REG_BITS
{                                   // bit      description
    uint16_t EN_DYN_ACK:1;          // 0        enables the W_TX_PAYLOAD_NOACK command
    uint16_t EN_ACK_PAY:1;          // 1        enables payload with ACK
    uint16_t EN_DPL:1;              // 2        enable dynamic payload length
    uint16_t Rsvd0:5;               // 7:3      reserved
};

typedef union
{
    uint16_t                         all;
    struct nRF24L01_FEATURE_REG_BITS bit;
} nRF24L01_FEATURE_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 failure register
// ----------------------------------------------------------------------
struct nRF24L01_FAILURE_REG_BITS
{                                   // bit      description
    uint16_t SPI_Timeout:1;         // 0        Spi timeout failure
    uint16_t ADDR_Range:1;          // 1        address width failure
    uint16_t PAYLOAD_Range:1;       // 2        payload width failure
    uint16_t PIPE_Range:1;          // 3        pipe is out of range
    uint16_t DATA_Range:1;          // 4        data Rata out of range
    uint16_t PWR_Change:1;          // 5        error in power mode change
    uint16_t Rsvd0:2;               // 7:5      reserved
};

typedef union
{
    uint16_t                         all;
    struct nRF24L01_FAILURE_REG_BITS bit;
} nRF24L01_FAILURE_REG_t;

// ----------------------------------------------------------------------
// nRF24L01 power state
// ----------------------------------------------------------------------
enum nRF24L01_pwr {nRF_pwrDown, nRF_standby};

// ----------------------------------------------------------------------
// nRF24L01 Global struct
// ----------------------------------------------------------------------
typedef struct  {
    nRF24L01_CONFIG_REG_t         CONFIG;           // 0x00
    nRF24L01_EN_AA_REG_t          EN_AA;            // 0x01
    nRF24L01_EN_RXADDR_REG_t      EN_RXADDR;        // 0x02
    nRF24L01_SETUP_AW_REG_t       SETUP_AW;         // 0x03
    nRF24L01_SETUP_RETR_REG_t     SETUP_RETR;       // 0x04
    nRF24L01_RF_CH_REG_t          RF_CH;            // 0x05
    nRF24L01_RF_SETUP_REG_t       RF_SETUP;         // 0x06
    nRF24L01_STATUS_REG_t         STATUS;           // 0x07
    nRF24L01_OBSERVE_TX_REG_t     OBSERVE_TX;       // 0x08
    nRF24L01_RPD_REG_t            RPD;              // 0x09
    uint16_t                      RX_ADDR_P0[nRF24_ADDRESS_WIDTH];    // 0x0A
    uint16_t                      RX_ADDR_P1[nRF24_ADDRESS_WIDTH];    // 0x0B
    uint16_t                      RX_ADDR_P2;       // 0x0C
    uint16_t                      RX_ADDR_P3;       // 0x0D
    uint16_t                      RX_ADDR_P4;       // 0x0E
    uint16_t                      RX_ADDR_P5;       // 0x0F
    uint16_t                      TX_ADDR[nRF24_ADDRESS_WIDTH];       // 0x10
    nRF24L01_RX_PW_P0_REG_t       RX_PW_P0;         // 0x11
    nRF24L01_RX_PW_P1_REG_t       RX_PW_P1;         // 0x12
    nRF24L01_RX_PW_P2_REG_t       RX_PW_P2;         // 0x13
    nRF24L01_RX_PW_P3_REG_t       RX_PW_P3;         // 0x14
    nRF24L01_RX_PW_P4_REG_t       RX_PW_P4;         // 0x15
    nRF24L01_RX_PW_P5_REG_t       RX_PW_P5;         // 0x16
    nRF24L01_FIFO_STATUS_REG_t    FIFO_STATUS;      // 0x17
    nRF24L01_DYNPD_REG_t          DYNPD;            // 0x18
    nRF24L01_FEATURE_REG_t        FEATURE;          // 0x19
    // ------------------------------------------------------------------
    enum nRF24L01_pwr             state;
    uint16_t                      csnPin;
    uint16_t                      cePin;
    uint16_t                      irqPin;
    uint16_t                      rxBuffer[nRF24_PAYLOAD_WIDTH];
    uint16_t                      txBuffer[nRF24_PAYLOAD_WIDTH];
    // ------------------------------------------------------------------
    nRF24L01_FAILURE_REG_t        failure;
    // ------------------------------------------------------------------
    // change following register name to use this
    // code  with another controller platform
    volatile struct SPI_REGS      *SPI_Regs;
} nRF24L01_Vars_t;

// ----------------------------------------------------------------------
// nRF24L01 Global struct defaults
// ----------------------------------------------------------------------
#define NRF24L01_DEFAULTS   {                       \
    0,              /*  CONFIG      */              \
    0,              /*  EN_AA       */              \
    0,              /*  EN_RXADDR   */              \
    0,              /*  SETUP_AW    */              \
    0,              /*  SETUP_RETR  */              \
    0,              /*  RF_CH       */              \
    0,              /*  RF_SETUP    */              \
    0,              /*  STATUS      */              \
    0,              /*  OBSERVE_TX  */              \
    0,              /*  RPD         */              \
    { 0 } ,         /*  RX_ADDR_P0  */              \
    { 0 } ,         /*  RX_ADDR_P1  */              \
    0,              /*  RX_ADDR_P2  */              \
    0,              /*  RX_ADDR_P3  */              \
    0,              /*  RX_ADDR_P4  */              \
    0,              /*  RX_ADDR_P5  */              \
    { 0 } ,         /*  TX_ADDR     */              \
    0,              /*  RX_PW_P0    */              \
    0,              /*  RX_PW_P1    */              \
    0,              /*  RX_PW_P2    */              \
    0,              /*  RX_PW_P3    */              \
    0,              /*  RX_PW_P4    */              \
    0,              /*  RX_PW_P5    */              \
    0,              /*  FIFO Status */              \
    0,              /*  DYNPD       */              \
    0,              /*  FEATURE     */              \
    nRF_pwrDown,    /*  state       */              \
    0,              /*  csnPin      */              \
    0,              /*  cePin       */              \
    0,              /*  irqPin      */              \
    { 0 } ,         /*  rxBuffer    */              \
    { 0 } ,         /*  txBuffer    */              \
    0,              /*  failure     */              \
    &SpiaRegs,      /*  Spi Register */             \
}
// ----------------------------------------------------------------------
// 	functions
// ----------------------------------------------------------------------
//  none

// ----------------------------------------------------------------------
// 	something...
// ----------------------------------------------------------------------
//  none

// ----------------------------------------------------------------------
// 	end of file
// ----------------------------------------------------------------------


#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* NRF24L01_C2000_NRF24L01_DEFS_H_ */
