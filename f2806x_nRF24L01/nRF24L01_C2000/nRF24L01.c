// ----------------------------------------------------------------------
// 	info and license
// ----------------------------------------------------------------------
//
//	filename:	nRF24L01.c
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
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "nRF24L01.h"

// ----------------------------------------------------------------------
// 	#defines
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	global variables
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	functions
// ----------------------------------------------------------------------
// nRF24_WritePin - Set the GPyDAT register bit for the specified pin.
// source: F2837xS_Gpio.c v210
// ----------------------------------------------------------------------
static inline void nRF24_WritePin(uint16_t pin, uint16_t outVal)
{
    const uint32_t gpyDataOffset = (0x8/2);
    const uint32_t gpyClear = (0x4/2);
    const uint32_t gpySet = (0x2/2);
    volatile uint32_t *gpioDataReg;
    uint32_t pinMask;

    gpioDataReg = (volatile uint32_t *)&GpioDataRegs + (pin/32)*gpyDataOffset;
    pinMask = 1UL << (pin % 32);

    if (outVal == 0)
    {
        gpioDataReg[gpyClear] = pinMask;
    }
    else
    {
        gpioDataReg[gpySet] = pinMask;
    }
}


// ----------------------------------------------------------------------
// nRF24_CalcAddrOffset - get address offset for structure pointer
// ----------------------------------------------------------------------
static inline uint16_t nRF24_CalcAddrOffset(uint16_t addrForOffset)
{
    uint16_t offset = 0;

    if(addrForOffset >= nRF24_RX_ADDR_P2)
        offset = 2*(nRF24_ADDRESS_WIDTH - 1);
    if(addrForOffset >= nRF24_TX_ADDR)
        offset += nRF24_ADDRESS_WIDTH - 1;
    if(addrForOffset >= nRF24_DYNPD)
        offset -= 4;

    return offset;
}

// ----------------------------------------------------------------------
// Read register value and write in global structure:
// ----------------------------------------------------------------------
void nRF24_SPI_Read(nRF24L01_Vars_t *v, uint16_t address)
{
    uint16_t *cntrlReg;
    uint16_t offset = nRF24_CalcAddrOffset(address);

    cntrlReg = &(v->CONFIG.all);

    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_R_REGISTER | address);
    cntrlReg[address + offset] = nRF24_SPI_Driver(v, nRF24_NOP);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// Write register value taken from global structure:
// ----------------------------------------------------------------------
void nRF24_SPI_Write(nRF24L01_Vars_t *v, uint16_t address)
{
    uint16_t *cntrlReg;
    uint16_t offset = nRF24_CalcAddrOffset(address);

    cntrlReg = &(v->CONFIG.all);

    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_W_REGISTER | address);
    nRF24_SPI_Driver(v, cntrlReg[address + offset]);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// Read payload and store it in global structure:
// ----------------------------------------------------------------------
void nRF24_SPI_ReadRxPayload(nRF24L01_Vars_t *v)
{
    uint16_t payloadWidth;
    uint16_t i;

    payloadWidth = nRF24_SPI_ReadRxPayloadWid(v);
    if(payloadWidth > 32)
    {
        nRF24_SPI_FlushRx(v);
        return;
    }

    // reset buffer
    for(i=0;i<nRF24_PAYLOAD_WIDTH;i++)
        v->rxBuffer[i] = 0x00;

    // readout payload
    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_R_RX_PAYLOAD);
    for(i=0;i<payloadWidth;i++)
        v->rxBuffer[i] = nRF24_SPI_Driver(v, nRF24_NOP);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// Write payload in TX mode:
// ----------------------------------------------------------------------
void nRF24_SPI_WriteTxPayload(nRF24L01_Vars_t *v)
{
    uint16_t i;

    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_W_TX_PAYLOAD);
    for(i=0;i<nRF24_PAYLOAD_WIDTH;i++)
        nRF24_SPI_Driver(v, v->txBuffer[i]);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// Flush TX FIFO (used in TX mode)
// ----------------------------------------------------------------------
void nRF24_SPI_FlushTx(nRF24L01_Vars_t *v)
{
    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_FLUSH_TX);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// Flush RX FIFO (used in RX mode)
// ----------------------------------------------------------------------
void nRF24_SPI_FlushRx(nRF24L01_Vars_t *v)
{
    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_FLUSH_RX);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// Instruction to reuse last transmitted payload
// ----------------------------------------------------------------------
void nRF24_SPI_ReuseTxPayload(nRF24L01_Vars_t *v)
{
    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_REUSE_TX_PL);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// Read payload width and return value
// ----------------------------------------------------------------------
uint16_t nRF24_SPI_ReadRxPayloadWid(nRF24L01_Vars_t *v)
{
    uint16_t rx;

    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_R_RX_PL_WID);
    rx = nRF24_SPI_Driver(v, nRF24_NOP);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high

    return rx;
}

// ----------------------------------------------------------------------
// Write payload in RX mode to be transmitted with ACK packet:
// • For 2Mbps data rate and 5 byte address; 15 byte is maximum ACK packet
//   payload length for ARD=250us (reset value).
// • For 1Mbps data rate and 5 byte address; 5 byte is maximum ACK packet
//   payload length for ARD=250us (reset value).
// ----------------------------------------------------------------------
void nRF24_SPI_WriteAckPayload(nRF24L01_Vars_t *v)
{
    uint16_t i;

    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_W_ACK_PAYLOAD);
    for(i=0;i<nRF24_PAYLOAD_WIDTH;i++)
        nRF24_SPI_Driver(v, v->txBuffer[i]);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// Write payload in TX mode; disables AUTOACK on this specific packet:
// ----------------------------------------------------------------------
void nRF24_SPI_WriteTxPayloadNoAck(nRF24L01_Vars_t *v)
{
    uint16_t i;

    nRF24_WritePin(v->csnPin, 0);           // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_W_TX_PAYLOAD_NOACK);
    for(i=0;i<nRF24_PAYLOAD_WIDTH;i++)
        nRF24_SPI_Driver(v, v->txBuffer[i]);
    nRF24_WritePin(v->csnPin, 1);           // CSN pin high
}

// ----------------------------------------------------------------------
// nRF24_PulseCE
// ----------------------------------------------------------------------
void nRF24_PulseCE(nRF24L01_Vars_t *v)
{
    nRF24_WritePin(v->cePin, 1);           // CE pin high
    DELAY_US(nRF24_CE_PULSE_DURATION);
    nRF24_WritePin(v->cePin, 0);           // CE pin low
}

// ----------------------------------------------------------------------
// Init nRF24L01
// ----------------------------------------------------------------------
void nRF24_Init(nRF24L01_Vars_t *v)
{
    uint16_t i;

    // check if settings are made correctly
    if(nRF24_ADDRESS_WIDTH > 5 || nRF24_ADDRESS_WIDTH < 3)
    {
        v->failure.bit.ADDR_Range = 1;
        return;
    }

    if(nRF24_PAYLOAD_WIDTH > 32 || nRF24_PAYLOAD_WIDTH < 0)
    {
        v->failure.bit.PAYLOAD_Range= 1;
        return;
    }

    // delay 100ms to pwr up Vddio > 1.9V
    DELAY_US(nRF24_VDD_UP_DURATION);

    nRF24_GetRegValues(v);

    if(!v->CONFIG.bit.PWR_UP)
        v->state = nRF_pwrDown;
    else
        v->state = nRF_standby;

    // clear possible pending IRQ
    v->STATUS.bit.MAX_RT = 1;
    v->STATUS.bit.RX_DR = 1;
    v->STATUS.bit.TX_DS = 1;
    nRF24_SPI_Write(v, nRF24_STATUS);

    // close all pipes & reset payload width
    v->EN_RXADDR.all = 0x00;
    nRF24_SPI_Write(v, nRF24_EN_RXADDR);

    // retransmit settings
    v->SETUP_RETR.bit.ARD = nRF_Wait_Retrans_2000uS;
    v->SETUP_RETR.bit.ARC = 15;
    nRF24_SPI_Write(v, nRF24_SETUP_RETR);

    // feature settings
    v->FEATURE.bit.EN_DPL = 1;
    // possible bug on some "Chinese clones" -> so leave this one turned off
    v->FEATURE.bit.EN_DYN_ACK = 0;
    v->FEATURE.bit.EN_ACK_PAY = 1;
    nRF24_SPI_Write(v, nRF24_FEATURE);

    // dynamic payload settings
    v->DYNPD.bit.DPL_P0 = 1;
    nRF24_SPI_Write(v, nRF24_DYNPD);

    nRF24_SPI_FlushTx(v);
    nRF24_SPI_FlushRx(v);

    // reset buffer´s
    for(i=0;i<nRF24_PAYLOAD_WIDTH;i++)
    {
        v->txBuffer[i] = 0;
        v->rxBuffer[i] = 0;
    }
}

// ----------------------------------------------------------------------
// read "all" register to get reset or actual values
// ----------------------------------------------------------------------
void nRF24_GetRegValues(nRF24L01_Vars_t *v)
{
    nRF24_SPI_Read(v, nRF24_CONFIG);
    nRF24_SPI_Read(v, nRF24_EN_AA);
    nRF24_SPI_Read(v, nRF24_EN_RXADDR);
    nRF24_SPI_Read(v, nRF24_SETUP_AW);
    nRF24_SPI_Read(v, nRF24_SETUP_RETR);
    nRF24_SPI_Read(v, nRF24_RF_CH);
    nRF24_SPI_Read(v, nRF24_RF_SETUP);
    nRF24_SPI_Read(v, nRF24_OBSERVE_TX);
    nRF24_SPI_Read(v, nRF24_RPD);
    nRF24_SPI_Read(v, nRF24_FIFO_STATUS);
    nRF24_SPI_Read(v, nRF24_DYNPD);
    nRF24_SPI_Read(v, nRF24_FEATURE);
}

// ----------------------------------------------------------------------
// Set nRF24L01 rx addresses pipe X
// ----------------------------------------------------------------------
void nRF24_SetRxAddress(nRF24L01_Vars_t *v, uint16_t pipe)
{
    uint16_t i, x;

    if(pipe > 5)
    {
        v->failure.bit.PIPE_Range = 1;
        return;
    }

    // need to know the address witdh
    switch (v->SETUP_AW.bit.AW)
    {
    case 0:
        x = 0;
        break;
    case nRF_3Bytes:
        x = 3;
        break;
    case nRF_4Bytes:
        x = 4;
        break;
    case nRF_5Bytes:
        x = 5;
        break;
    default:
        return;
    }

    if (x < 3)
    {
        v->failure.bit.ADDR_Range = 1;
        return; // return @ illegal value
    }

    switch (pipe)
    {
    case 0:
        nRF24_WritePin(v->csnPin, 0);           // CSN pin low
        v->STATUS.all = nRF24_SPI_Driver(v, nRF24_W_REGISTER | nRF24_RX_ADDR_P0);
        for( i = 0; i < x; i++)
            nRF24_SPI_Driver(v, v->RX_ADDR_P0[i]);
        nRF24_WritePin(v->csnPin, 1);           // CSN pin high
        break;
        // ----------------------------------------------------------------------
    case 1:
        nRF24_WritePin(v->csnPin, 0);           // CSN pin low
        v->STATUS.all = nRF24_SPI_Driver(v, nRF24_W_REGISTER | nRF24_RX_ADDR_P1);
        for( i = 0; i < x; i++)
            nRF24_SPI_Driver(v, v->RX_ADDR_P1[i]);
        nRF24_WritePin(v->csnPin, 1);           // CSN pin high
        break;
        // ----------------------------------------------------------------------
    case 2:
        nRF24_SPI_Write(v, nRF24_RX_ADDR_P2);
        break;
        // ----------------------------------------------------------------------
    case 3:
        nRF24_SPI_Write(v, nRF24_RX_ADDR_P3);
        break;
        // ----------------------------------------------------------------------
    case 4:
        nRF24_SPI_Write(v, nRF24_RX_ADDR_P4);
        break;
        // ----------------------------------------------------------------------
    case 5:
        nRF24_SPI_Write(v, nRF24_RX_ADDR_P5);
        break;
        // ----------------------------------------------------------------------
    default:
        break;
    }
}

// ----------------------------------------------------------------------
// Set nRF24L01 tx addresses
// ----------------------------------------------------------------------
void nRF24_SetTxAddress(nRF24L01_Vars_t *v)
{
    uint16_t i, x;

    // need to know the address witdh
    switch (v->SETUP_AW.bit.AW)
    {
    case 0:
        x = 0;
        break;
    case nRF_3Bytes:
        x = 3;
        break;
    case nRF_4Bytes:
        x = 4;
        break;
    case nRF_5Bytes:
        x = 5;
        break;
    }

    if (x < 3)
    {
        v->failure.bit.ADDR_Range = 1;
        return; // return @ illegal value
    }

    nRF24_WritePin(v->csnPin, 0);       // CSN pin low
    v->STATUS.all = nRF24_SPI_Driver(v, nRF24_W_REGISTER | nRF24_TX_ADDR);
    for( i = 0; i < x; i++)
        nRF24_SPI_Driver(v, v->TX_ADDR[i]);
    nRF24_WritePin(v->csnPin, 1);       // CSN pin high
}


// ----------------------------------------------------------------------
// Set nRF24L01 RF data rate
//
// ----------------------------------------------------------------------
void nRF24_SetDataRate(nRF24L01_Vars_t *v, enum nRF24L01_dataRate x)
{
    if(x > 3)
    {
        v->failure.bit.DATA_Range = 1;
        return;
    }

    // read - write operation
    nRF24_SPI_Read(v, nRF24_RF_SETUP);

    switch (x)
    {
    case nRF_250kbit:
        v->RF_SETUP.bit.RF_DR_LOW = 1;
        v->RF_SETUP.bit.RF_DR_HIGH = 0;
        break;
    case nRF_1Mbps:
        v->RF_SETUP.bit.RF_DR_LOW = 0;
        v->RF_SETUP.bit.RF_DR_HIGH = 0;
        break;
    case nRF_2Mbps:
        v->RF_SETUP.bit.RF_DR_LOW = 0;
        v->RF_SETUP.bit.RF_DR_HIGH = 1;
        break;
    }

    nRF24_SPI_Write(v, nRF24_RF_SETUP);
}

// ----------------------------------------------------------------------
// nRF24 to standby Mode
// ----------------------------------------------------------------------
void nRF24_Standby(nRF24L01_Vars_t *v)
{
    v->CONFIG.bit.PWR_UP = 1;
    nRF24_SPI_Write(v, nRF24_CONFIG);
    DELAY_US(nRF24_PWR_UP_DURATION); // -> need time to pwr up

    // check if change to standby has worked
    nRF24_SPI_Read(v, nRF24_CONFIG);

    if(v->CONFIG.bit.PWR_UP)
        v->state = nRF_standby;
    else
        v->failure.bit.PWR_Change = 1;
}

// ----------------------------------------------------------------------
// nRF24 to power down Mode
// ----------------------------------------------------------------------
void nRF24_PwrDown(nRF24L01_Vars_t *v)
{
    v->CONFIG.bit.PWR_UP = 0;
    nRF24_SPI_Write(v, nRF24_CONFIG);

    // check if change to standby has worked
    nRF24_SPI_Read(v, nRF24_CONFIG);

    if(!v->CONFIG.bit.PWR_UP)
        v->state = nRF_pwrDown;
    else
        v->failure.bit.PWR_Change = 1;
}

// ----------------------------------------------------------------------
// Tx Data
// ----------------------------------------------------------------------
void nRF24_ActivateTx(nRF24L01_Vars_t *v)
{
    if(v->state != nRF_standby)
        nRF24_Standby(v);

    // Cancel any outstanding TX interrupt
    v->STATUS.bit.MAX_RT = 1;
    v->STATUS.bit.TX_DS = 1;
    nRF24_SPI_Write(v, nRF24_STATUS);

    v->CONFIG.bit.PRIM_RX = 0;
    nRF24_SPI_Write(v, nRF24_CONFIG);

    nRF24_PulseCE(v);
}

// ----------------------------------------------------------------------
// Rx Data
// ----------------------------------------------------------------------
void nRF24_ActivateRx(nRF24L01_Vars_t *v)
{
    if(v->state != nRF_standby)
        nRF24_Standby(v);

    // clear FIFO
    nRF24_SPI_FlushRx(v);

    // Cancel any outstanding RX interrupt
    v->STATUS.bit.RX_DR = 1;
    nRF24_SPI_Write(v, nRF24_STATUS);

    // enable PRIM_RX
    v->CONFIG.bit.PRIM_RX = 1;
    nRF24_SPI_Write(v, nRF24_CONFIG);

    // CE high so nRF is in RX mode
    nRF24_WritePin(v->cePin, 1);
    DELAY_US(nRF24_CE_FOR_RX_DURATION);
}
// ----------------------------------------------------------------------
// 	something...
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	end of file
// ----------------------------------------------------------------------

