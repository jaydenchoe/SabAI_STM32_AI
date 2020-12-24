# AWS IOT

## Installation

Lecture Link: https://master.d2s409snhlt74e.amplifyapp.com/  (<i>믿을만하지 못함</i>)

## Trouble shooting

### STM32CubeProgramming Mac에서 실행방법

1. 기존 설치된 java를 지운다. (<i>optional</i>)
   * 만약 jenv 같은 java version manager가 있다면 그걸 써도 무관.
2. zulu jdk 1.8을 설치한다
   * https://www.azul.com/downloads/zulu-community/?version=java-8-lts&os=macos&architecture=x86-64-bit&package=jdk-fx
3. 실행은 lecture에 나와있는 ```java -jar STM32CubeProgramming``` 으로 하면 안된다.
   * ```./STM32CubeProgramming``` 혹은
   * Launchpad에서 STM32CubeProgramming 으로 실행하면 됩니다.
   
### Lab1 - module 2 STM secure boot 관련 부분 문제 처리

https://master.d2s409snhlt74e.amplifyapp.com/lab1/module2/
   
1. pythonn version 은 3.8.4를 쓰는걸로 (진행중..확인중)
2. 압축을 풀면 sh 스크립트가 실행파일이 아닙...
   * ```chomd -R 755 prebuild.sh``` 같은 처리가 필요.
3. python 관련 라이브러리 필요
   * ```pip3 install pycryptodomex```
   * ```pip3 install pycryptodomex```
   * ```pip3 install ecdsa```
   * ```pip3 install numpy```
   * ```pip3 install pyelftools```

이러면 prebuild script 실행완료 됩니다 ㅠ


