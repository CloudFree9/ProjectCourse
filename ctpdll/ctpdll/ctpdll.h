#ifdef CTPDLL_EXPORTS
#define CTPDLL_API __declspec(dllexport)
#else
#define CTPDLL_API __declspec(dllimport)
#endif

#pragma pack(push)
#pragma pack(1)

struct Mt5Quotes_Rec 
{
	char instID[31];
	double	Open;
	double	High;
	double	Low;
	int Volume;
	double	Close;
	time_t timestamp;
};

#pragma pack(pop)

extern "C" {

	CTPDLL_API char* ctpGetBrokerID(size_t *len);
	CTPDLL_API size_t ctpSetBrokerID(char *id);
	CTPDLL_API char* ctpGetFrontURL(size_t *len);
	CTPDLL_API size_t ctpSetFrontURL(char *url);
	CTPDLL_API char* ctpGetLoginID(size_t *len);
	CTPDLL_API size_t ctpSetLoginID(char *name);
	CTPDLL_API char* ctpGetPassword(size_t *len);
	CTPDLL_API size_t ctpSetPassword(char *pwd);
	CTPDLL_API char* ctpGetLogDir(size_t *len);
	CTPDLL_API size_t ctpSetLogDir(char *dir);
	CTPDLL_API void *ctpGetQuotesHandler();

	CTPDLL_API void ctpInit();
	CTPDLL_API int ctpConnect();
	CTPDLL_API int ctpSubscribeQuotes(char **ppInstrument, int num);
	CTPDLL_API int ctpGetQuotes(int howmany, Mt5Quotes_Rec target[]);
	CTPDLL_API void ctpJoin();

	CTPDLL_API void ctpSetMQEnv(char *host, int port, char *user, char *passwd, char *vhost);
	CTPDLL_API void ctpGetMQEnv(char host[], int *port, char user[], char passwd[], char vhost[]);
	CTPDLL_API void internalTest();
}