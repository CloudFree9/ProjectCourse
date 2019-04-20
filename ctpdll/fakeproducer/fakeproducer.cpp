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
static char queueID3[128] = { 0 };
static char queueID4[128] = { 0 };
static char queueID5[128] = { 0 };
static char queueID6[128] = { 0 };

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
	sprintf_s(queueID3, "CTP_QUOTES_%s", "IF1905");
	sprintf_s(queueID4, "CTP_QUOTES_%s", "TF1906");
	sprintf_s(queueID5, "CTP_QUOTES_%s", "hc1910");
	sprintf_s(queueID6, "CTP_QUOTES_%s", "rb1910");

	double ceil1 = 49430 * 1.1, ceil2 = 285.6 * 1.1, ceil3 = 4130.2 * 1.1, ceil4 = 98.725 * 1.1, ceil5 = 3751 * 1.1, ceil6 = 3812 * 1.1;
	double floor1 = 49430 * 0.9, floor2 = 285.6 * 0.9, floor3 = 4130.2 * 0.9, floor4 = 98.725 * 0.9, floor5 = 3751 * 0.9, floor6 = 3812 * 0.9;
	srand(32767);

	struct Mt5Quotes_Rec msg1 = { 0 }, msg2 = { 0 }, msg3 = { 0 }, msg4 = { 0 }, msg5 = { 0 }, msg6 = { 0 };
	strcpy_s(msg1.instID, "cu1905");
	msg1.Open = msg1.Close = msg1.Low = msg1.High = 49430;
	msg1.Volume = 100;
	
	strcpy_s(msg2.instID, "au1906");
	msg2.Open = msg2.Close = msg2.Low = msg2.High = 285.6;
	msg2.Volume = 100;

	strcpy_s(msg3.instID, "IF1905");
	msg3.Open = msg3.Close = msg3.Low = msg3.High = 4130.2;
	msg3.Volume = 100;

	strcpy_s(msg4.instID, "TF1906");
	msg4.Open = msg4.Close = msg4.Low = msg4.High = 98.725;
	msg4.Volume = 100;

	strcpy_s(msg5.instID, "hc1910");
	msg5.Open = msg5.Close = msg5.Low = msg5.High = 3751;
	msg5.Volume = 100;

	strcpy_s(msg6.instID, "rb1910");
	msg6.Open = msg6.Close = msg6.Low = msg6.High = 3812;
	msg6.Volume = 100;

	std::cout << "Hello World!\n";

	while (1) {

		time_t now;
		time(&now);
		struct tm pTime;
		localtime_s(&pTime, &now);
		char sTime[80] = { 0 };
		sprintf_s(sTime, "%04d-%02d-%02d %02d:%02d:%02d", pTime.tm_year + 1900, pTime.tm_mon + 1, pTime.tm_mday, pTime.tm_hour, pTime.tm_min, pTime.tm_sec);

		double i1 = rand(), i2 = rand(), i3 = rand(), i4 = rand(), i5 = rand(), i6 = rand();
		double incre_percent1 = (i1 - 16384)/ 1638400;
		double incre_percent2 = (i2 - 16384) / 1638400;
		double incre_percent3 = (i3 - 16384) / 1638400;
		double incre_percent4 = (i4 - 16384) / 1638400;
		double incre_percent5 = (i5 - 16384) / 1638400;
		double incre_percent6 = (i6 - 16384) / 1638400;

		double price1 = msg1.Close * (1 + incre_percent1);
		double price2 = msg2.Close * (1 + incre_percent2);
		double price3 = msg3.Close * (1 + incre_percent3);
		double price4 = msg4.Close * (1 + incre_percent4);
		double price5 = msg5.Close * (1 + incre_percent5);
		double price6 = msg6.Close * (1 + incre_percent6);

		if (price1 > ceil1) price1 = ceil1;
		if (price2 > ceil2) price2 = ceil2;
		if (price3 > ceil3) price3 = ceil3;
		if (price4 > ceil4) price4 = ceil4;
		if (price5 > ceil5) price5 = ceil5;
		if (price6 > ceil6) price6 = ceil6;
		if (price1 < floor1) price1 = floor1;
		if (price2 < floor2) price2 = floor2;
		if (price3 < floor3) price3 = floor3;
		if (price4 < floor4) price4 = floor4;
		if (price5 < floor5) price5 = floor5;
		if (price6 < floor6) price6 = floor6;

		msg1.Open = msg1.High = msg1.Low = msg1.Close = price1;
		msg2.Open = msg2.High = msg2.Low = msg2.Close = price2;
		msg3.Open = msg3.High = msg3.Low = msg3.Close = price3;
		msg4.Open = msg4.High = msg4.Low = msg4.Close = price4;
		msg5.Open = msg5.High = msg5.Low = msg5.Close = price5;
		msg6.Open = msg6.High = msg6.Low = msg6.Close = price6;
		msg1.timestamp = msg2.timestamp = msg3.timestamp = msg4.timestamp = msg5.timestamp = msg6.timestamp = now;
		rbmqPublish((char*)"amq.direct", queueID1, 1, (char *)(&msg1), sizeof(msg1));
		rbmqPublish((char *)"amq.direct", queueID2, 1, (char *)(&msg2), sizeof(msg2));
		rbmqPublish((char*)"amq.direct", queueID3, 1, (char *)(&msg3), sizeof(msg3));
		rbmqPublish((char *)"amq.direct", queueID4, 1, (char *)(&msg4), sizeof(msg4));
		rbmqPublish((char*)"amq.direct", queueID5, 1, (char *)(&msg5), sizeof(msg5));
		rbmqPublish((char *)"amq.direct", queueID6, 1, (char *)(&msg6), sizeof(msg6));

		// Here to publish the msg to rabbitmq
		Sleep(1000);
		std::cout << "Tick: " << msg1.instID << ", Close:" << msg1.Close << ", Time:" << sTime << "\n";
		std::cout << "Tick: " << msg2.instID << ", Close:" << msg2.Close << ", Time:" << sTime << "\n";
		std::cout << "Tick: " << msg3.instID << ", Close:" << msg3.Close << ", Time:" << sTime << "\n";
		std::cout << "Tick: " << msg4.instID << ", Close:" << msg4.Close << ", Time:" << sTime << "\n";
		std::cout << "Tick: " << msg5.instID << ", Close:" << msg5.Close << ", Time:" << sTime << "\n";
		std::cout << "Tick: " << msg6.instID << ", Close:" << msg6.Close << ", Time:" << sTime << "\n";
	}
}

