
#ifdef __cplusplus
 extern "C" {
#endif
/**
  ******************************************************************************
  * @file           : app_x-cube-ai.c
  * @brief          : AI program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
 /*
  * Description
  *   v1.0 - Minimum template to show how to use the Embedded Client API
  *          model. Only one input and one output is supported. All
  *          memory resources are allocated statically (AI_NETWORK_XX, defines
  *          are used).
  *          Re-target of the printf function is out-of-scope.
  *
  *   For more information, see the embeded documentation:
  *
  *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
  *
  *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
  *   typical : C:\Users\<user_name>\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\5.2.0
  */
/* Includes ------------------------------------------------------------------*/
/* System headers */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "main.h"
#include "ai_datatypes_defines.h"

/* USER CODE BEGIN includes */
 extern ai_handle ai_axis_sensor_data_weights_get(void);
/* USER CODE END includes */

/* Global AI objects */
static ai_handle axis_sensor = AI_HANDLE_NULL;
static ai_network_report axis_sensor_info;

/* Global c-array to handle the activations buffer */
AI_ALIGNED(4)
static ai_u8 activations[AI_AXIS_SENSOR_DATA_ACTIVATIONS_SIZE];

/*  In the case where "--allocate-inputs" option is used, memory buffer can be
 *  used from the activations buffer. This is not mandatory.
 */
#if !defined(AI_AXIS_SENSOR_INPUTS_IN_ACTIVATIONS)
/* Allocate data payload for input tensor */
AI_ALIGNED(4)
static ai_u8 in_data_s[AI_AXIS_SENSOR_IN_1_SIZE_BYTES];
#endif

/*  In the case where "--allocate-outputs" option is used, memory buffer can be
 *  used from the activations buffer. This is no mandatory.
 */
#if !defined(AI_AXIS_SENSOR_OUTPUTS_IN_ACTIVATIONS)
/* Allocate data payload for the output tensor */
AI_ALIGNED(4)
static ai_u8 out_data_s[AI_AXIS_SENSOR_OUT_1_SIZE_BYTES];
#endif

static void ai_log_err(const ai_error err, const char *fct)
{
  /* USER CODE BEGIN 0 */
  if (fct)
    printf("TEMPLATE - Error (%s) - type=0x%02x code=0x%02x\r\n", fct,
        err.type, err.code);
  else
    printf("TEMPLATE - Error - type=0x%02x code=0x%02x\r\n", err.type, err.code);

  do {} while (1);
  /* USER CODE END 0 */
}

static int ai_boostrap(ai_handle w_addr, ai_handle act_addr)
{
  ai_error err;

  /* 1 - Create an instance of the model */
  err = ai_axis_sensor_create(&axis_sensor, AI_AXIS_SENSOR_DATA_CONFIG);
  if (err.type != AI_ERROR_NONE) {
    ai_log_err(err, "ai_axis_sensor_create");
    return -1;
  }

  /* 2 - Initialize the instance */
  const ai_network_params params = {
      AI_AXIS_SENSOR_DATA_WEIGHTS(w_addr),
      AI_AXIS_SENSOR_DATA_ACTIVATIONS(act_addr) };

  if (!ai_axis_sensor_init(axis_sensor, &params)) {
      err = ai_axis_sensor_get_error(axis_sensor);
      ai_log_err(err, "ai_axis_sensor_init");
      return -1;
    }

  /* 3 - Retrieve the network info of the created instance */
  if (!ai_axis_sensor_get_info(axis_sensor, &axis_sensor_info)) {
    err = ai_axis_sensor_get_error(axis_sensor);
    ai_log_err(err, "ai_axis_sensor_get_error");
    ai_axis_sensor_destroy(axis_sensor);
    axis_sensor = AI_HANDLE_NULL;
    return -3;
  }

  return 0;
}

static int ai_run(void *data_in, void *data_out)
{
  ai_i32 batch;

  ai_buffer *ai_input = axis_sensor_info.inputs;
  ai_buffer *ai_output = axis_sensor_info.outputs;

  ai_input[0].data = AI_HANDLE_PTR(data_in);
  ai_output[0].data = AI_HANDLE_PTR(data_out);

  batch = ai_axis_sensor_run(axis_sensor, ai_input, ai_output);
  if (batch != 1) {
    ai_log_err(ai_axis_sensor_get_error(axis_sensor),
        "ai_axis_sensor_run");
    return -1;
  }

  return 0;
}

/* USER CODE BEGIN 2 */
int acquire_and_process_data(void * data)
{
  return 0;
}

int post_process(void * data)
{
  return 0;
}
/* USER CODE END 2 */

/*************************************************************************
  *
  */
void MX_X_CUBE_AI_Init(void)
{
    /* USER CODE BEGIN 3 */
  printf("\r\nTEMPLATE - initialization\r\n");

  ai_boostrap(ai_axis_sensor_data_weights_get(), activations);
    /* USER CODE END 3 */
}

void MX_X_CUBE_AI_Process(void)
{
    /* USER CODE BEGIN 4 */

  int res = -1;
  uint8_t *in_data = NULL;
  uint8_t *out_data = NULL;

  printf("TEMPLATE - run - main loop\r\n");

  if (axis_sensor) {

    if (( axis_sensor_info.n_inputs != 1) || (axis_sensor_info.n_outputs != 1)) {
      ai_error err = {AI_ERROR_INVALID_PARAM, AI_ERROR_CODE_OUT_OF_RANGE};
      ai_log_err(err, "template code should be updated\r\n to support a model with multiple IO");
      return;
    }

    /* 1 - Set the I/O data buffer */

#if AI_CNN_HUMAN_INPUTS_IN_ACTIVATIONS
    in_data = cnn_human_info.inputs[0].data;
#else
    in_data = in_data_s;
#endif

#if AI_CNN_HUMAN_OUTPUTS_IN_ACTIVATIONS
    out_data = cnn_human_info.outputs[0].data;
#else
    out_data = out_data_s;
#endif

    if ((!in_data) || (!out_data)) {
      printf("TEMPLATE - I/O buffers are invalid\r\n");
      return;
    }

    /* 2 - main loop */
    do {
      /* 1 - acquire and pre-process input data */
      res = acquire_and_process_data(in_data);
      /* 2 - process the data - call inference engine */
      if (res == 0)
        res = ai_run(in_data, out_data);
      /* 3- post-process the predictions */
      if (res == 0)
        res = post_process(out_data);
    //} while ( res == 0 );  /// jayden.choe 무한루프가 되니 일단 한번만 실행되게 바꿨음.
    } while ( NULL );
  }

  if (res) {
    ai_error err = {AI_ERROR_INVALID_STATE, AI_ERROR_CODE_NETWORK};
    ai_log_err(err, "Process has FAILED");
  }
    /* USER CODE END 4 */
}
#ifdef __cplusplus
}
#endif
