/*
 * peripherals.c
 *
 *  Created on: Dec 27, 2020
 *      Author: jaehunchoe
 */

#include "main.h"
#include "peripherals.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_uart.h"

#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__)/sizeof(*(__BUFFER__)))

extern UART_HandleTypeDef huart1;

static char str_test[] = "UART1 low level - Hello World\n\r";
static uint8_t ch_linefeed = {'\n'};
static uint8_t ch_CR = {'\r'};

uint8_t g_ch_uart1_rx_data = {'\0'};


// 보드 상 파란색 버튼을 누르면 LED on/off되는 콜백 예
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if ( GPIO_Pin == USER_BUTTON_Pin) {
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	}
}

// UART1 RX complete시 불리는 콜백함수 (지금 에코 기능만 일단 넣었고, CR+NL 에코 출력은 NL이 안 먹는 것 같아 수정 필 )
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if ( huart->Instance == USART1 ) { // UART1 RX 끝날 때 마다 할일
		 if (g_ch_uart1_rx_data == 'r' || g_ch_uart1_rx_data == '\n' ) {
			 HAL_UART_Transmit(&huart1, &ch_linefeed, 1, 1); // CR + NL을 에코 출력
			 HAL_UART_Transmit(&huart1, &ch_CR, 1, 1);
		 } else {
			 HAL_UART_Transmit(&huart1, &g_ch_uart1_rx_data, 1, 1); // 해당 캐릭터를 에코 출력
		 }
		 HAL_UART_Receive_IT(&huart1, &g_ch_uart1_rx_data, 1); // UART1 RX 인터럽트 다시 살린다.
	}
}

// low-level HAL_UART_Transmit을 이용한 UART1 output test 확인.
int test_UART1_Output () {
	HAL_UART_Transmit(&huart1, str_test,COUNTOF(str_test)-1, 10);
}

// libc의 printf(tyny printf로 추정)에서 call하는 _write ==> __io_putchar 중 __io_putchar를 overriding한 함수
int __io_putchar(int ch) {

	int ret;
	ret = HAL_UART_Transmit(&huart1, &ch, 1, 10);
	return ret;
}



