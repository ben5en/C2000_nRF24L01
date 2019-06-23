#ifndef NRF24L01_C2000_NRF24L01_H_
#define NRF24L01_C2000_NRF24L01_H_

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------
// 	info and license
// ----------------------------------------------------------------------
//
//	filename:	nRF24L01.h
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
#include "../nRF24L01_C2000/nRF24L01_defs.h"
#include "../nRF24L01_C2000/nRF24L01_SPI.h"

// ----------------------------------------------------------------------
// 	#defines
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	global variables
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	functions
// ----------------------------------------------------------------------
void nRF24_Init(nRF24L01_Vars_t *v);
void nRF24_GetRegValues(nRF24L01_Vars_t *v);
void nRF24_SetRxAddress(nRF24L01_Vars_t *v, uint16_t pipe);
void nRF24_SetTxAddress(nRF24L01_Vars_t *v);
void nRF24_SetDataRate(nRF24L01_Vars_t *v, enum nRF24L01_dataRate x);
void nRF24_Standby(nRF24L01_Vars_t *v);
void nRF24_PwrDown(nRF24L01_Vars_t *v);
void nRF24_ActivateTx(nRF24L01_Vars_t *v);
void nRF24_ActivateRx(nRF24L01_Vars_t *v);

//
void nRF24_SPI_Read(nRF24L01_Vars_t *v, uint16_t address);
void nRF24_SPI_Write(nRF24L01_Vars_t *v, uint16_t address);
void nRF24_SPI_ReadRxPayload(nRF24L01_Vars_t *v);
void nRF24_SPI_WriteTxPayload(nRF24L01_Vars_t *v);
void nRF24_SPI_FlushTx(nRF24L01_Vars_t *v);
void nRF24_SPI_FlushRx(nRF24L01_Vars_t *v);
void nRF24_SPI_ReuseTxPayload(nRF24L01_Vars_t *v);
uint16_t nRF24_SPI_ReadRxPayloadWid(nRF24L01_Vars_t *v);
void nRF24_SPI_WriteAckPayload(nRF24L01_Vars_t *v);
void nRF24_SPI_WriteTxPayloadNoAck(nRF24L01_Vars_t *v);
void nRF24_PulseCE(nRF24L01_Vars_t *v);
// ----------------------------------------------------------------------
// 	something...
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// 	end of file
// ----------------------------------------------------------------------


#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* NRF24L01_C2000_NRF24L01_H_ */
