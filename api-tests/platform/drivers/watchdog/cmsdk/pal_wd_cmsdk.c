/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "pal_wd_cmsdk.h"

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_cmsdk_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
#if 0 // M2351
    /* Disable Timer */
    ((wd_timer_t *)base_addr)->CTRL = 0x0;

    if (time_us == 0)
    {
        ((wd_timer_t *)base_addr)->LOAD = 0;
    }
    else
    {
        ((wd_timer_t *)base_addr)->LOAD = time_us * timer_tick_us;
    }
#endif
    return 0;
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_cmsdk_enable(addr_t base_addr)
{
#if 0 // M2351
    /* Enable counter by enabling intr and reset */
     ((wd_timer_t *)base_addr)->CTRL = (Watchdog_CTRL_INTEN_Msk | Watchdog_CTRL_RESEN_Msk);
#endif

    return 0;
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_cmsdk_disable(addr_t base_addr)
{
#if 0 // M2351
    /* Disable Timer */
    ((wd_timer_t *)base_addr)->CTRL   = 0x0;
    ((wd_timer_t *)base_addr)->INTCLR = 0x1;
#endif
    return 0;
}

/**
    @brief           - Checks whether hardware watchdog timer is enabled
    @param           - base_addr       : Base address of the watchdog module
    @return          - Enabled : 1, Disabled : 0
**/
int pal_wd_cmsdk_is_enabled(addr_t base_addr)
{
#if 1 // M2351
    return 1;
#else
    return (((wd_timer_t *)base_addr)->CTRL & Watchdog_CTRL_INTEN_Msk ? \
    (((wd_timer_t *)base_addr)->CTRL & Watchdog_CTRL_RESEN_Msk ? 1 : 0) : 0);
#endif
}

