/*
 * peripherals.c
 *
 *  Created on: Dec 27, 2020
 *      Author: jaehunchoe
 */

#include <stdio.h>
#include "main.h"
#include "peripherals.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_uart.h"
#include "lsm6dsl.h" // for LSM6DSL sensor
#include "b_l4s5i_iot01a_bus.h" // for LSM6DSL sensor
#include "axis_sensor.h"

#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))

void MEMS_Init(void);
void get_and_print_3axis( void );

static int get_acc_3axis(int32_t* x, int32_t* y, int32_t* z);
static void print_3axis (int32_t x, int32_t y, int32_t z);

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart4;
extern float save_data[];
extern uint8_t pending_initial_data;
extern uint16_t begin_index;
extern float save_data[600];
extern uint8_t pending_initial_data;

static char str_uart1_test[] = "UART1 low level - Hello World\n\r";
static char str_uart4_test[] = "UART4 low level - Hello World\n\r";

static uint8_t ch_linefeed = {'\n'};
static uint8_t ch_CR = {'\r'};
#if CAPTURE_MODE
static uint8_t s_is_capture_started = 0;
#endif
uint8_t g_ch_uart1_rx_data = {'\0'};
uint8_t g_ch_uart4_rx_data = {'\0'}; // loopback test용

LSM6DSL_Object_t g_motion_sensor;
volatile uint32_t g_data_rdy_int_received = 0;
uint8_t new_sensor_data_detected_from_timer_event = 0;

/* GPIO and 3axis handling ------------------------------------------------------------------*/

// GPIO interrupt callback이며
// 1) 보드 상 파란색 버튼을 누르면 LED2 on/off 처리 하기도 하
// 2) 3축 센서 I2C2에 데이터가 있는지 알려주는 (0이아니면 데이터 있음) 역할을 한다.
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if ( GPIO_Pin == USER_BUTTON_Pin) {
#if CAPTURE_MODE
		if ( s_is_capture_started == 0) {
			s_is_capture_started = 1;
		} else {
			s_is_capture_started = 0;
		}
#else
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
#endif
	} else if (GPIO_Pin == GPIO_PIN_11) {
		g_data_rdy_int_received++;
	}
}

/* UART1 handling ------------------------------------------------------------------*/

// UART RX complete시 불리는 콜백함수
// 1) UART1 RX event시: 받은 캐릭터를 그대로 UART1 TX에 echo로 출력(UART_Transmit)한다. UART1은 Virtual COM 형태로 PC와 연결된다.
// 2) UART4 RX event시: 받은 캐릭터를 그대로 UART1 TX에 echo로 출력(UART_Transmit)한다. 이것은 UART4의 echo test 채널로만 사용할 목적이다. UART3는 다른 STM32 보드나 라즈베리파이(이경우에는 USB2Serial Dongle 필요)에 연결된다.
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if ( huart->Instance == USART1 ) { // UART1 RX 끝날 때 마다 할일
		 if (g_ch_uart1_rx_data == '\r' || g_ch_uart1_rx_data == '\n' ) {
			 HAL_UART_Transmit(&huart1, &ch_linefeed, 1, 1); // CR + NL을 에코 출력
			 HAL_UART_Transmit(&huart1, &ch_CR, 1, 1);
		 } else {
			 HAL_UART_Transmit(&huart1, &g_ch_uart1_rx_data, 1, 1); // 해당 캐릭터를 에코 출력
		 }
		 HAL_UART_Receive_IT(&huart1, &g_ch_uart1_rx_data, 1); // UART1 RX 인터럽트 다시 살린다.

	}
#if UART4_LOOPBACK_TEST
	else if ( huart->Instance == UART4 ) {
		 HAL_UART_Transmit(&huart1, &g_ch_uart4_rx_data, 1, 1); // UART4 데이터임을 알리는 prefix 특수 문자 출력
		 HAL_UART_Receive_IT(&huart4, &g_ch_uart4_rx_data, 1); // UART4 RX 인터럽트 다시 살린다.
	}
#endif
}


// low-level HAL_UART_Transmit을 이용한 UART1 output test 확인.
void test_UART1_Output () {
	HAL_UART_Transmit(&huart1, (uint8_t*)str_uart1_test,COUNTOF(str_uart1_test)-1, 10);
}

// low-level HAL_UART_Transmit을 이용한 UART4 output test 확인.
void test_UART4_Output () {
	HAL_UART_Transmit(&huart4, (uint8_t*)str_uart4_test,COUNTOF(str_uart4_test)-1, 10);
}


// libc의 printf(tyny printf로 추정)에서 call하는 _write ==> __io_putchar 중 __io_putchar를 overriding한 함수
int __io_putchar(int ch) {

	int ret;
	ret = HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 10);
	return ret;
}

/* Timer15 and Timer 5 handling ------------------------------------------------------------------*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	static int32_t x= -1;
	static int32_t y= -1;
	static int32_t z= -1;

// 25Hz로 Accelerometer 센서값을 얻어서 쌓고 일정한 양이 쌓이면 인공지능 모델에 던진다.
	if ( htim->Instance == TIM15 ) {
#if CAPTURE_MODE
		get_and_print_3axis();
#else // Inference Mode

		if ( get_acc_3axis(&x, &y, &z) == -1 ) { // no change or errornous case
//			printf( "ERROR: can't read 3axis\n\r");
			new_sensor_data_detected_from_timer_event = 0;
			return;
		}
		// 추론 배열에 값 추가
		save_data[begin_index++] = x;
		save_data[begin_index++] = y;
		save_data[begin_index++] = z;

		// If we reached the end of the ring buffer, reset
		if (begin_index >= 600) {
			  begin_index = 0;
		}
		new_sensor_data_detected_from_timer_event = 1;
#endif
	}
// 1Hz로 LED toggling
	if ( htim->Instance == TIM5 ) {
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	}
}

/* 3axis sensor handling ------------------------------------------------------------------*/

void MEMS_Init( void ) {
	LSM6DSL_IO_t io_ctx;
	uint8_t id;
	LSM6DSL_AxesRaw_t axes;

	g_data_rdy_int_received = 0; // jayden.choe

	/* Link I2C functions to the LSM6DSL Driver */
	io_ctx.BusType = LSM6DSL_I2C_BUS;
	io_ctx.Address = LSM6DSL_I2C_ADD_L;
	io_ctx.Init = BSP_I2C2_Init;
	io_ctx.DeInit = BSP_I2C2_DeInit;
	io_ctx.ReadReg = BSP_I2C2_ReadReg;
	io_ctx.WriteReg = BSP_I2C2_WriteReg;
	io_ctx.GetTick = BSP_GetTick;
	LSM6DSL_RegisterBusIO(&g_motion_sensor, &io_ctx);

	/* Read the LSM6DSL WHO_AM_I register */
	LSM6DSL_ReadID(&g_motion_sensor, &id);
	if (id != LSM6DSL_ID) {
	  Error_Handler();
	}

	/* Initialize the LSM6DSL sensor */
	LSM6DSL_Init(&g_motion_sensor);

	/* Configure the LSM6DSL accelerometer (ODR, scale and interrupt) */
	LSM6DSL_ACC_SetOutputDataRate(&g_motion_sensor, 26.0f); /* 26 Hz */
	LSM6DSL_ACC_SetFullScale(&g_motion_sensor, 4);          /* [-4000mg; +4000mg] */
	LSM6DSL_ACC_Set_INT1_DRDY(&g_motion_sensor, ENABLE);    /* Enable DRDY */
	LSM6DSL_ACC_GetAxesRaw(&g_motion_sensor, &axes);        /* Clear DRDY */

	/* Start the LSM6DSL accelerometer */
	LSM6DSL_ACC_Enable(&g_motion_sensor);
}

//  I2C2에서 현재의 3axis 값을 읽는다.
static int get_acc_3axis(int32_t* x, int32_t* y, int32_t* z) {
	 if (g_data_rdy_int_received != 0) {
		 g_data_rdy_int_received = 0;
		 LSM6DSL_Axes_t acc_axes;
		 LSM6DSL_ACC_GetAxes(&g_motion_sensor, &acc_axes);
		 (*x) = (int32_t)acc_axes.x;
		 (*y) = (int32_t)acc_axes.y;
		 (*z) = (int32_t)acc_axes.z;

		 return 0;

	 } else {

		 return -1;
	 }
}


// 3axis 값을 캡처용으로 출력 referring to https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/micro/examples/magic_wand/train/README.md
// 참조 내용: am_util_stdio_printf("%04.2f,%04.2f,%04.2f\r\n", acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);

static void print_3axis (int32_t x, int32_t y, int32_t z) {
	static uint8_t is_first_print = 1;
	static uint8_t i = 0;
#if CAPTURE_MODE
	if ( s_is_capture_started == 1 ) {
#endif
		if ( is_first_print == 1 ) {
		// 18 blank lines to separate motions
			for (i = 0; i < 18; ++i) {
				printf( "\r\n" );
			}
		// Motion Capture Header
			printf( "-,-,-\r\n");
			is_first_print = 0;
		} else {
		   printf("%ld.0,%ld.0,%ld.0\r\n", x, y, z); // float option이 없기 때문에 integer로 처리해야 하며 나중에 파이썬 코드 파싱에 문제 없는지 확인 필요
		}
#if CAPTURE_MODE
	}
#endif
}

// 3axis 현재 값을 얻어서 출력한다
void get_and_print_3axis( void ) {
	static int32_t x= -1;
	static int32_t y= -1;
	static int32_t z= -1;
	get_acc_3axis(&x, &y, &z);
	print_3axis(x,y,z);
}



