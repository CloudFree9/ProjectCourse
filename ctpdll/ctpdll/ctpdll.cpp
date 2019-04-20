// ctpdll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <set>
#include <queue>
#include <Windows.h>
#include "ThostFtdcMdApi.h"
#include "rabbitmqenc.h"
#include "ctpdll.h"
#include <time.h>
#include <stdlib.h>

using namespace std;
static char frontURL[128] = "tcp://180.168.146.187:10010";
static TThostFtdcUserIDType loginID = "109512";
static TThostFtdcPasswordType password = "Wczyyqc9";
static char logDir[128] = ".";
static TThostFtdcBrokerIDType brokerID = "9999";

static char rbmqHost[64] = "localhost";
static int rbmqPort = 5672;
static char rbmqUserName[64] = "guest";
static char rbmqPassword[64] = "guest";
static char rbmqVHost[64] = "/";

class QuotesHandler;
static QuotesHandler *quotesHandler = NULL;

static int requestID = 1;

// Below are status constants
static const int STATUS_ERR = -1;
static const int STATUS_BORN = 0;
static const int STATUS_INIT = 1;
static const int STATUS_CONNECTED = 2;
static const int STATUS_LOGGEDIN = 3;
static const int STATUS_SUBSCRIBED = 4;

static int status = STATUS_BORN;
static set<TThostFtdcInstrumentIDType*> *subscription = new set<TThostFtdcInstrumentIDType*>;
static std::queue<Mt5Quotes_Rec*> *quotesQueue = new std::queue<Mt5Quotes_Rec*>;
static int mt5QuotesQueuePosition = 0;

class QuotesHandler : public CThostFtdcMdSpi
{

private:
	CThostFtdcMdApi *m_mdApi;

public:

	QuotesHandler() : CThostFtdcMdSpi() {
		status = STATUS_INIT;
	}

	void join() {
		m_mdApi->Join();
	}

	int connect()
	{
		if (status != STATUS_INIT) {
			return -1;
		}

		m_mdApi = CThostFtdcMdApi::CreateFtdcMdApi(logDir, true, true);

		if (m_mdApi == NULL) {
			return -2;
		}

		m_mdApi->RegisterSpi(this);
		m_mdApi->RegisterFront(frontURL);
		m_mdApi->Init();
		return 0;
	}

	void OnFrontConnected()
	{
		status = STATUS_CONNECTED;
		std::cout << "Front connected!\n";
		login();
	}

	void OnFrontDisconnected(int nReason)
	{
		std::cerr << "Front end disconnected! Error code is: " << nReason << "\n";
		status = STATUS_ERR;
	}

	int login()
	{
		if (status != STATUS_CONNECTED) {
			std::cerr << "CTP Quotes Login request rejected: must connect to frontend first!\n";
			return -1;
		}

		CThostFtdcReqUserLoginField t = { 0 };
		strcpy(t.BrokerID, brokerID);
		strcpy(t.UserID, loginID);
		strcpy(t.Password, password);

		int try_count = 1;
		int connStat = -1;

		while (try_count++ <= 10)
		{
			connStat = m_mdApi->ReqUserLogin(&t, requestID++);
			if (connStat == 0)
			{
				//				status = STATUS_LOGGEDIN;
				break;
			}
			Sleep(1000);
		}
		return connStat;
	}

	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		if (pRspInfo->ErrorID == 0)
		{
			std::cout << "Login successfully done\n";
			status = STATUS_LOGGEDIN;
		}
		else {
			std::cerr << "Login failed. Error code: " << pRspInfo->ErrorID << ", Error Msg: " << pRspInfo->ErrorMsg << "\n";
		}
		cout.flush();
	}

	int subscribe(char **ppInstrument, int num)
	{
		if (status != STATUS_LOGGEDIN) {
			std::cerr << "CTP Quotes Subscribe request rejected: must Login first!\n";
			return -1;
		}

		int try_count = 1;
		int subStat = -1;

		while (try_count++ <= 10)
		{
			subStat = m_mdApi->SubscribeMarketData(ppInstrument, num);
			if (subStat == 0)
			{
				status = STATUS_SUBSCRIBED;
				break;
			}
			Sleep(1000);
		}
		return subStat;
	}

	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		if (pRspInfo->ErrorID == 0)
		{
			std::cout << "Subscription sussfully done for requestID: "
				<< nRequestID
				<< ", Instrument: "
				<< pSpecificInstrument->InstrumentID
				<< std::endl;

			void *p = new TThostFtdcInstrumentIDType;
			memcpy(p, pSpecificInstrument->InstrumentID, sizeof(pSpecificInstrument->InstrumentID));
			subscription->insert((TThostFtdcInstrumentIDType*)p);
		}
		else {
			std::cerr << "Subscription failed. Error code: "
				<< pRspInfo->ErrorID
				<< ", Error Msg: "
				<< pRspInfo->ErrorMsg
				<< std::endl;
		}

	}

	//Get quotes
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
	{
		Mt5Quotes_Rec rec;
		char queueID[128];

		rec.Close = pDepthMarketData->LastPrice;
		rec.Open = pDepthMarketData->OpenPrice;
		rec.High = pDepthMarketData->HighestPrice;
		rec.Low = pDepthMarketData->LowestPrice;
		rec.Volume = pDepthMarketData->Volume;
		strcpy(rec.instID, pDepthMarketData->InstrumentID);
//		quotesQueue->push(rec);

		struct tm tmval;
		int ymd = atoi(pDepthMarketData->TradingDay);
		tmval.tm_year = ymd / 10000 - 1900;
		tmval.tm_mon = ymd % 10000 / 100 - 1;
		tmval.tm_mday = ymd % 100;

		char tmp[3] = { 0 };
		strncpy(tmp, pDepthMarketData->UpdateTime, 2);
		tmval.tm_hour = atoi(tmp);
		strncpy(tmp, pDepthMarketData->UpdateTime + 3, 2);
		tmval.tm_min = atoi(tmp);
		strncpy(tmp, pDepthMarketData->UpdateTime + 6, 2);
		tmval.tm_sec = atoi(tmp);

		time_t ts = mktime(&tmval);
		rec.timestamp = ts;

		cout << "Instrument: " << rec.instID
			<< ", Open: " << pDepthMarketData->OpenPrice
			<< ", High: " << pDepthMarketData->HighestPrice
			<< ", Low: " << pDepthMarketData->LowestPrice
			<< ", Last: " << pDepthMarketData->LastPrice
			<< ", Tradingdate: " << pDepthMarketData->TradingDay
			<< ", TimeStamp: " << pDepthMarketData->UpdateTime
			<< ", Millisec: " << pDepthMarketData->UpdateMillisec
			<< endl;

		sprintf(queueID, "CTP_QUOTES_%s", rec.instID);
		rbmqPublish("amq.direct", queueID, 1, (char *)(&rec), sizeof(rec));
	}
};

void *ctpGetQuotesHandler()
{
	if (quotesHandler == NULL) {
		quotesHandler = new QuotesHandler;
	}
	return quotesHandler;
}

char* ctpGetBrokerID(size_t *len)
{
	*len = strlen(brokerID);
	return brokerID;
}

size_t ctpSetBrokerID(char *id)
{
	if (id == NULL || id[0] == 0)
		return -1;

	if (strlen(id) >= sizeof(brokerID))
		return -2;

	strcpy(brokerID, id);
	return strlen(brokerID);
}

char* ctpGetFrontURL(size_t *len)
{
	*len = strlen(frontURL);
	return frontURL;
}

size_t ctpSetFrontURL(char *url)
{
	if (url == NULL || url[0] == 0)
		return -1;

	if (strlen(url) >= sizeof(frontURL))
		return -2;

	strcpy(frontURL, url);
	return strlen(frontURL);
}

char* ctpGetLoginID(size_t *len)
{
	*len = strlen(loginID);
	return loginID;
}

size_t ctpSetLoginID(char *name)
{
	if (name == NULL || name[0] == 0)
		return -1;

	if (strlen(name) >= sizeof(loginID))
		return -2;

	strcpy(loginID, name);
	return strlen(loginID);
}

char* ctpGetPassword(size_t *len)
{
	*len = strlen(password);
	return password;
}

size_t ctpSetPassword(char *pwd)
{
	if (pwd == NULL || pwd[0] == 0)
		return -1;

	if (strlen(pwd) >= sizeof(password))
		return -2;

	strcpy(password, pwd);
	return strlen(password);
}

char* ctpGetLogDir(size_t *len)
{
	*len = strlen(logDir);
	return logDir;
}

size_t ctpSetLogDir(char *dir)
{
	if (dir == NULL || dir[0] == 0)
		return -1;

	if (strlen(dir) >= sizeof(logDir))
		return -2;

	strcpy(logDir, dir);
	return strlen(dir);
}


void ctpInit()
{
	int ret = rbmqInit(rbmqHost, rbmqPort, rbmqUserName, rbmqPassword);

	if (!quotesHandler)
	{
		quotesHandler = new QuotesHandler;
	}
	status = STATUS_INIT;
}
	
int ctpConnect()
{
	int try_count = 10;
	if (status != STATUS_INIT) return -1;

	if (rbmqConnect(rbmqVHost) != 0 || rbmqOpenChannel(1) != 0) 
	{
		return -1;
	}

	quotesHandler->connect();

	int ret = 0;
	while (try_count > 0)
	{
		if (status == STATUS_LOGGEDIN || status == STATUS_ERR) break;
		Sleep(2000);
		try_count--;
	}

	if (try_count == 0) return -2;
	if (status == STATUS_ERR) return -3;
	return 0;
}

int ctpSubscribeQuotes(char **ppInstrument, int num)

{
	int ret = 0;
	if (status != STATUS_LOGGEDIN)
		return -1;

	quotesHandler->subscribe(ppInstrument, num);

	for (int i = 0; i < num; i++)
	{
		char queueID[128];
		if (!ppInstrument[i] || !(*ppInstrument[i])) continue;
		sprintf(queueID, "CTP_QUOTES_%s", ppInstrument[i]);
		rbmqSubscribe("amq.direct", queueID, 1, queueID);
//		rbmqPublish("amq.direct", queueID, 1, "Hello", 5);
	}
	return ret;
}

size_t ctpGetQuotes(size_t howmany, Mt5Quotes_Rec target[])
{
	if (howmany <= 0) return 0;
	if (howmany > quotesQueue->size()) howmany = quotesQueue->size();

	for (unsigned int i = 0; i < howmany; i++)
	{
		Mt5Quotes_Rec *head = quotesQueue->front();
		memcpy(&target[i], head, sizeof(Mt5Quotes_Rec));
		quotesQueue->pop();
		delete head;
	}
		
	return howmany;
}

void ctpJoin() {
	quotesHandler->join();
}

void ctpSetMQEnv(char *host, int port, char *user, char *passwd, char *vhost) {
	if (host) strncpy(rbmqHost, host, sizeof(rbmqHost) - 1);
	rbmqPort = port;
	if (user) strncpy(rbmqUserName, user, sizeof(rbmqUserName) - 1);
	if (passwd) strncpy(rbmqPassword, passwd, sizeof(rbmqPassword) - 1);
	if (vhost) strncpy(rbmqVHost, vhost, sizeof(rbmqVHost) - 1);
}

void ctpGetMQEnv(char host[], int *port, char user[], char passwd[], char vhost[]) {
	strcpy(host, rbmqHost);
	strcpy(user, rbmqUserName);
	strcpy(passwd, rbmqPassword);
	strcpy(vhost, rbmqVHost);
	*port = rbmqPort;
}

void internalTest() {
	int ret;
	size_t len = 511;
	char msg[512] = { 0 };
	ret = rbmqInit("localhost", 5672, "guest", "guest");
	ret = rbmqConnect("/");
	ret = rbmqOpenChannel(1);

	ret = rbmqSubscribe("amq.direct", "test1", 1, "test1");
	ret = rbmqSubscribe("amq.direct", "test2", 1, "test2");
	while (1) {
		ret = rbmqNextMessage(msg, &len);
		std::cout << "Message got: " << msg << std::endl;
		memset(msg, 0, sizeof(msg));
		len = sizeof(msg) - 1;
	}
}