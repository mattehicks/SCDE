#ifndef SCDED_PLATFORM_H
#define SCDED_PLATFORM_H



void SCDED_PlatformPlatLock();
void SCDED_PlatformUnlock();
void SCDED_PlatformReconCb(void *arg, int8_t err);
void SCDED_PlatformDisconCb(void *arg);
void SCDED_PlatformRecvCb(void *arg, char *data, unsigned short len);
void SCDED_PlatformSentCb(void *arg);
void SCDED_PlatformDefaultSlotConnCb(void *arg);
void SCDED_PlatformETXSlotConnCb(void *arg);
int SCDED_PlatformSendData(ConnTypePtr conn, char *buff, int len);
void SCDED_PlatformDisconnect(ConnTypePtr conn);
void SCDED_PlatformDisableTimeout(ConnTypePtr conn);
void SCDED_PlatformInit(int port, int maxConnCt);



#endif
