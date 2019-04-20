#ifdef RABBITMQENC_EXPORTS
#define RABBITMQENC_API __declspec(dllexport)
#else
#define RABBITMQENC_API __declspec(dllimport)
#endif

extern "C" {
	RABBITMQENC_API int rbmqInit(char *host, int port, char *username, char *password);
	RABBITMQENC_API int rbmqGetPort();
	RABBITMQENC_API unsigned char *rbmqGetHostname();
	RABBITMQENC_API int rbmqConnect(char *vhost);
	RABBITMQENC_API int rbmqOpenChannel(int no);
	RABBITMQENC_API int rbmqSubscribe(char *exchange, char *key, int channel, char *qname);
	RABBITMQENC_API int rbmqPublish(char *exchange, char *key, int channel, char *msg, size_t len);
	RABBITMQENC_API int rbmqNextMessage(char msg[], size_t *len);
	RABBITMQENC_API int rbmqCloseChannel();
	RABBITMQENC_API int rbmqCloseConnection();
}