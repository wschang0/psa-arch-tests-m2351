/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "pal_common.h"
#include "pal_uart.h"
#include "pal_nvmem.h"
#include "pal_wd_cmsdk.h"
#define __IO
#include "uart_reg.h"


/*---------------------------------------------------------------------------------------------------------*/
/* UART_LINE constants definitions                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define UART_WORD_LEN_5     (0UL) /*!< UART_LINE setting to set UART word length to 5 bits */
#define UART_WORD_LEN_6     (1UL) /*!< UART_LINE setting to set UART word length to 6 bits */
#define UART_WORD_LEN_7     (2UL) /*!< UART_LINE setting to set UART word length to 7 bits */
#define UART_WORD_LEN_8     (3UL) /*!< UART_LINE setting to set UART word length to 8 bits */

#define UART_PARITY_NONE    (0x0UL << UART_LINE_PBE_Pos) /*!< UART_LINE setting to set UART as no parity   */
#define UART_PARITY_ODD     (0x1UL << UART_LINE_PBE_Pos) /*!< UART_LINE setting to set UART as odd parity  */
#define UART_PARITY_EVEN    (0x3UL << UART_LINE_PBE_Pos) /*!< UART_LINE setting to set UART as even parity */
#define UART_PARITY_MARK    (0x5UL << UART_LINE_PBE_Pos) /*!< UART_LINE setting to keep parity bit as '1'  */
#define UART_PARITY_SPACE   (0x7UL << UART_LINE_PBE_Pos) /*!< UART_LINE setting to keep parity bit as '0'  */

#define UART_STOP_BIT_1     (0x0UL << UART_LINE_NSB_Pos) /*!< UART_LINE setting for one stop bit */
#define UART_STOP_BIT_1_5   (0x1UL << UART_LINE_NSB_Pos) /*!< UART_LINE setting for 1.5 stop bit when 5-bit word length */
#define UART_STOP_BIT_2     (0x1UL << UART_LINE_NSB_Pos) /*!< UART_LINE setting for two stop bit when 6, 7, 8-bit word length */

/*---------------------------------------------------------------------------------------------------------*/
/* UART BAUDRATE MODE constants definitions                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define UART_BAUD_MODE0     (0UL) /*!< Set UART Baudrate Mode is Mode0 */
#define UART_BAUD_MODE2     (UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk) /*!< Set UART Baudrate Mode is Mode2 */

/**
 *    @brief        Calculate UART baudrate mode2 divider
 *
 *    @param[in]    u32SrcFreq      UART clock frequency
 *    @param[in]    u32BaudRate     Baudrate of UART module
 *
 *    @return       UART baudrate mode2 divider
 *
 *    @details      This macro calculate UART baudrate mode2 divider.
 */
#define UART_BAUD_MODE2_DIVIDER(u32SrcFreq, u32BaudRate)    ((((u32SrcFreq) + ((u32BaudRate)/2ul)) / (u32BaudRate))-2ul)


volatile UART_T *g_UART;


static void SendChar_ToUART(int ch)
{
    while(g_UART->FIFOSTS & UART_FIFOSTS_TXFULL_Msk){}
    g_UART->DAT = (uint32_t)ch;
    if((char)ch == '\n')
    {
        while(g_UART->FIFOSTS & UART_FIFOSTS_TXFULL_Msk){}
        g_UART->DAT = '\r';
    }
}



/**
    @brief    - This function initializes the UART
    @param    - uart base addr
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(uint32_t uart_base_addr)
{
	g_UART = (UART_T *)uart_base_addr;

    g_UART->LINE = UART_PARITY_NONE | UART_STOP_BIT_1 | UART_WORD_LEN_8;
    g_UART->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(12000000, 115200);
		
    return PAL_STATUS_SUCCESS;
}

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO
    @param    - str      : Input String
              - data     : Value for format specifier
    @return   - SUCCESS/FAILURE
**/

int pal_print_ns(char *str, uint32_t data)
{
    int8_t  j, buffer[16];
    int8_t  i = 0, is_neg = 0;

    for (; *str != '\0'; ++str)
    {
        if (*str == '%')
        {
            ++str;
            if (*str == 'd')
            {
                if (data < 0)
                {
                    data = -(data);
                    is_neg = 1;
                }

                while (data != 0)
                {
                    j         = data % 10;
                    data      = data /10;
                    buffer[i] = j + 48;
                    i        += 1;
                }

                if (is_neg)
                    buffer[i++] = '-';
            }
            else if (*str == 'x' || *str == 'X')
            {
                while (data != 0)
                {
                    j         = data & 0xf;
                    data    >>= 4;
                    buffer[i] = j + ((j > 9) ? 55 : 48);
                    i        += 1;
                }
            }
            if (i > 0)
            {
                while (i > 0) {

                    //pal_uart_cmsdk_putc(buffer[--i]);
					SendChar_ToUART(buffer[--i]);
                }
            }
            else
            {
                //pal_uart_cmsdk_putc(48);
				SendChar_ToUART(48);
				
            }
        }
        else
        {
            //pal_uart_cmsdk_putc(*str);
			SendChar_ToUART(*str);
        }
    }

    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_init_ns(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    //return(pal_wd_cmsdk_init(base_addr,time_us, timer_tick_us));

    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_enable_ns(addr_t base_addr)
{
    //return(pal_wd_cmsdk_enable(base_addr));
	
    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr  : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_disable_ns(addr_t base_addr)
{
    //return (pal_wd_cmsdk_disable(base_addr));
    return PAL_STATUS_SUCCESS;
    
}

/**
    @brief    - Reads from given non-volatile address.
    @param    - base    : Base address of nvmem
                offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
int pal_nvmem_read_ns(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (nvmem_read(base, offset, buffer, size))
    {
        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
}

/**
    @brief    - Writes into given non-volatile address.
    @param    - base    : Base address of nvmem
                offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
int pal_nvmem_write_ns(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (nvmem_write(base, offset, buffer, size))
    {
        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
}

/**
 *   @brief    - This function will read peripherals using SPI commands
 *   @param    - addr : address of the peripheral
 *               data : read buffer
 *               len  : length of the read buffer in bytes
 *   @return   - error status
**/
int pal_spi_read(addr_t addr, uint8_t *data, uint32_t len)
{
    return 0xFF;
}

/**
 *   @brief    - Terminates the simulation at the end of all tests completion.
 *               By default, it put cpus into power down mode.
 *   @param    - void
 *   @return   - void
**/
void pal_terminate_simulation(void)
{
    /* Add logic to terminate the simluation */

    while(1)
    {
        asm volatile("WFI");
    }
}
