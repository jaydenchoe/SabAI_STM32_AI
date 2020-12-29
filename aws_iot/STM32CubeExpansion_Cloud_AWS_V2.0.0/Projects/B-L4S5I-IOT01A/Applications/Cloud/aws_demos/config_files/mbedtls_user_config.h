/**
  ******************************************************************************
  * @file    mbedtls_user_config.h
  * @author  MCD Application Team
  * @brief   mbedtls configuration user overlay
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef MBEDTLS_USER_CONFIG_H
#define MBEDTLS_USER_CONFIG_H

/* OTA */
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED

/* STSAFE PoC */
#define MBEDTLS_PEM_WRITE_C
#endif /* MBEDTLS_USER_CONFIG_H */
