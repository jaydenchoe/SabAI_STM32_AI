/*
 * FreeRTOS V1.1.4
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef AWS_TEST_RUNNER_CONFIG_H
#define AWS_TEST_RUNNER_CONFIG_H

/* Uncomment this line if you want to run DQP_FR tests only. */
/* #define testrunnerAFQP_ENABLED */

#define testrunnerUNSUPPORTED                      0

/* Enable tests by setting defines to 1 */
#define testrunnerFULL_CRYPTO_ENABLED              1    /* Requires 2.7kbytes more headroom on TestRunner stack */
#define testrunnerFULL_DEFENDER_ENABLED            0
#define testrunnerFULL_GGD_ENABLED                 0
#define testrunnerFULL_GGD_HELPER_ENABLED          0
#define testrunnerFULL_HTTPS_CLIENT_ENABLED        0
#define testrunnerFULL_MEMORYLEAK_ENABLED          0    /* Do not enable when running the qualification tests as it catches memory leaks in the tests implementation. */
#define testrunnerFULL_MQTT_AGENT_ENABLED          1
#define testrunnerFULL_MQTT_ALPN_ENABLED           testrunnerUNSUPPORTED  /* Only Infineon implements it */
#define testrunnerFULL_MQTT_STRESS_TEST_ENABLED    1
#define testrunnerFULL_MQTTv4_ENABLED              1
#define testrunnerFULL_OTA_AGENT_ENABLED           0    /* Disabled by default because it fails if run after TCP tests. To be run separately. */
#define testrunnerFULL_OTA_CBOR_ENABLED            testrunnerUNSUPPORTED  /* Windows dependency */
#define testrunnerFULL_OTA_PAL_ENABLED             1
#define testrunnerFULL_PKCS11_ENABLED              1
#define testrunnerUTIL_PLATFORM_CLOCK_ENABLED      0    /* Not in QP */
#define testrunnerUTIL_PLATFORM_THREADS_ENABLED    0    /* Not in QP */
#define testrunnerFULL_POSIX_ENABLED               0    /* Not in QP */
#define testrunnerFULL_SERIALIZER_ENABLED          0    /* Not in QP */
#define testrunnerFULL_SHADOW_ENABLED              0
#define testrunnerFULL_SHADOWv4_ENABLED            0    /* Not in QP */
#define testrunnerFULL_TASKPOOL_ENABLED            0    /* Not in QP */
#define testrunnerFULL_TCP_ENABLED                 1
#define testrunnerFULL_TLS_ENABLED                 1
#define testrunnerFULL_WIFI_ENABLED                0    /* Excluded by default because long to run */

/* Tests which have never been built for this board */
#define testrunnerFULL_BLE_END_TO_END_TEST_ENABLED testrunnerUNSUPPORTED  /* Not on this board */
#define testrunnerFULL_WIFI_PROVISIONING_ENABLED   testrunnerUNSUPPORTED  /* Not on this board - depends on BLE */

#endif /* AWS_TEST_RUNNER_CONFIG_H */
