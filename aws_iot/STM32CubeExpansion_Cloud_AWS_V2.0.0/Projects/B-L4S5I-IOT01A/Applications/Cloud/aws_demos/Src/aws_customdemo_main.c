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

/* Standard includes. */
#include <aws_customdemo_runner.h>
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

#define DEFINE_GLOBAL_ONCE
#include "aws_customdemo_globals.h"

#include "st_sensordata_collector.h"

#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
extern void _sensorDataPublisherTask();
extern void onboardSensorReaderTask();
extern int aws_custom_mqttlib_initialise(
        bool awsIotMqttMode, const char * pIdentifier,
        void * pNetworkServerInfo, void * pNetworkCredentialInfo,
        const IotNetworkInterface_t * pNetworkInterface);

void init_telemetry_pub_queue();

/*
* Main function for the custom demo runner
*/
int aws_customdemo_main(bool awsIotMqttMode, const char * pIdentifier,
                 void * pNetworkServerInfo, void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface)
{
    int status = EXIT_FAILURE;

    IotLogInfo("ENTRY: aws_custom_demo_main with awsIotMqttMode: %d\r\n", awsIotMqttMode);

    status = aws_custom_mqttlib_initialise(awsIotMqttMode, pIdentifier,
                                           pNetworkServerInfo,
                                           pNetworkCredentialInfo,
                                           pNetworkInterface);

    if (status == EXIT_SUCCESS)
    {
        IotLogInfo("AWS Custom Demo Initialization is Successful");
        //Start the sensor polling Task here. In production code, these two must be done asynchronously.
        // As the device logic should function independent of network connectivity
        // To simplify the demo, these are done sequentially currently

        init_telemetry_pub_queue();

        Iot_CreateDetachedThread(_sensorDataPublisherTask, NULL,
        democonfigDEMO_PRIORITY,
                                 democonfigDEMO_STACKSIZE);

        Iot_CreateDetachedThread(onboardSensorReaderTask, NULL,
        democonfigDEMO_PRIORITY,
                                 democonfigDEMO_STACKSIZE);

       while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(3000));

        }

    }
    else
    {
        IotLogInfo("AWS Custom Demo Initialization Failed");
    }

    return EXIT_SUCCESS;
}

/*
 * Telemetry Data publisher queue initialization
 */
void init_telemetry_pub_queue()
{

    if (xSensorDataQueue == NULL)
    {
        /* Create the queue used to pass pointers to strings to the sensor data publishing task. */
        xSensorDataQueue = xQueueCreate(mainLOGGING_MESSAGE_QUEUE_LENGTH,
                                        sizeof(struct sensorDataMsg *));

        if (xSensorDataQueue != NULL)
        {
            IotLogInfo("xSensor Data Queue is created ");

        }
        else
        {

            IotLogError(" Unable to create xSensor Data Queue");

        }
    }
}
