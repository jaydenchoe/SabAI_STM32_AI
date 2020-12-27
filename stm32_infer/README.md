# STM32 inference board (추론용보드)

## Installation

1. STM32CubeIde를 실행한다
2. "File" Menu -> "Open Projects from File System..." 메뉴로 들어간다
3. "Directory" button을 눌러서 stm32_infer folder를 선택하고 "Finish" 버튼을 누른다. 
4. 이후 프로젝트를 생성하고 빌드하는데 문제가 있으면 꼭 알려주세요.


## 실행 및 Flashing 방법

1. 보드를 PC에 연결한다. 전원 LED가 들어오는 것을 확인한다.
2. 터미널 프로그램을 띄우고 보드의 UART1과 시리얼 채널을 연결한다. 맥의 경우 /dev/ 아래의 cu.usbmodem14503 비슷한 형태로 시리얼 노드가 존재한다.
3. 터미널 프로그램의 시리얼 셋팅은 코드 중 main.c의 static void MX_USART1_UART_Init(void) 함수 내용 참고 (115200 baud rate). 
4. IDE 하부의 콘솔창 부분에 내장 시리얼 터미널을 추가할 수도 있다. 콘솔창 메뉴버튼들 중 + 버튼을 누르고 이어 나오는 창에서 Command Shell Console을 선택하고 이후 Serial Device를 선택하면 된다. 빌드시 자꾸 가려져서 아주 편리하지는 않으므로 별도 터미널을 사용할 것을 권한다.
5. STM32CubeIDE 상에서 화면 중앙 상단의 초록색 화살표를 누르면 IDE 하단의 콘솔 부분에서 STM32CubeProgrammer가 실행되어 펌웨어 바이너리가 Flashing 되며, 이후 IDE가 보드를 리셋해서 보드가 실행된다
6. 이제 바이너리가 이미 구워진 상태이므로, 이후에 보드를 수동 리셋해서 다시 시작하려면 보드의 까만 리셋 버튼을 누른다.  
