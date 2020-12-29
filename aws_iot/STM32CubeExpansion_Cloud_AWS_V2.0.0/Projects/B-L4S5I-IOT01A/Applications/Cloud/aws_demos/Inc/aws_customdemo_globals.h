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

#ifndef APPLICATION_CODE_APP_AWS_CUSTOMDEMO_GLOBALS_H_
#define APPLICATION_CODE_APP_AWS_CUSTOMDEMO_GLOBALS_H_


#ifdef DEFINE_GLOBAL_ONCE


unsigned char gucSensorTopicName[]="dt/stm32l475e/sensor-data/topic/1";
QueueHandle_t xSensorDataQueue = NULL;
const char *pSensorUid="st-stm32l475e-sensor-01";


#else

extern IotMqttConnection_t gpmqttConnection;
extern unsigned char gucSensorTopicName[];
extern  QueueHandle_t xSensorDataQueue;

#endif


#endif /* APPLICATION_CODE_APP_AWS_CUSTOMDEMO_GLOBALS_H_ */