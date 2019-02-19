/*
 * Copyright (c) 2016-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cmsis.h"

/*
 * MPS2 AN519 has different frequencies for system core clock (20MHz) and
 * peripherals clock (25MHz).
 */
#define  OSCCLK0          (40000000UL)
#define  OSCCLK2          (25000000UL)

#define  SYSTEM_CLOCK     (OSCCLK0/2)
#define  PERIPHERAL_CLOCK (OSCCLK2)

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  extern uint32_t __Vectors;
#endif

uint32_t SystemCoreClock = SYSTEM_CLOCK;
uint32_t PeripheralClock = PERIPHERAL_CLOCK;

/* System Core Clock update function */
void SystemCoreClockUpdate (void)
{
  SystemCoreClock = SYSTEM_CLOCK;
}

/* System initialization function */
void SystemInit (void)
{
#if __DOMAIN_NS != 1U
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t) &__Vectors;
#endif
#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif
#endif /* __DOMAIN_NS != 1U */

  SystemCoreClock = SYSTEM_CLOCK;
  PeripheralClock = PERIPHERAL_CLOCK;

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3)


    /* Initial the system */    
    SYS_UnlockReg(); 
    
    /* Enable PLL */    
    CLK->PLLCTL = CLK_PLLCTL_64MHz_HIRC;
    
    /* Waiting for PLL stable */    
    while((CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == 0);    
    
    /* Switch HCLK clock source to PLL */    
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_PLL;        /* Enable IP clock */    CLK->APBCLK0 |= CLK_APBCLK0_UART0CKEN_Msk;        /* Select IP clock source */    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART0SEL_Msk)) | CLK_CLKSEL1_UART0SEL_HIRC;        /* Set multi-function pins for UART0 RXD and TXD */    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~(UART0_RXD_PB12_Msk | UART0_TXD_PB13_Msk))) | UART0_RXD_PB12 | UART0_TXD_PB13;
    
#endif


  
}
