/*
 *  logging setup
 */ 



#if !defined(_LOGGING_H)
#define _LOGGING_H

#define LOG_TAG(_TAG) static char tag[] = _TAG

#if defined(ESP_PLATFORM)
#include <esp_log.h>
#include "sdkconfig.h"

#define LOGV(...) ESP_LOGV(tag, __VA_ARGS__)
#define LOGE(...) ESP_LOGE(tag, __VA_ARGS__)
#define LOGD(...) ESP_LOGD(tag, __VA_ARGS__)
#define LOGI(...) ESP_LOGI(tag, __VA_ARGS__)
#define LOGW(...) ESP_LOGW(tag, __VA_ARGS__)

#define LOGV_TAG(_TAG, ...) ESP_LOGV(_TAG, __VA_ARGS__)
#define LOGE_TAG(_TAG, ...) ESP_LOGE(_TAG, __VA_ARGS__)
#define LOGD_TAG(_TAG, ...) ESP_LOGD(_TAG, __VA_ARGS__)
#define LOGI_TAG(_TAG, ...) ESP_LOGI(_TAG, __VA_ARGS__)
#define LOGW_TAG(_TAG, ...) ESP_LOGW(_TAG, __VA_ARGS__)


#else
extern void scde_log(char *tag, char type, char *fmt, ...);
#define LOGV(...) scde_log(tag, 'V', __VA_ARGS__)
#define LOGE(...) scde_log(tag, 'E', __VA_ARGS__)
#define LOGD(...) scde_log(tag, 'D', __VA_ARGS__)
#define LOGI(...) scde_log(tag, 'I', __VA_ARGS__)
#define LOGW(...) scde_log(tag, 'W', __VA_ARGS__)

#define LOGV_TAG(_TAG, ...) scde_log(_TAG, 'V', __VA_ARGS__)
#define LOGE_TAG(_TAG, ...) scde_log(_TAG, 'E', __VA_ARGS__)
#define LOGD_TAG(_TAG, ...) scde_log(_TAG, 'D', __VA_ARGS__)
#define LOGI_TAG(_TAG, ...) scde_log(_TAG, 'I', __VA_ARGS__)
#define LOGW_TAG(_TAG, ...) scde_log(_TAG, 'W', __VA_ARGS__)

#endif /* ESP_PLATFORM */

#endif /* _LOGGING_H */
