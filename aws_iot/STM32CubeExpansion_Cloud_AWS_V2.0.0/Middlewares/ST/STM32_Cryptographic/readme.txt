/**
  @page STM32_Cryptographic wrapper.

  @verbatim
  ******************** (C) COPYRIGHT 2020 STMicroelectronics *******************
  * @file    readme.txt
  * @brief   STM32_Cryptographic wrapper.
  *          STM32_Cryptographic is a wrapper redirecting the cryptographic
  *          operations to the mbed-crypto middleware instead of
  *          the STM32 cryptographic library.
  *
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Middleware Description
   This middleware provides a wrapper used to perform cryptographic operations.
   Each call to a STM32 Crypto Library API is redirected to the equivalent mbed-crypto API.

@par Directory contents

   - Fw_Crypto/Wrapper_Crypto/wrap_aes.c              AES operations.
   - Fw_Crypto/Wrapper_Crypto/wrap_digest.c           Hash operations.
   - Fw_Crypto/Wrapper_Crypto/wrap_sign_verify.c      ECDSA and RSA operations.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
