/* mbed Microcontroller Library
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include "i2c_device.h"
#include "mbed_assert.h"
#include "mbed_error.h"
#include "stm32h7xx_ll_rcc.h"

/*  Define I2C Device */
#if DEVICE_I2C

/**
  * @brief  Get I2C clock source frequency according I2C instance used.
  * @param  i2c I2C instance name.
  * @retval I2C clock source frequency in Hz.
  */
uint32_t i2c_get_pclk(I2CName i2c)
{
    uint32_t clocksource;
    uint32_t pclk = 0;
    PLL3_ClocksTypeDef pll3_clocks;
    if ((i2c == I2C_1) || (i2c == I2C_2) || (i2c == I2C_3)) {
        clocksource = __HAL_RCC_GET_I2C123_SOURCE();
        switch (clocksource) {
            case RCC_I2C123CLKSOURCE_D2PCLK1:
                pclk = HAL_RCC_GetPCLK1Freq();
                break;
            case RCC_I2C123CLKSOURCE_PLL3:
                HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
                pclk = pll3_clocks.PLL3_R_Frequency;
                break;
            case RCC_I2C123CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
            case RCC_I2C123CLKSOURCE_CSI:
                pclk = CSI_VALUE;
                break;
            default:
                // should not happend
                error("I2C123: Invalid clock source");
                break;
        }
    }
#if defined I2C4_BASE
    else if (i2c == I2C_4) {
        clocksource = __HAL_RCC_GET_I2C4_SOURCE();
        switch (clocksource) {
            case RCC_I2C4CLKSOURCE_D3PCLK1:
                pclk = HAL_RCCEx_GetD3PCLK1Freq();
                break;
            case RCC_I2C4CLKSOURCE_PLL3:
                HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
                pclk = pll3_clocks.PLL3_R_Frequency;
                break;
            case RCC_I2C4CLKSOURCE_HSI:
                pclk = HSI_VALUE;
                break;
            case RCC_I2C4CLKSOURCE_CSI:
                pclk = CSI_VALUE;
                break;
            default:
                // should not happend
                error("I2C4: Invalid clock source");
                break;
        }
    }
#endif
    else {
        // should not happend
        error("I2C: unknown instance");
    }
    return pclk;
}
/**
  * @}
  */

/** @defgroup I2C_DEVICE_Exported_Functions I2C_DEVICE Exported Functions
  * @{
  */
/**
 * @brief  Provide the suitable timing depending on requested frequency
 * @param  hz Required I2C clock in Hz.
 * @retval I2C timing or 0 in case of error.
 */
uint32_t i2c_get_timing(I2CName i2c, int hz)
{
    uint32_t tim;
    uint32_t pclk;
    pclk = i2c_get_pclk(i2c);
    if (pclk == I2C_PCLK_DEF) {
        switch (hz) {
            case 100000:
                tim = TIMING_VAL_DEFAULT_CLK_100KHZ;
                break;
            case 400000:
                tim = TIMING_VAL_DEFAULT_CLK_400KHZ;
                break;
            case 1000000:
                tim = TIMING_VAL_DEFAULT_CLK_1MHZ;
                break;
            default:
                MBED_ASSERT((hz == 100000) || (hz == 400000) || (hz == 1000000));
                break;
        }
    } else {
        /* If MBED_CONF_TARGET_I2C_TIMING_VALUE_ALGO assert is triggered. 
        User needs to enable I2C_TIMING_VALUE_ALGO in target.json for specific target. 
        Enabling this may impact performance*/
        MBED_ASSERT(MBED_CONF_TARGET_I2C_TIMING_VALUE_ALGO);
#if MBED_CONF_TARGET_I2C_TIMING_VALUE_ALGO
        tim = i2c_compute_timing(pclk, hz);
#endif
    }
    return tim;
}
/**
  * @}
  */

#endif // DEVICE_I2C
