/**
  ******************************************************************************
  * @file    stsafe_service_interface.c
  * @author  SMD/AME application teams
  * @date    17-february-2020
  * @brief   Service Interface file to support the hardware services required by the
  *          STSAFE-A Middleware and offered by the specific HW, Low Level library, or BSP
  *          selected by the user. E.g.:
  *           + IOs
  *           + Communication Bus (e.g. I2C)
  *           + Timing delay
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * STSAFE DRIVER SOFTWARE LICENSE AGREEMENT (SLA0088)
  *
  * BY INSTALLING, COPYING, DOWNLOADING, ACCESSING OR OTHERWISE USING THIS SOFTWARE
  * OR ANY PART THEREOF (AND THE RELATED DOCUMENTATION) FROM STMICROELECTRONICS
  * INTERNATIONAL N.V, SWISS BRANCH AND/OR ITS AFFILIATED COMPANIES (STMICROELECTRONICS),
  * THE RECIPIENT, ON BEHALF OF HIMSELF OR HERSELF, OR ON BEHALF OF ANY ENTITY BY WHICH
  * SUCH RECIPIENT IS EMPLOYED AND/OR ENGAGED AGREES TO BE BOUND BY THIS SOFTWARE LICENSE
  * AGREEMENT.
  *
  * Under STMicroelectronics’ intellectual property rights, the redistribution,
  * reproduction and use in source and binary forms of the software or any part thereof,
  * with or without modification, are permitted provided that the following conditions
  * are met:
  * 1.  Redistribution of source code (modified or not) must retain any copyright notice,
  *     this list of conditions and the disclaimer set forth below as items 10 and 11.
  * 2.  Redistributions in binary form, except as embedded into a microcontroller or
  *     microprocessor device or a software update for such device, must reproduce any
  *     copyright notice provided with the binary code, this list of conditions, and the
  *     disclaimer set forth below as items 10 and 11, in documentation and/or other
  *     materials provided with the distribution.
  * 3.  Neither the name of STMicroelectronics nor the names of other contributors to this
  *     software may be used to endorse or promote products derived from this software or
  *     part thereof without specific written permission.
  * 4.  This software or any part thereof, including modifications and/or derivative works
  *     of this software, must be used and execute solely and exclusively in combination
  *     with a secure microcontroller device from STSAFE family manufactured by or for
  *     STMicroelectronics.
  * 5.  No use, reproduction or redistribution of this software partially or totally may be
  *     done in any manner that would subject this software to any Open Source Terms.
  *     “Open Source Terms” shall mean any open source license which requires as part of
  *     distribution of software that the source code of such software is distributed
  *     therewith or otherwise made available, or open source license that substantially
  *     complies with the Open Source definition specified at www.opensource.org and any
  *     other comparable open source license such as for example GNU General Public
  *     License(GPL), Eclipse Public License (EPL), Apache Software License, BSD license
  *     or MIT license.
  * 6.  STMicroelectronics has no obligation to provide any maintenance, support or
  *     updates for the software.
  * 7.  The software is and will remain the exclusive property of STMicroelectronics and
  *     its licensors. The recipient will not take any action that jeopardizes
  *     STMicroelectronics and its licensors' proprietary rights or acquire any rights
  *     in the software, except the limited rights specified hereunder.
  * 8.  The recipient shall comply with all applicable laws and regulations affecting the
  *     use of the software or any part thereof including any applicable export control
  *     law or regulation.
  * 9.  Redistribution and use of this software or any part thereof other than as  permitted
  *     under this license is void and will automatically terminate your rights under this
  *     license.
  * 10. THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" AND ANY
  *     EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  *     WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
  *     OF THIRD PARTY INTELLECTUAL PROPERTY RIGHTS, WHICH ARE DISCLAIMED TO THE FULLEST
  *     EXTENT PERMITTED BY LAW. IN NO EVENT SHALL STMICROELECTRONICS OR CONTRIBUTORS BE
  *     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  *     DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  *     THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  *     NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  *     ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  * 11. EXCEPT AS EXPRESSLY PERMITTED HEREUNDER, NO LICENSE OR OTHER RIGHTS, WHETHER EXPRESS
  *     OR IMPLIED, ARE GRANTED UNDER ANY PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF
  *     STMICROELECTRONICS OR ANY THIRD PARTY.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stsafea_service.h"
#include "stsafea_interface_conf.h"
#include MCU_PLATFORM_INCLUDE

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define STSAFEA_VREG_PIN                          GPIO_PIN_7
#define STSAFEA_VREG_GPIO_PORT                    GPIOD
#define STSAFEA_VREG_GPIO_PORT_CLK_ENABLE         __HAL_RCC_GPIOD_CLK_ENABLE
#define STSAFEA_DEVICE_ADDRESS                    0x0020

#define I2C_ANALOG_FILTER_DELAY_DEFAULT        2U      /* ns */

#define BUS_I2C2_INSTANCE                       I2C2

#define BUS_I2C2_CLK_ENABLE()                   __HAL_RCC_I2C2_CLK_ENABLE()
#define BUS_I2C2_CLK_DISABLE()                  __HAL_RCC_I2C2_CLK_DISABLE()

#define BUS_I2C2_SDA_GPIO_PIN                   GPIO_PIN_11
#define BUS_I2C2_SCL_GPIO_PIN                   GPIO_PIN_10

#define BUS_I2C2_SDA_GPIO_PORT                  GPIOB
#define BUS_I2C2_SCL_GPIO_PORT                  GPIOB

#define BUS_I2C2_SDA_GPIO_AF                    GPIO_AF4_I2C2
#define BUS_I2C2_SCL_GPIO_AF                    GPIO_AF4_I2C2

#define BUS_I2C2_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C2_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

#define BUS_I2C2_POLL_TIMEOUT                0x1000U

#define BUS_I2C2_TIMING                      0x00702991 /* Corresponding to frequency of I2C1 = 400 KHz*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef  hbus_i2c2;
/* Private function prototypes -----------------------------------------------*/
int32_t HW_IO_Init(void);
void    BSP_TimeDelay(uint32_t msDelay);
HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *phi2c, uint32_t timing);
int32_t BSP_I2C2_Init(void);
int32_t BSP_I2C2_DeInit(void);
int32_t BSP_I2C2_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t CRC16X25_Init(void);
uint32_t CRC_Compute(uint8_t *pData1, uint16_t Length1, uint8_t *pData2, uint16_t Length2);
static void I2C2_MspInit(I2C_HandleTypeDef *hI2c);
static void I2C2_MspDeInit(I2C_HandleTypeDef *hI2c);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Configure STSAFE IO and Bus operation functions to be implemented at User level
  * @param  Ctx the STSAFE IO context
  * @retval 0 in case of success, an error code otherwise
  */
int8_t StSafeA_HW_Probe(void *pCtx)
{
  STSAFEA_HW_t *HwCtx = (STSAFEA_HW_t *)pCtx;


  HwCtx->IOInit     = HW_IO_Init;
  HwCtx->BusInit    = BSP_I2C2_Init;
  HwCtx->BusDeInit  = BSP_I2C2_DeInit;
  HwCtx->BusSend    = BSP_I2C2_Send;
  HwCtx->BusRecv    = BSP_I2C2_Recv;
  HwCtx->CrcInit    = CRC16X25_Init;
  HwCtx->CrcCompute = CRC_Compute;
  HwCtx->TimeDelay  = HAL_Delay;
  HwCtx->DevAddr    = STSAFEA_DEVICE_ADDRESS;

  return STSAFEA_HW_OK;
}


/**
  * @brief  Additional IO pins configuration needed for STSAFE (VREG pin, etc.)
  * @param  none
  * @retval 0 in case of success, an error code otherwise
  */
int32_t HW_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  STSAFEA_VREG_GPIO_PORT_CLK_ENABLE();

  /* Configure GPIO pin : RST Pin */
  GPIO_InitStruct.Pin = STSAFEA_VREG_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STSAFEA_VREG_GPIO_PORT, &GPIO_InitStruct);

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STSAFEA_VREG_GPIO_PORT, STSAFEA_VREG_PIN, GPIO_PIN_SET);
  HAL_Delay(50);

  return STSAFEA_HW_OK;
}


/**
  * @brief  This function provides a delay (in milliseconds)
  * @param  none
  * @retval 0 in case of success, an error code otherwise
  */
void BSP_TimeDelay(uint32_t msDelay)
{
  /* Could be redirected to a Task Delay or to a different custom implementation */
  HAL_Delay(msDelay);
}

/**
  * @brief  MX I2C2 Inititialization.
  * @param  phi2c : I2C handle.
  * @param  timing : I2C timings as described in the I2C peripheral V2 and V3.
  * @retval Prescaler dividor
  */
__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *phi2c, uint32_t timing)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  phi2c->Init.Timing           = timing;
  phi2c->Init.OwnAddress1      = 0;
  phi2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  phi2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  phi2c->Init.OwnAddress2      = 0;
  phi2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  phi2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(phi2c) == HAL_OK)
  {
    if (HAL_I2CEx_ConfigAnalogFilter(phi2c, I2C_ANALOGFILTER_DISABLE) == HAL_OK)
    {
      if (HAL_I2CEx_ConfigDigitalFilter(phi2c, I2C_ANALOG_FILTER_DELAY_DEFAULT) == HAL_OK)
      {
        ret = HAL_OK;
      }
    }
  }

  return ret;
}

/**
  * @brief  Initializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C2_Init(void)
{
  int32_t ret = STSAFEA_HW_ERR;

  hbus_i2c2.Instance  = I2C2;

  if (HAL_I2C_GetState(&hbus_i2c2) == HAL_I2C_STATE_RESET)
  {
    /* Init the I2C Msp */
    I2C2_MspInit(&hbus_i2c2);

    /* Init the I2C */
    if (MX_I2C2_Init(&hbus_i2c2, BUS_I2C2_TIMING) == HAL_OK)
    {
      if (HAL_I2CEx_ConfigAnalogFilter(&hbus_i2c2, I2C_ANALOGFILTER_ENABLE) == HAL_OK)
      {
        ret = STSAFEA_HW_OK;
      }
    }
  }
  else
  {
    ret = STSAFEA_HW_OK;
  }

  return ret;
}

/**
  * @brief  DeInitializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C2_DeInit(void)
{
  int32_t ret  = STSAFEA_HW_ERR;

  /* DeInit the I2C */
  I2C2_MspDeInit(&hbus_i2c2);

  /* DeInit the I2C */
  if (HAL_I2C_DeInit(&hbus_i2c2) == HAL_OK)
  {
    ret = STSAFEA_HW_OK;
  }

  return ret;
}


/**
  * @brief  Send data through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  pData  Pointer to data buffer to write
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_I2C2_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = STSAFEA_HW_ERR;

  if (HAL_I2C_Master_Transmit(&hbus_i2c2,
                              DevAddr,
                              pData,
                              Length,
                              BUS_I2C2_POLL_TIMEOUT) == HAL_OK)
  {
    ret = STSAFEA_HW_OK;
  }

  return ret;
}

/**
  * @brief  Send data through BUS
  * @param  DevAddr Device address on Bus.
  * @param  pData  Pointer to data buffer to read
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t  BSP_I2C2_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = STSAFEA_HW_ERR;

  if (HAL_I2C_Master_Receive(&hbus_i2c2,
                             DevAddr,
                             pData,
                             Length,
                             BUS_I2C2_POLL_TIMEOUT) == HAL_OK)
  {
    ret = STSAFEA_HW_OK;
  }

  return ret;
}

/**
  * @brief  Initializes I2C MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C2_MspInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init;

  /* Enable I2C clock */
  BUS_I2C2_CLK_ENABLE();

  /* Enable GPIO clock */
  BUS_I2C2_SDA_GPIO_CLK_ENABLE();
  BUS_I2C2_SCL_GPIO_CLK_ENABLE();

  /* Configure I2C SDA Line */
  gpio_init.Pin = BUS_I2C2_SDA_GPIO_PIN;
  gpio_init.Mode = GPIO_MODE_AF_OD;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = BUS_I2C2_SDA_GPIO_AF;
  HAL_GPIO_Init(BUS_I2C2_SDA_GPIO_PORT, &gpio_init);

  /* Configure I2C SCL Line */
  gpio_init.Pin = BUS_I2C2_SCL_GPIO_PIN;
  gpio_init.Mode = GPIO_MODE_AF_OD;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = BUS_I2C2_SCL_GPIO_AF;
  HAL_GPIO_Init(BUS_I2C2_SCL_GPIO_PORT, &gpio_init);

}

/**
  * @brief  DeInitializes I2C MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C2_MspDeInit(I2C_HandleTypeDef *hI2c)
{
  /* Disable I2C clock */
  __HAL_RCC_I2C2_CLK_DISABLE();

  /* DeInitialize peripheral GPIOs */
  HAL_GPIO_DeInit(BUS_I2C2_SDA_GPIO_PORT, BUS_I2C2_SDA_GPIO_PIN);
  HAL_GPIO_DeInit(BUS_I2C2_SCL_GPIO_PORT, BUS_I2C2_SCL_GPIO_PIN);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
