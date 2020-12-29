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


/* Forward declaration of network connected DEMO callback to be renamed from #define in aws_demo_config.h */
#include <aws_customdemo_inc.h>
#include "platform/iot_clock.h"


#ifndef IOT_DEMO_MQTT_TOPIC_PREFIX
#define IOT_DEMO_MQTT_TOPIC_PREFIX           "iotdemo"
#endif

/**
 * @brief The first characters in the client identifier. A timestamp is appended
 * to this prefix to create a unique client identifer.
 *
 * This prefix is also used to generate topic names and topic filters used in this
 * demo.
 */
#define CLIENT_IDENTIFIER_PREFIX                 "iotdemo"

/**
 * @brief The longest client identifier that an MQTT server must accept (as defined
 * by the MQTT 3.1.1 spec) is 23 characters. Add 1 to include the length of the NULL
 * terminator.
 */
#define CLIENT_IDENTIFIER_MAX_LENGTH             ( 24 )

/**
 * @brief The keep-alive interval used for this demo.
 *
 * An MQTT ping request will be sent periodically at this interval.
 */
#define KEEP_ALIVE_SECONDS                       ( 60 )

/**
 * @brief The timeout for MQTT operations in this demo.
 */
#define MQTT_TIMEOUT_MS                          ( 5000 )

/**
 * @brief The Last Will and Testament topic name in this demo.
 *
 * The MQTT server will publish a message to this topic name if this client is
 * unexpectedly disconnected.
 */
#define WILL_TOPIC_NAME                          IOT_DEMO_MQTT_TOPIC_PREFIX "/will"

/**
 * @brief The length of #WILL_TOPIC_NAME.
 */
#define WILL_TOPIC_NAME_LENGTH                   ( ( uint16_t ) ( sizeof( WILL_TOPIC_NAME ) - 1 ) )

/**
 * @brief The message to publish to #WILL_TOPIC_NAME.
 */
#define WILL_MESSAGE                             "MQTT demo unexpectedly disconnected."

/**
 * @brief The length of #WILL_MESSAGE.
 */
#define WILL_MESSAGE_LENGTH                      ( ( size_t ) ( sizeof( WILL_MESSAGE ) - 1 ) )

/**
 * @brief How many topic filters will be used in this demo.
 */
#define TOPIC_FILTER_COUNT                       ( 4 )
#define TEMP_TOPIC_FILTER_COUNT                       ( 1 )
/**
 * @brief The length of each topic filter.
 *
 * For convenience, all topic filters are the same length.
 */
#define TOPIC_FILTER_LENGTH                      ( ( uint16_t ) ( sizeof( IOT_DEMO_MQTT_TOPIC_PREFIX "/topic/1" ) - 1 ) )
#define TEMP_TOPIC_FILTER_LENGTH                      ( ( uint16_t ) ( sizeof( IOT_DEMO_MQTT_TEMPTOPIC_PREFIX) - 1 ) )
/**
 * @brief Format string of the PUBLISH messages in this demo.
 */
#define PUBLISH_PAYLOAD_FORMAT                   "Hello world %d!"
/**
 * @brief The topic name on which acknowledgement messages for incoming publishes
 * should be published.
 */
#define ACKNOWLEDGEMENT_TOPIC_NAME               IOT_DEMO_MQTT_TOPIC_PREFIX "/acknowledgements"

/**
 * @brief The length of #ACKNOWLEDGEMENT_TOPIC_NAME.
 */
#define ACKNOWLEDGEMENT_TOPIC_NAME_LENGTH        ( ( uint16_t ) ( sizeof( ACKNOWLEDGEMENT_TOPIC_NAME ) - 1 ) )

/**
 * @brief Format string of PUBLISH acknowledgement messages in this demo.
 */
#define ACKNOWLEDGEMENT_MESSAGE_FORMAT           "Client has received PUBLISH %.*s from server."
/**
 * @brief The maximum number of times each PUBLISH in this demo will be retried.
 */
#define PUBLISH_RETRY_LIMIT                      ( 10 )

/**
 * @brief A PUBLISH message is retried if no response is received within this
 * time.
 */
#define PUBLISH_RETRY_MS                         ( 1000 )

/**
 * @brief Size of the buffers that hold acknowledgement messages in this demo.
 */
#define ACKNOWLEDGEMENT_MESSAGE_BUFFER_LENGTH    ( sizeof( ACKNOWLEDGEMENT_MESSAGE_FORMAT ) + 2 )

extern int init_shadow_callbacks(IotMqttConnection_t * pMqttConnection, const char * pIdentifier);

int aws_custom_mqttlib_initialise(
        bool awsIotMqttMode, const char * pIdentifier,
        void * pNetworkServerInfo, void * pNetworkCredentialInfo,
        const IotNetworkInterface_t * pNetworkInterface);

void aws_custom_demo_networkConnectedCallback(
        bool awsIotMqttMode, const char * pIdentifier,
        void * pNetworkServerInfo, void * pNetworkCredentialInfo,
        const IotNetworkInterface_t * pNetworkInterface);

/* Forward declaration of network disconnected DEMO callback to be renamed from #define in aws_demo_config.h */
void aws_custom_demo_networkDisconnectedCallback(
        const IotNetworkInterface_t * pNetworkInterface);

int aws_customdemo_main(bool awsIotMqttMode, const char * pIdentifier,
                 void * pNetworkServerInfo, void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface);
SemaphoreHandle_t xSemaphore;

IotMqttConnection_t gpmqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;


int aws_custom_demo_init(demoContext_t *pmqttDemoContext)
{

    IotLogInfo(("ENTRY: aws_custom_demo_init\r\n"));

   bool status =  Iot_CreateDetachedThread(runDemoTask, pmqttDemoContext,
    democonfigDEMO_PRIORITY,
                             democonfigDEMO_STACKSIZE);

   return status;

}

/*
 * This is a custom demo runner. Initialises the custom demo runner which overrides the
 * aws standard demos
 */

void aws_custom_demo_run(void)
{

    configPRINTF(( "ENTRY: aws_custom_demo_run\r\n" ));

    static demoContext_t mqttDemoContext = {
            .networkTypes = democonfigNETWORK_TYPES, .demoFunction =
                    aws_customdemo_main,
            .networkConnectedCallback = aws_custom_demo_networkConnectedCallback,
            .networkDisconnectedCallback = aws_custom_demo_networkDisconnectedCallback };

    aws_custom_demo_init(&mqttDemoContext);

}



void aws_custom_demo_networkConnectedCallback(
        bool awsIotMqttMode, const char * pIdentifier,
        void * pNetworkServerInfo, void * pNetworkCredentialInfo,
        const IotNetworkInterface_t * pNetworkInterface)
{

    IotLogInfo(("ENTRY: aws_custom_demo_networkConnectedCallback\r\n"));

}

void aws_custom_demo_networkDisconnectedCallback(
        const IotNetworkInterface_t * pNetworkInteface)
{

    IotLogInfo(("ENTRY: aws_custom_demo_networkDisconnectedCallback\r\n"));

}

/**
 * @brief Establish a new connection to the MQTT server.
 *
 * @param[in] awsIotMqttMode Specify if this demo is running with the AWS IoT
 * MQTT server. Set this to `false` if using another MQTT server.
 * @param[in] pIdentifier NULL-terminated MQTT client identifier.
 * @param[in] pNetworkServerInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkCredentialInfo Passed to the MQTT connect function when
 * establishing the MQTT connection.
 * @param[in] pNetworkInterface The network interface to use for this demo.
 * @param[out] pMqttConnection Set to the handle to the new MQTT connection.
 *
 * @return `EXIT_SUCCESS` if the connection is successfully established; `EXIT_FAILURE`
 * otherwise.
 */
static int _establishMqttConnection(
        bool awsIotMqttMode, const char * pIdentifier,
        void * pNetworkServerInfo, void * pNetworkCredentialInfo,
        const IotNetworkInterface_t * pNetworkInterface,
        IotMqttConnection_t * pMqttConnection)
{
    int status = EXIT_SUCCESS;
    IotMqttError_t connectStatus = IOT_MQTT_STATUS_PENDING;
    IotMqttNetworkInfo_t networkInfo = IOT_MQTT_NETWORK_INFO_INITIALIZER;
    IotMqttConnectInfo_t connectInfo = IOT_MQTT_CONNECT_INFO_INITIALIZER;
    IotMqttPublishInfo_t willInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    char pClientIdentifierBuffer[CLIENT_IDENTIFIER_MAX_LENGTH] = { 0 };

    /* Set the members of the network info not set by the initializer. This
     * struct provided information on the transport layer to the MQTT connection. */
    networkInfo.createNetworkConnection = true;
    networkInfo.u.setup.pNetworkServerInfo = pNetworkServerInfo;
    networkInfo.u.setup.pNetworkCredentialInfo = pNetworkCredentialInfo;
    networkInfo.pNetworkInterface = pNetworkInterface;

#if ( IOT_MQTT_ENABLE_SERIALIZER_OVERRIDES == 1 ) && defined( IOT_DEMO_MQTT_SERIALIZER )
    networkInfo.pMqttSerializer = IOT_DEMO_MQTT_SERIALIZER;
#endif

    /* Set the members of the connection info not set by the initializer. */
    connectInfo.awsIotMqttMode = awsIotMqttMode;
    connectInfo.cleanSession = true;
    connectInfo.keepAliveSeconds = KEEP_ALIVE_SECONDS;
    connectInfo.pWillInfo = &willInfo;

    /* Set the members of the Last Will and Testament (LWT) message info. The
     * MQTT server will publish the LWT message if this client disconnects
     * unexpectedly. */
    willInfo.pTopicName = WILL_TOPIC_NAME;
    willInfo.topicNameLength = WILL_TOPIC_NAME_LENGTH;
    willInfo.pPayload = WILL_MESSAGE;
    willInfo.payloadLength = WILL_MESSAGE_LENGTH;

    /* Use the parameter client identifier if provided. Otherwise, generate a
     * unique client identifier. */
    if ((pIdentifier != NULL) && (pIdentifier[0] != '\0'))
    {
        connectInfo.pClientIdentifier = pIdentifier;
        connectInfo.clientIdentifierLength = (uint16_t) strlen(pIdentifier);
    }
    else
    {
        /* Every active MQTT connection must have a unique client identifier. The demos
         * generate this unique client identifier by appending a timestamp to a common
         * prefix. */
        status = snprintf(pClientIdentifierBuffer,
        CLIENT_IDENTIFIER_MAX_LENGTH,
                          CLIENT_IDENTIFIER_PREFIX "%lu",
                          (long unsigned int) IotClock_GetTimeMs());

        /* Check for errors from snprintf. */
        if (status < 0)
        {
            IotLogError(
                    "Failed to generate unique client identifier for demo.");
            status = EXIT_FAILURE;
        }
        else
        {
            /* Set the client identifier buffer and length. */
            connectInfo.pClientIdentifier = pClientIdentifierBuffer;
            connectInfo.clientIdentifierLength = (uint16_t) status;

            status = EXIT_SUCCESS;
        }
    }

    /* Establish the MQTT connection. */
    if (status == EXIT_SUCCESS)
    {
        IotLogInfo("MQTT demo client identifier is %.*s (length %hu).",
                   connectInfo.clientIdentifierLength,
                   connectInfo.pClientIdentifier,
                   connectInfo.clientIdentifierLength);

        connectStatus = IotMqtt_Connect(&networkInfo, &connectInfo,
        MQTT_TIMEOUT_MS,
                                        pMqttConnection);

        if (connectStatus != IOT_MQTT_SUCCESS)
        {
            IotLogError("MQTT CONNECT returned error %s.",
                        IotMqtt_strerror(connectStatus));

            status = EXIT_FAILURE;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

/*
 * Custom MQTT library initialisation. This function initialised both the mqtt & shadow services
 *
 */
int aws_custom_mqttlib_initialise(
        bool awsIotMqttMode, const char * pIdentifier,
        void * pNetworkServerInfo, void * pNetworkCredentialInfo,
        const IotNetworkInterface_t * pNetworkInterface)
{

    configPRINTF(( "ENTRY: aws_custom_mqttlib_initialise\r\n" ));

    int status = EXIT_SUCCESS;
    int shadowInitStatus = EXIT_SUCCESS;
    IotMqttError_t mqttInitStatus = IOT_MQTT_SUCCESS;
    /* Handle of the MQTT connection used in this demo. */
    IotMqttConnection_t mqttConnection = IOT_MQTT_CONNECTION_INITIALIZER;

    mqttInitStatus = IotMqtt_Init();

    if (mqttInitStatus != IOT_MQTT_SUCCESS)
    {
        /* Failed to initialize MQTT library. */
        status = EXIT_FAILURE;
    }

    if (status == EXIT_SUCCESS)
    {
        /* Establish a new MQTT connection. */
        status = _establishMqttConnection(awsIotMqttMode, pIdentifier,
                                          pNetworkServerInfo,
                                          pNetworkCredentialInfo,
                                          pNetworkInterface, &mqttConnection);
    }

    if (status == EXIT_SUCCESS)
    {
        gpmqttConnection = mqttConnection; //For Testing
    }

    if (status == EXIT_SUCCESS)
    {
        IotLogInfo("MQTT-CUST_INIT:MQTT Initialisation successful");

        IotLogInfo("MQTT-CUST_INIT:MQTT Initialising Shadow Service");

        /* Use the default MQTT timeout. */
        shadowInitStatus = AwsIotShadow_Init(0);

        if (shadowInitStatus != AWS_IOT_SHADOW_SUCCESS)
        {
            status = EXIT_FAILURE;
        }
        else
        {

            //register for the shadow callbacks
            shadowInitStatus |= init_shadow_callbacks((IotMqttConnection_t *)gpmqttConnection,
                                                      pIdentifier);

            IotLogInfo("MQTT-CUST_INIT:MQTT-SHADOW Initialisation Successful");
        }

        status |= shadowInitStatus;

    }

    configPRINTF(( "EXIT: aws_custom_mqttlib_initialise with status:[%d] \r\n", status ));
    return status;
}

/*
 * Utility function to send application data to AWS cloud.
 * This leverages the freeRTOS mqtt APIs
 *
 */
int _publishSensorData(unsigned char *pTopicName, char *pMsg, int msglen)
{

    IotMqttPublishInfo_t publishInfo = IOT_MQTT_PUBLISH_INFO_INITIALIZER;
    IotMqttCallbackInfo_t publishComplete = IOT_MQTT_CALLBACK_INFO_INITIALIZER;
    IotMqttError_t publishStatus = IOT_MQTT_STATUS_PENDING;

    //publish Json string
    publishComplete.pCallbackContext = NULL;  // no need for stats right now
    publishInfo.pTopicName = (const char *) pTopicName;

    //Disable the callback, as for real-time telemetry data publishing there is no need durable connections
    //Enable this only when you need to implement application level retries
    publishComplete.function = NULL;

    publishInfo.qos = IOT_MQTT_QOS_1;
    publishInfo.topicNameLength = strlen((const char *)pTopicName);
    publishInfo.payloadLength = msglen;
    publishInfo.pPayload = pMsg;
    publishInfo.retryMs = PUBLISH_RETRY_MS;
    publishInfo.retryLimit = PUBLISH_RETRY_LIMIT;

    IotLogInfo("ENTRY:_publishSensorData");
    publishStatus = IotMqtt_Publish(gpmqttConnection, &publishInfo, 0,
                                    &publishComplete, NULL);

    if (publishStatus != IOT_MQTT_STATUS_PENDING)
    {

        IotLogError("IoT MQTT Publish returned error %s ",
                    IotMqtt_strerror(publishStatus));
    }
    else
    {
        IotLogInfo("Message submitted for publish successfully");
    }

    IotLogInfo("EXIT :_publishSensorData with status :[%d]", publishStatus);
    return publishStatus;
}

