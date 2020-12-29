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

#include <aws_customdemo_inc.h>
#include "st_sensordata_collector.h"


/*
 * This task publishes the sensor data via mqtt to the AWS IoT core service
 *
 */
void _sensorDataPublisherTask()
{

    struct sensorDataMsg *pcRxedMsg;
    int publishStatus=0;

    IotLogInfo("ENTRY : _sensorDataPublisherTask ");


    for (;;)
    {
        /* Block to wait to receive the data. */
        if (xQueueReceive(xSensorDataQueue, &(pcRxedMsg),
                          portMAX_DELAY) == pdPASS)
        {

            struct sensorDataMsg    *pRxMsg = pcRxedMsg;


            IotLogInfo ("\nReceived Message ,:[ %s ] of length [ %d]\n", pRxMsg->pSensorMsg,pRxMsg->iMsgLen);
            //The Topic Name is hard-coded, message length is hard coded for now
            publishStatus = _publishSensorData(gucSensorTopicName, pRxMsg->pSensorMsg,pRxMsg->iMsgLen);

            IotLogInfo("Publish Status: %d\n", publishStatus);
            //Free the Message buffer
            vPortFree(pRxMsg->pSensorMsg);
            vPortFree(pRxMsg);

        }

    }

}


void sendToTelemetryQueue(unsigned char *pTopicName, char *pMsg, int msglen)
{
    struct sensorDataMsg *psensorData;

    IotLogInfo("ENTRY: sendToTelemetryQueue:");

    psensorData = pvPortMalloc(sizeof(struct sensorDataMsg));
    psensorData->pSensorMsg = pMsg;
    psensorData->iMsgLen = msglen;

    IotLogInfo("Sending  data as json string to Telemetry Queue:[ %s ]\n",  psensorData->pSensorMsg );

    xQueueSend(xSensorDataQueue, (void * ) &psensorData, (TickType_t ) 0);

    IotLogInfo("EXIT: sendToTelemetryQueue: [ %s ]\n",  psensorData->pSensorMsg );

}

