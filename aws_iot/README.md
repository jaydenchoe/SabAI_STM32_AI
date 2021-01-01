# AWS IOT

## Installation

Lecture Link: https://master.d2s409snhlt74e.amplifyapp.com/  (<i>믿을만하지 못함</i>)

### Installation guide

**선행조건**

* python 3.8.4 버전 무조껀 설치되어있어야 한다.

1. /B-L4S5I-IOT01_STSAFE_Provisioning 프로젝트 빌드 후 플래시
2. /B-L4S5I-IOT01_2_Images_SECoreBin 빌드 후 플래시
3. /B-L4S5I-IOT01_2_Images_SBSFU 빌드 후 플래시
4. /B-L4S5I-IOT01_aws_demos 빌드 후 플래시
5. 로그에 STM32 MCU 보드의 디바이스 인증서가 출력 됩니다.
이 인증서는 STSAFE A110의 Root CA로 부터 생성된 X.509 표준 인증서 이며 PEM format으로 출력 되었습니다.
로그에 출력되는 인증서를 복사하여 Device Certificate로 저장 합니다 : stm32-iot-node-cert.pem
stm32-iot-node-cert.pem 파일은 AWS에 연결할 때 Device의 인증서로 등록하여 사용합니다.


## Trouble shooting

### STM32CubeProgramming Mac에서 실행방법

1. 기존 설치된 java를 지운다. (<i>optional</i>)
  *  *  만약 jenv 같은 java version manager가 있다면 그걸 써도 무관.
2. zulu jdk 1.8을 설치한다
   * https://www.azul.com/downloads/zulu-community/?version=java-8-lts&os=macos&architecture=x86-64-bit&package=jdk-fx
3. 실행은 lecture에 나와있는 ```java -jar STM32CubeProgramming``` 으로 하면 안된다.
   * ```./STM32CubeProgramming``` 혹은
   * Launchpad에서 STM32CubeProgramming 으로 실행하면 됩니다.
   
### Lab1 - module 2 STM secure boot 관련 부분 문제 처리

https://master.d2s409snhlt74e.amplifyapp.com/lab1/module2/

https://master.d2s409snhlt74e.amplifyapp.com/lab1/module2/#build-error-fix

이부분을 참조하면 됩니다. SBSFU 까지 빌드 되어서 플래싱되는것 확인.


### Lab 1 - module 3 aws iot application

1. 바이너리 빌드 설치 후
2. 인증서 받아서 저장.

### AWS IOT project 관련 설정부분

* [aws_wifi_config.h](https://github.com/jaydenchoe/SabAI/blob/main/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/Cloud/aws_demos/config_files/aws_wifi_config.h)
  * WIFI 설정 파일


* [aws_customdemo_main.c](https://github.com/jaydenchoe/SabAI/blob/main/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/Cloud/aws_demos/Src/aws_customdemo_main.c)
  * AWS IOT 데모의 메인 부분입니다.
  * [aws_customdemo_main](https://github.com/jaydenchoe/SabAI/blob/daa75e09d48e921298ab7970bf5439b6b133a350/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/Cloud/aws_demos/Src/aws_customdemo_main.c#L66) 함수 부분만 파악하면 될 것 같은 부분입니다.
  * Thread를 두개 생성하는데 센서 리딩하는 Thread(onboardSensorReaderTask), 리딩한 값을 MQTT로 publish하는 Thread(_sensorDataPublisherTask) 이렇게 구성됩니다.

* [aws_customdemo_publisher.c](https://github.com/jaydenchoe/SabAI/blob/main/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/Cloud/aws_demos/Src/aws_customdemo_publisher.c)
  * MQTT로 publish하는 부분

* [st_sensordata_collector.c](https://github.com/jaydenchoe/SabAI/blob/main/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/Cloud/aws_demos/Src/st_sensordata_collector.c)
  * 센서 값을 읽어서 Queue에 넣는 부분
 
### WIFI 설정 ###

WiFi 연결을 설정합니다.
STM32CubeExpansion_Cloud_AWS_V2.0.0/Middlewares/Third_Party/amazon-freertos/demos/include/aws_clientcredential.h
STM32 CubeIDE에서 B-L4S5I-IOT01_aws_dems project를 선택한 후 includes의 ‘amazon-freertos/demos/include’를 확장한 후 aws_clientcredential.h 파일을 선택합니다.
File 속성이 읽기 모드로 되어 있는 경우 쓰기가 가능하도록 변경합니다.

파일 속성을 쓰기 가능 모드로 변경 합니다.
접속할 WiFI 공유기의 SSID와 비밀 번호를 설정합니다.

```
/*
 * @brief Wi-Fi network to join.
 *
 * @todo If you are using Wi-Fi, set this to your network name.
 */
#define clientcredentialWIFI_SSID                    "xxxx"

/*
 * @brief Password needed to join Wi-Fi network.
 * @todo If you are using WPA, set this to your network password.
 */
#define clientcredentialWIFI_PASSWORD                "xxxx"
```
  
### 우리가 해야할 부분

1. [x] AWS IOT에 기기 연동 테스트
2. [x] AWS IOT에 데모 값 MQTT로 전송되는지 확인
3. [x] Queue에 넣을 데이터 structure 구현
4. [ ] 해당 데이터 structure에 맞는 데이터 입력하게끔 serial RX 부분 구현
5. [x] AWS IOT로 올라간 값을 web 에서 읽게 해주기
6. [ ] web page 구성


### 추가 설명

/B-L4S5I-IOT01_aws_demos/Application/User/st_sensordata_collector.c

SEE TODO.s (line 116~120, 155~164)

시리얼로 읽어들인 값을 type, start button status에 담아주시면 됩니다.

MQTT로 올릴떄 topic 이름은 
`/Users/sempark/hackathon/SabAI/aws_iot/STM32CubeExpansion_Cloud_AWS_V2.0.0/Projects/B-L4S5I-IOT01A/Applications/Cloud/aws_demos/Inc/aws_customdemo_globals.h`
파일 내에

```
unsigned char gucSensorTopicName[]="iot/sabai/topic/1";
```
이 부분에 정의해두었습니다.

