/* MIT License

Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef APPLICATION_CODE_APP_AWS_CUSTOMDEMO_INC_H_
#define APPLICATION_CODE_APP_AWS_CUSTOMDEMO_INC_H_

#include <stdint.h>
#include <stdio.h>
#include "iot_config.h"

/* Includes for library initialization. */
#include "iot_demo_runner.h"
#include "platform/iot_threads.h"
#include "types/iot_network_types.h"
#include "aws_demo.h"
#include "aws_demo_config.h"
/* MQTT include. */
#include "iot_mqtt.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "task.h"
/* The config header is always included first. */
#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL    IOT_LOG_DEBUG
#endif
#define LIBRARY_LOG_NAME         ( "ST-L475E-MAIN" )
#include "iot_logging_setup.h"
/* Shadow include. */
#include <string.h>
#include "aws_iot_shadow.h"

#include "aws_customdemo_globals.h"
#include <aws_customdemo_runner.h>

#endif /* APPLICATION_CODE_APP_AWS_CUSTOMDEMO_INC_H_ */