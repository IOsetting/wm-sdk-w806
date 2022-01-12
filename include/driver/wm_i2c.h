// Copyright 2021 IOsetting <iosetting(at)outlook.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __WM_I2C_H__
#define __WM_I2C_H__

#include "wm_hal.h"

typedef struct __I2C_HandleTypeDef
{
    I2C_TypeDef                	*Instance;		/*!< I2C registers base address               */
	uint32_t 					Frequency;
	HAL_LockTypeDef            	Lock;           /*!< Object lock                              */
	__IO uint32_t               ErrorCode;      /*!< I2C Error code                           */
} I2C_HandleTypeDef;

#define I2C                             ((I2C_TypeDef *)I2C_BASE)

#ifdef __cplusplus
extern "C"{
#endif

HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef HAL_I2C_Write(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Read(I2C_HandleTypeDef *hi2c, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size);

#ifdef __cplusplus
}
#endif

#endif