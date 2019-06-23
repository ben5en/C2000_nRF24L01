#ifndef NRF24L01_C2000_NRF24L01_SPI_H_
#define NRF24L01_C2000_NRF24L01_SPI_H_
#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------
// 	info and license
// ----------------------------------------------------------------------
//
//	filename:	nRF24L01_SPI.h
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

// ----------------------------------------------------------------------
// 	#defines
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	global variables
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	functions
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SPI driver to interface with nRF24L01
// Every new command must be started by a high to low transition on CSN.
// ----------------------------------------------------------------------
static inline uint16_t nRF24_SPI_Driver(nRF24L01_Vars_t *v, uint16_t data)
{
    const uint16_t timeout = mSec0_5;
    uint16_t count = 0;

    v->SPI_Regs->SPITXBUF = (data << 8);        // send out the data
    while(v->SPI_Regs->SPIFFRX.bit.RXFFST !=1)  // wait for the packet to complete
    {
        if(++count >= timeout)
        {
            v->failure.bit.SPI_Timeout= 1;
            break;
        }
    }

    return(v->SPI_Regs->SPIRXBUF);
}
// ----------------------------------------------------------------------
// 	something...
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	end of file
// ----------------------------------------------------------------------


#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* NRF24L01_C2000_NRF24L01_SPI_H_ */
