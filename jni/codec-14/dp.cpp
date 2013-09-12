#include "dp.h"
#include <stdlib.h>
#include <stdio.h>

#include <android/log.h>


#define LOG_TAG "CIDANA_SDK"

void DP(const char * szFormat, ...)
{
	char szBuffer[256]; 
	va_list vl; 
	va_start(vl, szFormat);

	memset(szBuffer, 0, sizeof(szBuffer));
	
	vsnprintf(szBuffer,255,szFormat,vl);
	szBuffer[255]=0;
	
	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s",(const char*)szBuffer);
	va_end(vl);
}
