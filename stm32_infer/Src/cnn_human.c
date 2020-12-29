/**
  ******************************************************************************
  * @file    cnn_human.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Mon Dec 28 04:22:16 2020
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */


#include "cnn_human.h"

#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "layers.h"



#undef AI_TOOLS_VERSION_MAJOR
#undef AI_TOOLS_VERSION_MINOR
#undef AI_TOOLS_VERSION_MICRO
#define AI_TOOLS_VERSION_MAJOR 5
#define AI_TOOLS_VERSION_MINOR 2
#define AI_TOOLS_VERSION_MICRO 0


#undef AI_TOOLS_API_VERSION_MAJOR
#undef AI_TOOLS_API_VERSION_MINOR
#undef AI_TOOLS_API_VERSION_MICRO
#define AI_TOOLS_API_VERSION_MAJOR 1
#define AI_TOOLS_API_VERSION_MINOR 3
#define AI_TOOLS_API_VERSION_MICRO 0

#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_cnn_human
 
#undef AI_CNN_HUMAN_MODEL_SIGNATURE
#define AI_CNN_HUMAN_MODEL_SIGNATURE     "161869c43c63b0f9381fd5395090921f"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     "(rev-5.2.0)"
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "Mon Dec 28 04:22:16 2020"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_CNN_HUMAN_N_BATCHES
#define AI_CNN_HUMAN_N_BATCHES         (1)

/**  Forward network declaration section  *************************************/
AI_STATIC ai_network AI_NET_OBJ_INSTANCE;


/**  Forward network array declarations  **************************************/
AI_STATIC ai_array conv2d_6_scratch0_array;   /* Array #0 */
AI_STATIC ai_array dense_18_bias_array;   /* Array #1 */
AI_STATIC ai_array dense_18_weights_array;   /* Array #2 */
AI_STATIC ai_array dense_17_bias_array;   /* Array #3 */
AI_STATIC ai_array dense_17_weights_array;   /* Array #4 */
AI_STATIC ai_array dense_16_bias_array;   /* Array #5 */
AI_STATIC ai_array dense_16_weights_array;   /* Array #6 */
AI_STATIC ai_array conv2d_6_bias_array;   /* Array #7 */
AI_STATIC ai_array conv2d_6_weights_array;   /* Array #8 */
AI_STATIC ai_array input_0_output_array;   /* Array #9 */
AI_STATIC ai_array conv2d_6_output_array;   /* Array #10 */
AI_STATIC ai_array dense_16_output_array;   /* Array #11 */
AI_STATIC ai_array dense_16_nl_output_array;   /* Array #12 */
AI_STATIC ai_array dense_17_output_array;   /* Array #13 */
AI_STATIC ai_array dense_17_nl_output_array;   /* Array #14 */
AI_STATIC ai_array dense_18_output_array;   /* Array #15 */
AI_STATIC ai_array dense_18_nl_output_array;   /* Array #16 */


/**  Forward network tensor declarations  *************************************/
AI_STATIC ai_tensor conv2d_6_scratch0;   /* Tensor #0 */
AI_STATIC ai_tensor dense_18_bias;   /* Tensor #1 */
AI_STATIC ai_tensor dense_18_weights;   /* Tensor #2 */
AI_STATIC ai_tensor dense_17_bias;   /* Tensor #3 */
AI_STATIC ai_tensor dense_17_weights;   /* Tensor #4 */
AI_STATIC ai_tensor dense_16_bias;   /* Tensor #5 */
AI_STATIC ai_tensor dense_16_weights;   /* Tensor #6 */
AI_STATIC ai_tensor conv2d_6_bias;   /* Tensor #7 */
AI_STATIC ai_tensor conv2d_6_weights;   /* Tensor #8 */
AI_STATIC ai_tensor input_0_output;   /* Tensor #9 */
AI_STATIC ai_tensor conv2d_6_output;   /* Tensor #10 */
AI_STATIC ai_tensor conv2d_6_output0;   /* Tensor #11 */
AI_STATIC ai_tensor dense_16_output;   /* Tensor #12 */
AI_STATIC ai_tensor dense_16_nl_output;   /* Tensor #13 */
AI_STATIC ai_tensor dense_17_output;   /* Tensor #14 */
AI_STATIC ai_tensor dense_17_nl_output;   /* Tensor #15 */
AI_STATIC ai_tensor dense_18_output;   /* Tensor #16 */
AI_STATIC ai_tensor dense_18_nl_output;   /* Tensor #17 */


/**  Forward network tensor chain declarations  *******************************/
AI_STATIC_CONST ai_tensor_chain conv2d_6_chain;   /* Chain #0 */
AI_STATIC_CONST ai_tensor_chain dense_16_chain;   /* Chain #1 */
AI_STATIC_CONST ai_tensor_chain dense_16_nl_chain;   /* Chain #2 */
AI_STATIC_CONST ai_tensor_chain dense_17_chain;   /* Chain #3 */
AI_STATIC_CONST ai_tensor_chain dense_17_nl_chain;   /* Chain #4 */
AI_STATIC_CONST ai_tensor_chain dense_18_chain;   /* Chain #5 */
AI_STATIC_CONST ai_tensor_chain dense_18_nl_chain;   /* Chain #6 */


/**  Forward network layer declarations  **************************************/
AI_STATIC ai_layer_conv2d_nl_pool conv2d_6_layer; /* Layer #0 */
AI_STATIC ai_layer_dense dense_16_layer; /* Layer #1 */
AI_STATIC ai_layer_nl dense_16_nl_layer; /* Layer #2 */
AI_STATIC ai_layer_dense dense_17_layer; /* Layer #3 */
AI_STATIC ai_layer_nl dense_17_nl_layer; /* Layer #4 */
AI_STATIC ai_layer_dense dense_18_layer; /* Layer #5 */
AI_STATIC ai_layer_nl dense_18_nl_layer; /* Layer #6 */


/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_6_scratch0_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 512, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  dense_18_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 3, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  dense_18_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 384, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  dense_17_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  dense_17_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 16384, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  dense_16_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  dense_16_weights_array, AI_ARRAY_FORMAT_LUT8_FLOAT,
  NULL, NULL, 720896, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_6_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_6_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 512, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  input_0_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 270, AI_STATIC)

/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_6_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 5632, AI_STATIC)

/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  dense_16_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#12 */
AI_ARRAY_OBJ_DECLARE(
  dense_16_nl_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#13 */
AI_ARRAY_OBJ_DECLARE(
  dense_17_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#14 */
AI_ARRAY_OBJ_DECLARE(
  dense_17_nl_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)

/* Array#15 */
AI_ARRAY_OBJ_DECLARE(
  dense_18_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 3, AI_STATIC)

/* Array#16 */
AI_ARRAY_OBJ_DECLARE(
  dense_18_nl_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 3, AI_STATIC)

/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_6_scratch0, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 2, 2), AI_STRIDE_INIT(4, 4, 4, 512, 1024),
  1, &conv2d_6_scratch0_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  dense_18_bias, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &dense_18_bias_array, NULL)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  dense_18_weights, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 128, 3, 1, 1), AI_STRIDE_INIT(4, 4, 512, 1536, 1536),
  1, &dense_18_weights_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  dense_17_bias, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &dense_17_bias_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  dense_17_weights, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 128, 128, 1, 1), AI_STRIDE_INIT(4, 4, 512, 65536, 65536),
  1, &dense_17_weights_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  dense_16_bias, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &dense_16_bias_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  dense_16_weights, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 5632, 128, 1, 1), AI_STRIDE_INIT(4, 1, 5632, 720896, 720896),
  1, &dense_16_weights_array, NULL)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_6_bias, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &conv2d_6_bias_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_6_weights, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 2, 2, 128), AI_STRIDE_INIT(4, 4, 4, 8, 16),
  1, &conv2d_6_weights_array, NULL)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  input_0_output, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 1, 3, 90), AI_STRIDE_INIT(4, 4, 4, 4, 12),
  1, &input_0_output_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_6_output, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 44), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &conv2d_6_output_array, NULL)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_6_output0, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 5632, 1, 1), AI_STRIDE_INIT(4, 4, 4, 22528, 22528),
  1, &conv2d_6_output_array, NULL)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  dense_16_output, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &dense_16_output_array, NULL)

/* Tensor #13 */
AI_TENSOR_OBJ_DECLARE(
  dense_16_nl_output, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &dense_16_nl_output_array, NULL)

/* Tensor #14 */
AI_TENSOR_OBJ_DECLARE(
  dense_17_output, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &dense_17_output_array, NULL)

/* Tensor #15 */
AI_TENSOR_OBJ_DECLARE(
  dense_17_nl_output, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &dense_17_nl_output_array, NULL)

/* Tensor #16 */
AI_TENSOR_OBJ_DECLARE(
  dense_18_output, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &dense_18_output_array, NULL)

/* Tensor #17 */
AI_TENSOR_OBJ_DECLARE(
  dense_18_nl_output, AI_STATIC,
  0x0, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &dense_18_nl_output_array, NULL)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_6_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &input_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_6_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_6_weights, &conv2d_6_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_6_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_6_layer, 0,
  OPTIMIZED_CONV2D_TYPE,
  conv2d_nl_pool, forward_conv2d_nl_pool,
  &AI_NET_OBJ_INSTANCE, &dense_16_layer, AI_STATIC,
  .tensors = &conv2d_6_chain, 
  .groups = 1, 
  .nl_func = nl_func_relu_array_f32, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .pool_func = pool_func_mp_array_f32, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_16_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_6_output0),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_16_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &dense_16_weights, &dense_16_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_16_layer, 4,
  DENSE_TYPE,
  dense, forward_dense,
  &AI_NET_OBJ_INSTANCE, &dense_16_nl_layer, AI_STATIC,
  .tensors = &dense_16_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_16_nl_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_16_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_16_nl_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_16_nl_layer, 4,
  NL_TYPE,
  nl, forward_relu,
  &AI_NET_OBJ_INSTANCE, &dense_17_layer, AI_STATIC,
  .tensors = &dense_16_nl_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_17_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_16_nl_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_17_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &dense_17_weights, &dense_17_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_17_layer, 5,
  DENSE_TYPE,
  dense, forward_dense,
  &AI_NET_OBJ_INSTANCE, &dense_17_nl_layer, AI_STATIC,
  .tensors = &dense_17_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_17_nl_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_17_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_17_nl_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_17_nl_layer, 5,
  NL_TYPE,
  nl, forward_relu,
  &AI_NET_OBJ_INSTANCE, &dense_18_layer, AI_STATIC,
  .tensors = &dense_17_nl_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_18_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_17_nl_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_18_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &dense_18_weights, &dense_18_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_18_layer, 6,
  DENSE_TYPE,
  dense, forward_dense,
  &AI_NET_OBJ_INSTANCE, &dense_18_nl_layer, AI_STATIC,
  .tensors = &dense_18_chain, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  dense_18_nl_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_18_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &dense_18_nl_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  dense_18_nl_layer, 6,
  NL_TYPE,
  nl, forward_sm,
  &AI_NET_OBJ_INSTANCE, &dense_18_nl_layer, AI_STATIC,
  .tensors = &dense_18_nl_chain, 
)


AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 792588, 1,
                     NULL),
  AI_BUFFER_OBJ_INIT(AI_BUFFER_FORMAT_U8,
                     1, 1, 24576, 1,
                     NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_CNN_HUMAN_IN_NUM, &input_0_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_CNN_HUMAN_OUT_NUM, &dense_18_nl_output),
  &conv2d_6_layer, 0, NULL)



AI_DECLARE_STATIC
ai_bool cnn_human_configure_activations(
  ai_network* net_ctx, const ai_buffer* activation_buffer)
{
  AI_ASSERT(net_ctx &&  activation_buffer && activation_buffer->data)

  ai_ptr activations = AI_PTR(AI_PTR_ALIGN(activation_buffer->data, AI_CNN_HUMAN_ACTIVATIONS_ALIGNMENT));
  AI_ASSERT(activations)
  AI_UNUSED(net_ctx)

  {
    /* Updating activations (byte) offsets */
    conv2d_6_scratch0_array.data = AI_PTR(activations + 0);
    conv2d_6_scratch0_array.data_start = AI_PTR(activations + 0);
    input_0_output_array.data = AI_PTR(NULL);
    input_0_output_array.data_start = AI_PTR(NULL);
    conv2d_6_output_array.data = AI_PTR(activations + 2048);
    conv2d_6_output_array.data_start = AI_PTR(activations + 2048);
    dense_16_output_array.data = AI_PTR(activations + 0);
    dense_16_output_array.data_start = AI_PTR(activations + 0);
    dense_16_nl_output_array.data = AI_PTR(activations + 512);
    dense_16_nl_output_array.data_start = AI_PTR(activations + 512);
    dense_17_output_array.data = AI_PTR(activations + 0);
    dense_17_output_array.data_start = AI_PTR(activations + 0);
    dense_17_nl_output_array.data = AI_PTR(activations + 512);
    dense_17_nl_output_array.data_start = AI_PTR(activations + 512);
    dense_18_output_array.data = AI_PTR(activations + 0);
    dense_18_output_array.data_start = AI_PTR(activations + 0);
    dense_18_nl_output_array.data = AI_PTR(NULL);
    dense_18_nl_output_array.data_start = AI_PTR(NULL);
    
  }
  return true;
}



AI_DECLARE_STATIC
ai_bool cnn_human_configure_weights(
  ai_network* net_ctx, const ai_buffer* weights_buffer)
{
  AI_ASSERT(net_ctx &&  weights_buffer && weights_buffer->data)

  ai_ptr weights = AI_PTR(weights_buffer->data);
  AI_ASSERT(weights)
  AI_UNUSED(net_ctx)

  {
    /* Updating weights (byte) offsets */
    
    dense_18_bias_array.format |= AI_FMT_FLAG_CONST;
    dense_18_bias_array.data = AI_PTR(weights + 792576);
    dense_18_bias_array.data_start = AI_PTR(weights + 792576);
    dense_18_weights_array.format |= AI_FMT_FLAG_CONST;
    dense_18_weights_array.data = AI_PTR(weights + 791040);
    dense_18_weights_array.data_start = AI_PTR(weights + 791040);
    dense_17_bias_array.format |= AI_FMT_FLAG_CONST;
    dense_17_bias_array.data = AI_PTR(weights + 790528);
    dense_17_bias_array.data_start = AI_PTR(weights + 790528);
    dense_17_weights_array.format |= AI_FMT_FLAG_CONST;
    dense_17_weights_array.data = AI_PTR(weights + 724992);
    dense_17_weights_array.data_start = AI_PTR(weights + 724992);
    dense_16_bias_array.format |= AI_FMT_FLAG_CONST;
    dense_16_bias_array.data = AI_PTR(weights + 724480);
    dense_16_bias_array.data_start = AI_PTR(weights + 724480);
    dense_16_weights_array.format |= AI_FMT_FLAG_CONST;
    dense_16_weights_array.data = AI_PTR(weights + 3584);
    dense_16_weights_array.data_start = AI_PTR(weights + 2560);
    conv2d_6_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_6_bias_array.data = AI_PTR(weights + 2048);
    conv2d_6_bias_array.data_start = AI_PTR(weights + 2048);
    conv2d_6_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_6_weights_array.data = AI_PTR(weights + 0);
    conv2d_6_weights_array.data_start = AI_PTR(weights + 0);
  }

  return true;
}


/**  PUBLIC APIs SECTION  *****************************************************/

AI_API_ENTRY
ai_bool ai_cnn_human_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if ( report && net_ctx )
  {
    ai_network_report r = {
      .model_name        = AI_CNN_HUMAN_MODEL_NAME,
      .model_signature   = AI_CNN_HUMAN_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = {AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR,
                            AI_TOOLS_API_VERSION_MICRO, 0x0},

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 874541,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .activations       = AI_STRUCT_INIT,
      .params            = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x0,
    };

    if ( !ai_platform_api_get_network_report(network, &r) ) return false;

    *report = r;
    return true;
  }

  return false;
}

AI_API_ENTRY
ai_error ai_cnn_human_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}

AI_API_ENTRY
ai_error ai_cnn_human_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    &AI_NET_OBJ_INSTANCE,
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}

AI_API_ENTRY
ai_handle ai_cnn_human_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}

AI_API_ENTRY
ai_bool ai_cnn_human_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = ai_platform_network_init(network, params);
  if ( !net_ctx ) return false;

  ai_bool ok = true;
  ok &= cnn_human_configure_weights(net_ctx, &params->params);
  ok &= cnn_human_configure_activations(net_ctx, &params->activations);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_cnn_human_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}

AI_API_ENTRY
ai_i32 ai_cnn_human_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}




#undef AI_CNN_HUMAN_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_VERSION_MAJOR
#undef AI_TOOLS_VERSION_MINOR
#undef AI_TOOLS_VERSION_MICRO
#undef AI_TOOLS_API_VERSION_MAJOR
#undef AI_TOOLS_API_VERSION_MINOR
#undef AI_TOOLS_API_VERSION_MICRO
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

