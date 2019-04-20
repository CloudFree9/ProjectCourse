//#include "pch.h"
#include <iostream>
#include "rabbitmqenc.h"
#include "ctpdll.h"
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

static char rbmqHost[64] = "localhost";
static int rbmqPort = 5672;
static char rbmqUserName[64] = "guest";
static char rbmqPassword[64] = "guest";
static char rbmqVHost[64] = "/";
static char queueID1[128] = { 0 };
static char queueID2[128] = { 0 };

int main()
{

	int ret = rbmqInit(rbmqHost, rbmqPort, rbmqUserName, rbmqPassword);
	if (ret != 0) {
		std::cout << "OOPS, rabbit MQ connection failed: host:" << rbmqHost << ", port:"
			<< rbmqPort << ", username:" << rbmqUserName << ", password:" << rbmqPassword << "\n";
		exit(1);
	}

	ret = rbmqConnect((char *)"/");
	ret = rbmqOpenChannel(1);

	sprintf_s(queueID1, "CTP_QUOTES_%s", "cu1905");
	sprintf_s(queueID2, "CTP_QUOTES_%s", "au1906");

	double ceil1 = 49430 * 1.1, ceil2 = 285.6 * 1.1;
	double floor1 = 49430 * 0.9, floor2 = 285.6 * 0.9;
	srand(32767);

	struct Mt5Quotes_Rec msg1 = { 0 }, msg2 = { 0 };
	strcpy_s(msg1.instID, "cu1905");
	msg1.Open = msg1.Close = msg1.Low = msg1.High = 49430;
	msg1.Volume = 100;
	
	strcpy_s(msg2.instID, "au1906");
	msg2.Open = msg2.Close = msg2.Low = msg2.High = 285.6;
	msg2.Volume = 100;
	std::cout << "Hello World!\n";

	while (1) {

		time_t now;
		time(&now);
		struct tm pTime;
		localtime_s(&pTime, &now);
		char sTime[80] = { 0 };
		sprintf_s(sTime, "%04d-%02d-%02d %02d:%02d:%02d", pTime.tm_year + 1900, pTime.tm_mon + 1, pTime.tm_mday, pTime.tm_hour, pTime.tm_min, pTime.tm_sec);

		double i1 = rand(), i2 = rand();
		double incre_percent1 = (i1 - 16384)/ 1638400;
		double incre_percent2 = (i2 - 16384) / 1638400;

		double price1 = msg1.Close * (1 + incre_percent1);
		double price2 = msg2.Close * (1 + incre_percent2);
		if (price1 > ceil1) price1 = ceil1;
		if (price2 > ceil2) price2 = ceil2;
		if (price1 < floor1) price1 = floor1;
		if (price2 < floor2) price2 = floor2;

		msg1.Open = msg1.High = msg1.Low = msg1.Close = price1;
		msg2.Open = msg2.High = msg2.Low = msg2.Close = price2;
		msg1.timestamp = msg2.timestamp = now;
		rbmqPublish((char*)"amq.direct", queueID1, 1, (char *)(&msg1), sizeof(msg1));
		rbmqPublish((char *)"amq.direct", queueID2, 1, (char *)(&msg2), sizeof(msg2));

		// Here to publish the msg to rabbitmq
		Sleep(1000);
		std::cout << "Tick: " << msg1.instID << ", Close:" << msg1.Close << ", Time:" << sTime << "\n";
		std::cout << "Tick: " << msg2.instID << ", Close:" << msg2.Close << ", Time:" << sTime << "\n";
	}
}

