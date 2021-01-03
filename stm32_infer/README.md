# STM32 inference board (추론용보드)

## 코드 빌드/테스트 환경: 

1. MacOS
2. STM32CubeIDE Version Version: 1.5.1 Build: 9029_20201210_1234 (UTC)
3. STM32CubeProgrammer v2.6.0-RC3 (CubeIDE에 원래 같이 깔린듯?)
4. CubeMX SW package 중 MEMS1 8.2.0 패키지 아래 LDM6DSL 5.2.3 설치
5. CubeMX SW pacakage 중 Cube-AI 5.2.0 패키지 아래 X-CUBE-AI (Core) 5.2.0 와 Application 5.2.0 설치 

## STM32CubeIDE 환경에서 코드 Installation

1. STM32CubeIDE를 실행한다
2. "File" Menu -> "Open Projects from File System..." 메뉴로 들어간다
3. "Directory" button을 눌러서 "stm32_infer" folder를 선택하고 "Finish" 버튼을 누른다. 
4. 이후 프로젝트를 생성하고 빌드하는데 문제가 있으면 꼭 알려주세요.

## STM32CubeIDE 환경에서 바이너리 Flashing 및 실행 방법

1. 보드를 PC에 연결한다. 전원 LED가 들어오는 것을 확인한다.
2. 터미널 프로그램을 띄우고 보드의 UART1과 시리얼 채널을 연결한다. 맥의 경우 /dev/ 아래의 cu.usbmodem14503 비슷한 형태로 시리얼 노드가 존재한다.
3. 터미널 프로그램의 시리얼 셋팅은 코드 중 main.c의 static void MX_USART1_UART_Init(void) 함수 내용 참고 (115200 baud rate). 
4. IDE 하부의 콘솔창 부분에 내장 시리얼 터미널을 추가할 수도 있다. 콘솔창 메뉴버튼들 중 + 버튼을 누르고 이어 나오는 창에서 Command Shell Console을 선택하고 이후 Serial Device를 선택하면 된다. 빌드시 자꾸 가려져서 아주 편리하지는 않으므로 별도 터미널을 사용할 것을 권한다.
5. STM32CubeIDE 상에서 화면 중앙 상단의 초록색 화살표를 누르면 IDE 하단의 콘솔 부분에서 STM32CubeProgrammer가 실행되어 펌웨어 바이너리가 Flashing 되며, 이후 IDE가 보드를 리셋해서 보드가 실행된다
6. 이제 바이너리가 이미 구워진 상태이므로, 이후에 보드를 수동 리셋해서 다시 시작하려면 보드의 까만 리셋 버튼을 누른다.  

## STM32CubeMX 환경에서 코드 로딩 방법

1. STM32CubeMX를 실행한다.
2. 첫화면에서 "Other Projects"를 선택한다.
3. "stm32_infer" 풀더에 들어가서 "stm32_infer.ioc"를 선택하고 "열기" 버튼을 누른다.
4. 혹은 CubeIDE 화면 왼쪽 코드 내비게이션 창에서 "ioc" 파일을 더블클릭해도 된다.

## 현재 깃헙 코드상에 enabling 되어 있는 디바이스 예제 리스트
1. UART1 RX/TX 
2. Timer (1초)
3. GPIO LED ON/OFF
4. Printf
5. 3축 센서 입력
6. AI 모델 template 1차 완료. 일단 케라스 모델 3축 모델 찾아서 STM32CubeMX 설정에서 끼워 넣었음. https://github.com/ausilianapoli/HAR-CNN-Keras-STM32. 하지만 입력 부분, 출력 부분 손봐야함. Normalization, Windowing, 해당 모델과 정합성 등.... 봐야함.
7. AWS 보드에 command 날릴 목적으로 UART4 RX/TX 살렸습니다. 루프백으로 테스트 코드와 ifdef 넣어서 commit 했습니다. 루프백 코드 돌리려면 UART4 TX/RX가 아두이노 커넥터 PA0/1이니 이 둘을 묶어주고 테스트 해 보면 됩니다(굳이 할일은 없겠으나). 참고자료: https://os.mbed.com/platforms/B-L4S5I-IOT01A/
8. 더 필요하면 말씀 주세요. 심전도나 심박, 카메라 등 각종 센서 붙여 볼 수 있을 것 같습니다. 다만 학습은 또 다른 이야기고....


# 기타
