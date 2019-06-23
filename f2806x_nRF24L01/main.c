// ----------------------------------------------------------------------
// 	info and license
// ----------------------------------------------------------------------
//
//	filename:	main.c
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
//  Controller based files are provided by Texas Instruments, see:
//
//  $TI Release: F2806x Support Library v2.04.00.00 $
//  $Release Date: Thu Oct 18 15:47:20 CDT 2018 $
//  $Copyright:
//  Copyright (C) 2009-2018 Texas Instruments Incorporated - http://www.ti.com/
// ----------------------------------------------------------------------
// 	history
// ----------------------------------------------------------------------
// 	23.06.2019 - initial programming

// ----------------------------------------------------------------------
// 	header files
// ----------------------------------------------------------------------
#include "DSP28x_Project.h"
#include "nRF24L01_C2000/nRF24L01.h"

// ----------------------------------------------------------------------
// 	#defines
// ----------------------------------------------------------------------
#define D9  GPIO34
#define D10 GPIO39

#define TX_EN   1       // select device is TX and / or
#define RX_EN   1     // RX

// ----------------------------------------------------------------------
// 	global variables
// ----------------------------------------------------------------------
nRF24L01_Vars_t nRF_TX = NRF24L01_DEFAULTS;
nRF24L01_Vars_t nRF_RX = NRF24L01_DEFAULTS;

uint16_t rxData;
uint16_t txData;

// ----------------------------------------------------------------------
// 	functions
// ----------------------------------------------------------------------
__interrupt void cpu_timer0_isr(void);
__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);

// ----------------------------------------------------------------------
// Configure Spi Register with FIFO use and no interrupt // 4 Wire
// ----------------------------------------------------------------------
void SPI_Config4Wire(volatile struct SPI_REGS *v)
{
    v->SPICCR.bit.SPISWRESET    = 0;    // force reset
    v->SPICCR.bit.CLKPOLARITY   = 0;    // 0 = rising edge // 1 = falling edge
    v->SPICCR.bit.SPILBK        = 0;    // loopback mode disable
    v->SPICCR.bit.SPICHAR       = 7;    // 8bit char length
    v->SPICTL.bit.OVERRUNINTENA = 0;    // overrun INT disable
    v->SPICTL.bit.CLK_PHASE     = 1;    // 0 = without delay // 1 = with delay
    v->SPICTL.bit.MASTER_SLAVE  = 1;    // SPI master mode
    v->SPICTL.bit.TALK          = 1;    // transmit is enabled
    v->SPICTL.bit.SPIINTENA     = 0;    // INT disable
    // for SPIBRR >= (3-127):
    /* SPI BAUD RATE = LSPCLK(default is 22,5MHz) / (SPIBRR +1) */
    // for SPIBRR < 3:
    /* SPI BAUD RATE = LSPCLK(default is 22,5MHz) / 4 */
    v->SPIBRR                   = 3;    // sets SPI Clk up to 3,75MHz
    v->SPIPRI.bit.TRIWIRE       = 0;    // 4-wire mode
    v->SPIPRI.bit.FREE          = 1;    // breakpoints don't disturb xmission
    v->SPICCR.bit.SPISWRESET    = 1;    // take out of reset

    // TX FIFO
    v->SPIFFTX.bit.SPIRST       = 1;    // use FIFO
    v->SPIFFTX.bit.SPIFFENA     = 1;    // use FIFO
    v->SPIFFTX.bit.TXFIFO       = 0;    // reset FIFO ptr.
    v->SPIFFTX.bit.TXFIFO       = 1;    // re-enable FIFO
    v->SPIFFTX.bit.TXFFIENA     = 0;    // FIFO INT disable
    v->SPIFFTX.bit.TXFFINTCLR   = 1;    // clear INT flag

    // RX FIFO
    v->SPIFFRX.bit.RXFFOVFCLR   = 1;    // clear overflow flag
    v->SPIFFRX.bit.RXFIFORESET  = 0;    // reset FIFO ptr.
    v->SPIFFRX.bit.RXFIFORESET  = 1;    // re-enable FIFO
    v->SPIFFRX.bit.RXFFOVFCLR   = 1;    // clear INT flag
    v->SPIFFRX.bit.RXFFIENA     = 0;    // INT disable
    v->SPIFFRX.bit.RXFFIL       = 2;    // INT Flag after 2 words in FIFO
    //
    v->SPIFFCT.all              = 0x00; // no use of this register
}

// ----------------------------------------------------------------------
// Start of main
// ----------------------------------------------------------------------
void main(void)
{
    // ----------------------------------------------------------------------
    // SYSTEM
    // ----------------------------------------------------------------------
    InitSysCtrl();
    DINT;   // diable ISRs

    // ----------------------------------------------------------------------
    // GPIO
    // ----------------------------------------------------------------------
    InitGpio();

    GpioDataRegs.GPBSET.bit.D9 = 1;
    GpioDataRegs.GPBSET.bit.D10 = 1;

    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.D9 = 1;     // LED / D9 output
    GpioCtrlRegs.GPBDIR.bit.D10 = 1;    // LED / D10 output
    EDIS;

    // ----------------------------------------------------------------------
    // TIMERS
    // ----------------------------------------------------------------------
    InitCpuTimers();   // For this example, only initialize the Cpu Timers

    // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
    // 90MHz CPU Freq, 1 second Period (in uSeconds)
    ConfigCpuTimer(&CpuTimer0, 90, 1000000/4);
    ConfigCpuTimer(&CpuTimer1, 90, 1000000/16);
    ConfigCpuTimer(&CpuTimer2, 90, 1000000);

    // To ensure precise timing, use write-only instructions to write to the
    // entire register. Therefore, if any of the configuration bits are changed
    // in ConfigCpuTimer and InitCpuTimers (in F2806x_CpuTimers.h), the below

    // Use write-only instruction to set TSS bit = 0
    CpuTimer0Regs.TCR.all = 0x4000;
    CpuTimer1Regs.TCR.all = 0x4000;
    CpuTimer2Regs.TCR.all = 0x4000;

    // ----------------------------------------------------------------------
    // SPI
    // ----------------------------------------------------------------------
    InitSpiaGpio();
    InitSpibGpio();
    SPI_Config4Wire(&SpiaRegs);
    SPI_Config4Wire(&SpibRegs);

#ifdef TX_EN
    // ----------------------------------------------------------------------
    // nRF24 TX settings
    // ----------------------------------------------------------------------
    nRF_TX.cePin  = 50;
    nRF_TX.csnPin = 51;
    nRF_TX.irqPin = 22;
    nRF_TX.SPI_Regs = &SpiaRegs;

    // set GPIOs here cause there is no function so far in init
    GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;   // CE low
    GpioDataRegs.GPBSET.bit.GPIO51   = 1;   // CSN high
    GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;   // LED low

    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;     // CE pin output
    GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;     // CSN pin output
    GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1;     // LED pin output
    EDIS;

    // Step 1: get all initial values from nRF24 and make basic setting
    nRF24_Init(&nRF_TX);

    // Step 2: set address width, RX & TX address afterwards
    nRF_TX.SETUP_AW.bit.AW = nRF_5Bytes;
    nRF24_SPI_Write(&nRF_TX, nRF24_SETUP_AW);

    nRF_TX.TX_ADDR[0] = 0xDE;
    nRF_TX.TX_ADDR[1] = 0xAD;
    nRF_TX.TX_ADDR[2] = 0xBE;
    nRF_TX.TX_ADDR[3] = 0xEF;
    nRF_TX.TX_ADDR[4] = 0xF0;
    nRF24_SetTxAddress(&nRF_TX);

    // Pipe 0 receives auto-ack's, autoacks are sent back to the TX addr so the PTX node
    // needs to listen to the TX addr on pipe#0 to receive them.
    nRF_TX.RX_ADDR_P0[0] = 0xDE;
    nRF_TX.RX_ADDR_P0[1] = 0xAD;
    nRF_TX.RX_ADDR_P0[2] = 0xBE;
    nRF_TX.RX_ADDR_P0[3] = 0xEF;
    nRF_TX.RX_ADDR_P0[4] = 0xF0;
    nRF24_SetRxAddress(&nRF_TX, nRF24_PIPE_0);

    // Step 3: open used pipes & set payload width
    nRF_TX.EN_RXADDR.bit.ERX_P0 = 1;    // enable PIPE 0
    nRF24_SPI_Write(&nRF_TX, nRF24_EN_RXADDR);

    nRF_TX.RX_PW_P0.all = (uint16_t)nRF24_PAYLOAD_WIDTH;
    nRF24_SPI_Write(&nRF_TX, nRF24_RX_PW_P0);

    // Step 4: speed, power & channel setting
    nRF24_SetDataRate(&nRF_TX, nRF_250kbit);
    nRF_TX.RF_SETUP.bit.RF_PWR = nRF_minus0dBm;
    nRF24_SPI_Write(&nRF_TX, nRF24_RF_SETUP);
    nRF_TX.RF_CH.bit.RF_CH = 120;
    nRF24_SPI_Write(&nRF_TX, nRF24_RF_CH);

    // Step 5: power up and set to standby
    nRF24_Standby(&nRF_TX);

    // only for debug
    nRF24_GetRegValues(&nRF_TX);
#endif
#ifdef RX_EN
    // ----------------------------------------------------------------------
    // nRF24 RX settings
    // ----------------------------------------------------------------------
    nRF_RX.cePin  = 52;
    nRF_RX.csnPin = 53;
    nRF_RX.irqPin = 21;
    nRF_RX.SPI_Regs = &SpibRegs;

    // set GPIOs here cause there is no function so far in init
    GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;   // CE low
    GpioDataRegs.GPBSET.bit.GPIO53   = 1;   // CSN high
    GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;   // LED low

    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;     // CE pin output
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 1;     // CSN pin output
    GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;     // LED pin output
    EDIS;

    // Step 1: get all initial values from nRF24 and make basic setting
    nRF24_Init(&nRF_RX);

    // Step 2: set address width, RX & TX address afterwards
    nRF_RX.SETUP_AW.bit.AW = nRF_5Bytes;
    nRF24_SPI_Write(&nRF_RX, nRF24_SETUP_AW);

    nRF_RX.TX_ADDR[0] = 0xDE;
    nRF_RX.TX_ADDR[1] = 0xAD;
    nRF_RX.TX_ADDR[2] = 0xBE;
    nRF_RX.TX_ADDR[3] = 0xEF;
    nRF_RX.TX_ADDR[4] = 0xF0;
    nRF24_SetTxAddress(&nRF_RX);

    // Pipe 0 receives auto-ack's, autoacks are sent back to the TX addr so the PTX node
    // needs to listen to the TX addr on pipe#0 to receive them.
    nRF_RX.RX_ADDR_P0[0] = 0xDE;
    nRF_RX.RX_ADDR_P0[1] = 0xAD;
    nRF_RX.RX_ADDR_P0[2] = 0xBE;
    nRF_RX.RX_ADDR_P0[3] = 0xEF;
    nRF_RX.RX_ADDR_P0[4] = 0xF0;
    nRF24_SetRxAddress(&nRF_RX, nRF24_PIPE_0);

    // Step 3: open used pipes & set payload width
    nRF_RX.EN_RXADDR.bit.ERX_P0 = 1;    // enable PIPE 0
    nRF24_SPI_Write(&nRF_RX, nRF24_EN_RXADDR);

    nRF_RX.RX_PW_P0.all = (uint16_t)nRF24_PAYLOAD_WIDTH;
    nRF24_SPI_Write(&nRF_RX, nRF24_RX_PW_P0);

    // Step 4: speed, power & channel
    nRF24_SetDataRate(&nRF_RX, nRF_250kbit);
    nRF_RX.RF_SETUP.bit.RF_PWR = nRF_minus0dBm;
    nRF24_SPI_Write(&nRF_RX, nRF24_RF_SETUP);
    nRF_RX.RF_CH.bit.RF_CH = 120;
    nRF24_SPI_Write(&nRF_RX, nRF24_RF_CH);

    // Step 5: power up and set in RX control mode
    nRF24_ActivateRx(&nRF_RX);

    // only for debug
    nRF24_GetRegValues(&nRF_RX);
#endif
    // ----------------------------------------------------------------------
    // ISR
    // ----------------------------------------------------------------------
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.TINT0 = &cpu_timer0_isr;
    PieVectTable.TINT1 = &cpu_timer1_isr;
    PieVectTable.TINT2 = &cpu_timer2_isr;
    EDIS;    // This is needed to disable write to EALLOW protected registers
    // Enable CPU int1 which is connected to CPU-Timer 0, CPU int13
    // which is connected to CPU-Timer 1, and CPU int 14, which is connected
    // to CPU-Timer 2
    IER |= M_INT1;
    IER |= M_INT13;
    IER |= M_INT14;

    // Enable TINT0 in the PIE: Group 1 interrupt 7
    // Enable global Interrupts and higher priority real-time debug events:
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

    while(1)
    {
        rxData = nRF_RX.rxBuffer[0];
        txData = nRF_TX.txBuffer[0];
    }
}

// ----------------------------------------------------------------------
// End of main
// ----------------------------------------------------------------------

__interrupt void
cpu_timer0_isr(void)
{
#ifdef TX_EN
    // show TX via LED
    GpioDataRegs.GPBTOGGLE.bit.D10 = 1;

    if(++CpuTimer0.InterruptCount > 255)
        CpuTimer0.InterruptCount = 1;

    nRF_TX.txBuffer[0] = CpuTimer0.InterruptCount;
    nRF_TX.txBuffer[1] = CpuTimer0.InterruptCount;

    nRF24_SPI_WriteTxPayload(&nRF_TX);
    nRF24_ActivateTx(&nRF_TX);
#endif
    // Acknowledge this interrupt to receive more interrupts from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//
// cpu_timer1_isr -
//
__interrupt void
cpu_timer1_isr(void)
{
    EALLOW;
#ifdef RX_EN
    CpuTimer1.InterruptCount++;

    // read out status; polling...
    nRF24_SPI_Read(&nRF_RX, nRF24_STATUS);

    if(nRF_RX.STATUS.bit.RX_DR)
    {
        // show RX via LED
        GpioDataRegs.GPBTOGGLE.bit.D9 = 1;

        nRF24_SPI_ReadRxPayload(&nRF_RX);
        nRF_RX.STATUS.bit.RX_DR = 1;    // clear flag
        nRF24_SPI_Write(&nRF_RX, nRF24_STATUS);

        nRF24_SPI_Read(&nRF_RX, nRF24_FIFO_STATUS);
        if(!nRF_RX.FIFO_STATUS.bit.RX_EMPTY)
            nRF24_SPI_ReadRxPayload(&nRF_RX);
    }
#endif
    // The CPU acknowledges the interrupt
    EDIS;
}

//
// cpu_timer2_isr -
//
__interrupt void
cpu_timer2_isr(void)
{
    EALLOW;
    CpuTimer2.InterruptCount++;

    // The CPU acknowledges the interrupt.
    EDIS;
}

// ----------------------------------------------------------------------
// 	end of file
// ----------------------------------------------------------------------

