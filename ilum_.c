#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CS_MCP3208	8
#define SPI_CHANNEL	0
#define SPI_SPEED	1000000

#define LED_PIN_1		4
#define LED_PIN_2		17
#define LED_PIN_3		18
#define LED_PIN_4		27
#define LED_PIN_5		22
#define LED_PIN_6		23
#define LED_PIN_7		24
#define LED_PIN_8		25
#define LED_PIN_9		6
#define LED_PIN_10		12
#define LED_PIN_11		13
#define LED_PIN_12		16
#define LED_PIN_13		19
#define LED_PIN_14		20
#define LED_PIN_15		26
#define LED_PIN_16		21
#define MAX_LED_NUM	16

// LED_PIN 배열 선언
const int LedPinTable[16] = {
LED_PIN_1, LED_PIN_2, LED_PIN_3, 
LED_PIN_4, LED_PIN_5, LED_PIN_6,
LED_PIN_7, LED_PIN_8, LED_PIN_9,
LED_PIN_10, LED_PIN_11, LED_PIN_12,
LED_PIN_13, LED_PIN_14, LED_PIN_15,
LED_PIN_16};

//조도센서에서 받아온 아날로그 신호를 디지털 신호로 변환.
int ReadMcp3208ADC(unsigned char adcChannel)
{
	unsigned char buff[3];
	int nAdcValue = 0;

	buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
	buff[1] = ((adcChannel & 0x07) << 6);
	buff[2] = 0x00;

	digitalWrite(CS_MCP3208, 0);
	wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);

	buff[1] = 0x0F & buff[1];
	nAdcValue = (buff[1] << 8) | buff[2];
	digitalWrite(CS_MCP3208, 1);
	return nAdcValue;
}

//LED 전체 제어(꺼짐)
void digitalWrite_LOW(){
	int i =0;
	for (i = 0; i < MAX_LED_NUM; i++)
			{
				digitalWrite(LedPinTable[i],LOW);
			}
}
//LED 전체 제어(켜짐)
void digitalWrite_HIGH(){
	int i = 0;
	for (i = 0; i < MAX_LED_NUM; i++)
			{
				digitalWrite(LedPinTable[i],HIGH);
			}
}

int main(void)
{
	int i=0;
	//wiringPi 초기화
	if (wiringPiSetupGpio() == -1)
		return 1;
	
	for(i=0;i<MAX_LED_NUM;i++)
	{
		//pin mode 설정(입력한 핀 번호 출력핀으로 사용)
		pinMode(LedPinTable[i],OUTPUT); 
		//LED 전체 꺼짐
		digitalWrite(LedPinTable[i],LOW);
	}
	//MCP3208에 몇번 채널을 읽을지 골라줄 변수 선언
	int nPhotoCellChannel = 1;
	//MCP3208을 통해 얻어낸 값을 저장할 변수 선언
	int nPhotoCellValue = 0;

	//wiringPi SPI 통신채널과 속도 설정
	if (wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)
	{
		fprintf(stdout, "wiringPiSPISetup Failed: %s\n", strerror(errno));
		return 1;
	}
	//결과값을 받을 핀을 출력핀으로 설정.
	pinMode(CS_MCP3208, OUTPUT);

	while (1)
	{	
		//MCP3208을 통해 조도센서의 값을 받아옴.
		nPhotoCellValue = ReadMcp3208ADC(nPhotoCellChannel);
		//조건문 조도센서에서 받아온 값이 200 이상이면(밝으면) LED 꺼짐
		if (nPhotoCellValue > 200)
		{
			digitalWrite_LOW();
		}
		//값이 50 이상이면(완전 어둡지않은 상태) 켜져있는 LED를 끄고 , LED 절반 켜짐
		else if (nPhotoCellValue > 50)
		{
			digitalWrite_LOW();
			for (i = 0; i < MAX_LED_NUM/2; i++)
			{
				digitalWrite(LedPinTable[i],HIGH);
			}
		}
		//50이하의 값인 경우(어두울 때) LED 모두 켜짐 
		else{
			digitalWrite_HIGH();
		}
	}
	return 0;
}