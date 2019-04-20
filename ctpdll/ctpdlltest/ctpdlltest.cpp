// ctpdlltest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "rabbitmqenc.h"
#include "ctpdll.h"

int _tmain(int argc, _TCHAR* argv[])
{

	char *instruments[] = {  "cu1905", "au1906" };
	std::cout << "Hello, CTP! Sizeof Mt5Quotes_Rec is " << sizeof(Mt5Quotes_Rec) << std::endl;

	std::cout << "Sizeof int is " << sizeof(int)
		<< ", Sizeof long is " << sizeof(long)
		<< ", Sizeof double is " << sizeof(double)
		<< std::endl;

	ctpSetMQEnv("localhost", 5672, "guest", "guest", "/");
	ctpInit();

	if (ctpConnect() != 0) {
		std::cerr << "Connection failed, bye." << std::endl;
		return -1;
	}

	ctpSubscribeQuotes(instruments, sizeof(instruments) / sizeof(char *));

	ctpJoin();
	return 0;
}

