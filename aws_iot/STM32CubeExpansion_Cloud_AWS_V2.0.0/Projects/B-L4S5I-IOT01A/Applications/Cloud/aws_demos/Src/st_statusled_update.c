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
#include "../../../../../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.h"
#include "iot_json_utils.h"
#include "platform/iot_clock.h"

int init_shadow_callbacks(IotMqttConnection_t * pMqttConnection, const char * pIdentifier);

/**
 * @brief Format string representing a Shadow document with a "desired" state.
 *
 * Note the client token, which is required for all Shadow updates. The client
 * token must be unique at any given time, but may be reused once the update is
 * completed. For this demo, a timestamp is used for a client token.
 */
#define SHADOW_DESIRED_JSON     \
    "{"                         \
    "\"state\":{"               \
    "\"desired\":{"             \
    "\"powerOn\":%01d"          \
    "}"                         \
    "},"                        \
    "\"clientToken\":\"%06lu\"" \
    "}"

/**
 * @brief The expected size of #SHADOW_DESIRED_JSON.
 *
 * Because all the format specifiers in #SHADOW_DESIRED_JSON include a length,
 * its full size is known at compile-time.
 */
#define EXPECTED_DESIRED_JSON_SIZE    ( sizeof( SHADOW_DESIRED_JSON ) - 3 )

/**
 * @brief Format string representing a Shadow document with a "reported" state.
 *
 * Note the client token, which is required for all Shadow updates. The client
 * token must be unique at any given time, but may be reused once the update is
 * completed. For this demo, a timestamp is used for a client token.
 */
#define SHADOW_REPORTED_JSON    \
    "{"                         \
    "\"state\":{"               \
    "\"reported\":{"            \
    "\"powerOn\":%01d"          \
    "}"                         \
    "},"                        \
    "\"clientToken\":\"%06lu\"" \
    "}"

/**
 * @brief The expected size of #SHADOW_REPORTED_JSON.
 *
 * Because all the format specifiers in #SHADOW_REPORTED_JSON include a length,
 * its full size is known at compile-time.
 */
#define EXPECTED_REPORTED_JSON_SIZE    ( sizeof( SHADOW_REPORTED_JSON ) - 3 )

  /* delta Sempaphore */
 IotSemaphore_t deltaSemaphore;

/**
 * @brief Parses a key in the "state" section of a Shadow delta document.
 *
 * @param[in] pDeltaDocument The Shadow delta document to parse.
 * @param[in] deltaDocumentLength The length of `pDeltaDocument`.
 * @param[in] pDeltaKey The key in the delta document to find. Must be NULL-terminated.
 * @param[out] pDelta Set to the first character in the delta key.
 * @param[out] pDeltaLength The length of the delta key.
 *
 * @return `true` if the given delta key is found; `false` otherwise.
 */
static bool _getDelta( const char * pDeltaDocument,
                       size_t deltaDocumentLength,
                       const char * pDeltaKey,
                       const char ** pDelta,
                       size_t * pDeltaLength )
{
    bool stateFound = false, deltaFound = false;
    const size_t deltaKeyLength = strlen( pDeltaKey );
    const char * pState = NULL;
    size_t stateLength = 0;

    /* Find the "state" key in the delta document. */
    stateFound = IotJsonUtils_FindJsonValue( pDeltaDocument,
                                             deltaDocumentLength,
                                             "state",
                                             5,
                                             &pState,
                                             &stateLength );

    if( stateFound == true )
    {
        /* Find the delta key within the "state" section. */
        deltaFound = IotJsonUtils_FindJsonValue( pState,
                                                 stateLength,
                                                 pDeltaKey,
                                                 deltaKeyLength,
                                                 pDelta,
                                                 pDeltaLength );
    }
    else
    {
        IotLogWarn( "Failed to find \"state\" in Shadow delta document." );
    }

    return deltaFound;
}

/*-----------------------------------------------------------*/

/**
 * @brief Parses the "state" key from the "previous" or "current" sections of a
 * Shadow updated document.
 *
 * @param[in] pUpdatedDocument The Shadow updated document to parse.
 * @param[in] updatedDocumentLength The length of `pUpdatedDocument`.
 * @param[in] pSectionKey Either "previous" or "current". Must be NULL-terminated.
 * @param[out] pState Set to the first character in "state".
 * @param[out] pStateLength Length of the "state" section.
 *
 * @return `true` if the "state" was found; `false` otherwise.
 */
static bool _getUpdatedState( const char * pUpdatedDocument,
                              size_t updatedDocumentLength,
                              const char * pSectionKey,
                              const char ** pState,
                              size_t * pStateLength )
{
    bool sectionFound = false, stateFound = false;
    const size_t sectionKeyLength = strlen( pSectionKey );
    const char * pSection = NULL;
    size_t sectionLength = 0;

    /* Find the given section in the updated document. */
    sectionFound = IotJsonUtils_FindJsonValue( pUpdatedDocument,
                                               updatedDocumentLength,
                                               pSectionKey,
                                               sectionKeyLength,
                                               &pSection,
                                               &sectionLength );

    if( sectionFound == true )
    {
        /* Find the "state" key within the "previous" or "current" section. */
        stateFound = IotJsonUtils_FindJsonValue( pSection,
                                                 sectionLength,
                                                 "state",
                                                 5,
                                                 pState,
                                                 pStateLength );
    }
    else
    {
        IotLogWarn( "Failed to find section %s in Shadow updated document.",
                    pSectionKey );
    }

    return stateFound;
}

/*-----------------------------------------------------------*/

/**
 * @brief Shadow delta callback, invoked when the desired and updates Shadow
 * states differ.
 *
 * This function simulates a device updating its state in response to a Shadow.
 *
 * @param[in] pCallbackContext Not used.
 * @param[in] pCallbackParam The received Shadow delta document.
 */
static void _shadowDeltaCallback( void * pCallbackContext,
                                  AwsIotShadowCallbackParam_t * pCallbackParam )
{
    bool deltaFound = false;
    const char * pDelta = NULL;
    size_t deltaLength = 0;
    IotSemaphore_t * pDeltaSemaphore = pCallbackContext;
    int updateDocumentLength = 0;
    AwsIotShadowError_t updateStatus = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowDocumentInfo_t updateDocument = AWS_IOT_SHADOW_DOCUMENT_INFO_INITIALIZER;

    /* Stored state. */
    static int32_t currentState = 0;

    /* A buffer containing the update document. It has static duration to prevent
     * it from being placed on the call stack. */
    static char pUpdateDocument[ EXPECTED_REPORTED_JSON_SIZE + 1 ] = { 0 };

    /* Check if there is a different "powerOn" state in the Shadow. */
    deltaFound = _getDelta( pCallbackParam->u.callback.pDocument,
                            pCallbackParam->u.callback.documentLength,
                            "powerOn",
                            &pDelta,
                            &deltaLength );

    if( deltaFound == true )
    {
        /* Change the current state based on the value in the delta document. */
        if( *pDelta == '0' )
        {
            IotLogInfo( "%.*s changing state from %d to 0.",
                        pCallbackParam->thingNameLength,
                        pCallbackParam->pThingName,
                        currentState );

            currentState = 0;

            //Turn OFF the LED
            BSP_LED_Off(LED2);
        }
        else if( *pDelta == '1' )
        {
            IotLogInfo( "%.*s changing state from %d to 1.",
                        pCallbackParam->thingNameLength,
                        pCallbackParam->pThingName,
                        currentState );

            currentState = 1;

            //Turn ON the LED
            BSP_LED_On(LED2);
        }
        else
        {
            IotLogWarn( "Unknown powerOn state parsed from delta document." );
        }

        /* Set the common members to report the new state. */
        updateDocument.pThingName = pCallbackParam->pThingName;
        updateDocument.thingNameLength = pCallbackParam->thingNameLength;
        updateDocument.u.update.pUpdateDocument = pUpdateDocument;
        updateDocument.u.update.updateDocumentLength = EXPECTED_REPORTED_JSON_SIZE;

        /* Generate a Shadow document for the reported state. To keep the client
         * token within 6 characters, it is modded by 1000000. */
        updateDocumentLength = snprintf( pUpdateDocument,
                                         EXPECTED_REPORTED_JSON_SIZE + 1,
                                         SHADOW_REPORTED_JSON,
                                         ( int ) currentState,
                                         ( long unsigned ) ( IotClock_GetTimeMs() % 1000000 ) );

        if( ( size_t ) updateDocumentLength != EXPECTED_REPORTED_JSON_SIZE )
        {
            IotLogError( "Failed to generate reported state document for Shadow update." );
        }
        else
        {
            /* Send the Shadow update. Its result is not checked, as the Shadow updated
             * callback will report if the Shadow was successfully updated. Because the
             * Shadow is constantly updated in this demo, the "Keep Subscriptions" flag
             * is passed to this function. */
            updateStatus = AwsIotShadow_Update( pCallbackParam->mqttConnection,
                                                &updateDocument,
                                                AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS,
                                                NULL,
                                                NULL );

            if( updateStatus != AWS_IOT_SHADOW_STATUS_PENDING )
            {
                IotLogWarn( "%.*s failed to report new state.",
                            pCallbackParam->thingNameLength,
                            pCallbackParam->pThingName );
            }
            else
            {
                IotLogInfo( "%.*s sent new state report.",
                            pCallbackParam->thingNameLength,
                            pCallbackParam->pThingName );
            }
        }
    }
    else
    {
        IotLogWarn( "Failed to parse powerOn state from delta document." );
    }

    /* Post to the delta semaphore to unblock the thread sending Shadow updates. */
    IotSemaphore_Post( pDeltaSemaphore );
}

/*-----------------------------------------------------------*/

/**
 * @brief Shadow updated callback, invoked when the Shadow document changes.
 *
 * This function reports when a Shadow has been updated.
 *
 * @param[in] pCallbackContext Not used.
 * @param[in] pCallbackParam The received Shadow updated document.
 */
static void _shadowUpdatedCallback( void * pCallbackContext,
                                    AwsIotShadowCallbackParam_t * pCallbackParam )
{
    bool previousFound = false, currentFound = false;
    const char * pPrevious = NULL, * pCurrent = NULL;
    size_t previousLength = 0, currentLength = 0;

    /* Silence warnings about unused parameters. */
    ( void ) pCallbackContext;

    /* Find the previous Shadow document. */
    previousFound = _getUpdatedState( pCallbackParam->u.callback.pDocument,
                                      pCallbackParam->u.callback.documentLength,
                                      "previous",
                                      &pPrevious,
                                      &previousLength );

    /* Find the current Shadow document. */
    currentFound = _getUpdatedState( pCallbackParam->u.callback.pDocument,
                                     pCallbackParam->u.callback.documentLength,
                                     "current",
                                     &pCurrent,
                                     &currentLength );

    /* Log the previous and current states. */
    if( ( previousFound == true ) && ( currentFound == true ) )
    {
        IotLogInfo( "Shadow was updated!\r\n"
                    "Previous: {\"state\":%.*s}\r\n"
                    "Current:  {\"state\":%.*s}",
                    previousLength,
                    pPrevious,
                    currentLength,
                    pCurrent );
    }
    else
    {
        if( previousFound == false )
        {
            IotLogWarn( "Previous state not found in Shadow updated document." );
        }

        if( currentFound == false )
        {
            IotLogWarn( "Current state not found in Shadow updated document." );
        }
    }
}



/**
 * @brief Set the Shadow callback functions used in this demo.
 *
 * @param[in] pDeltaSemaphore Used to synchronize Shadow updates with the delta
 * callback.
 * @param[in] mqttConnection The MQTT connection used for Shadows.
 * @param[in] pThingName The Thing Name for Shadows in this demo.
 * @param[in] thingNameLength The length of `pThingName`.
 *
 * @return `EXIT_SUCCESS` if all Shadow callbacks were set; `EXIT_FAILURE`
 * otherwise.
 */
static int _setShadowCallbacks( IotSemaphore_t * pDeltaSemaphore,
                                IotMqttConnection_t mqttConnection,
                                const char * pThingName,
                                size_t thingNameLength )
{
    int status = EXIT_SUCCESS;
    AwsIotShadowError_t callbackStatus = AWS_IOT_SHADOW_STATUS_PENDING;
    AwsIotShadowCallbackInfo_t deltaCallback = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER,
                               updatedCallback = AWS_IOT_SHADOW_CALLBACK_INFO_INITIALIZER;

    /* Set the functions for callbacks. */
    deltaCallback.pCallbackContext = pDeltaSemaphore;
    deltaCallback.function = _shadowDeltaCallback;
    updatedCallback.function = _shadowUpdatedCallback;

    /* Set the delta callback, which notifies of different desired and reported
     * Shadow states. */
    callbackStatus = AwsIotShadow_SetDeltaCallback( mqttConnection,
                                                    pThingName,
                                                    thingNameLength,
                                                    0,
                                                    &deltaCallback );

    if( callbackStatus == AWS_IOT_SHADOW_SUCCESS )
    {
        /* Set the updated callback, which notifies when a Shadow document is
         * changed. */
        callbackStatus = AwsIotShadow_SetUpdatedCallback( mqttConnection,
                                                          pThingName,
                                                          thingNameLength,
                                                          0,
                                                          &updatedCallback );
    }

    if( callbackStatus != AWS_IOT_SHADOW_SUCCESS )
    {
        IotLogError( "Failed to set demo shadow callback, error %s.",
                     AwsIotShadow_strerror( callbackStatus ) );

        status = EXIT_FAILURE;
    }

    return status;
}

int init_shadow_callbacks(IotMqttConnection_t * pMqttConnection, const char * pIdentifier) {

    int status = EXIT_SUCCESS;

    /* Allows the Shadow update function to wait for the delta callback to complete
      * a state change before continuing. */

    bool deltaSemaphoreCreated = false;
    size_t thingNameLength = 0;


    /* Determine the length of the Thing Name. */
    if (pIdentifier != NULL)
    {
        thingNameLength = strlen(pIdentifier);

        if (thingNameLength == 0)
        {
            IotLogError(
                    "The length of the Thing Name (identifier) must be nonzero.");
            status = EXIT_FAILURE;
        }
    }
    else
    {
        IotLogError(
                "A Thing Name (identifier) must be provided for the Shadow demo.");
        status = EXIT_FAILURE;
    }

    /* Create the semaphore that synchronizes with the delta callback. */
    deltaSemaphoreCreated = IotSemaphore_Create(&deltaSemaphore, 0, 1);

    if (deltaSemaphoreCreated == false)
    {
        status = EXIT_FAILURE;
    }

    if (status == EXIT_SUCCESS)
    {
        /* Set the Shadow callbacks for this demo. */
        status = _setShadowCallbacks(&deltaSemaphore, (IotMqttConnection_t)pMqttConnection,
                                     pIdentifier, thingNameLength);
    }

    return status;

}

