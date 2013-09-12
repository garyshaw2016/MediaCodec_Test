#ifndef _CI_HWDEC_WRAPPER_DEBUG_H
#define _CI_HWDEC_WRAPPER_DEBUG_H

#ifndef ENABLEDBG
#define ENABLEDBG 1
#endif

extern char *g_dbg_picture_string[];

// enable the verbose output
#ifndef PRINT_VERBOSE
#define PRINT_VERBOSE 0
#endif

// enable the output to the standard output
#ifndef PRINT_DEBUGINFO
#define PRINT_DEBUGINFO 1
#endif

#if defined(__GNUC__) && !defined(NDEBUG)
#define NDEBUG
#endif

#define RELEASE_VERSION 0

// If 1, only parse the stream, do not invoke the hwdec api
#define TEST_PARSER_ONLY    0

// If 1, log the hwdec decoding time
#define LOG_HWDEC_TIME 0

// If 1, dump the input stream to file
#define DBGDUMP 0

#if PLATFORM_IMX53 || PLATFORM_RENESAS || PLATFORM_OMAP4 || PLATFORM_IMX5X || PLATFORM_VPU || PLATFORM_GENERIC
#define OMX_DECODER 1
#else
#define OMX_DECODER 0
#endif

// dump file definition
#if DBGDUMP
#include <stdio.h>
extern FILE *g_dbg_dump;
#if OMX_DECODER
#define DBG_DUMP_FILE "/data/player/hwdec_dump.3gp"
#endif
#endif

#if PRINT_DEBUGINFO           /* output to the standard io */
#if OMX_DECODER // For Android
#define LOG_NDEBUG 0
#define LOG_TAG "CIVDec"
#include <utils/Log.h>  // for android

#define CITraceE(format, arg...) {LOGE((format), ##arg);}
#if RELEASE_VERSION
#define CITraceW(format, arg...)
#define CITraceI(format, arg...)
#define CITraceV(format, arg...)
#else
#define CITraceW(format, arg...) {LOGW((format), ##arg);}
#define CITraceI(format, arg...) {LOGI((format), ##arg);}
#if PRINT_VERBOSE
#define CITraceV(format, arg...) {LOGV((format), ##arg);}
#else
#define CITraceV(format, arg...)
#endif // PRINT_VERBOSE
#endif // RELEASE_VERSION
#else  // For WinCE
#define CITraceE(format, ...) {printf("[CIV-E]: "format"\n", ##__VA_ARGS__);}
#if RELEASE_VERSION
#define CITraceW(format, ...)
#define CITraceI(format, ...)
#define CITraceV(format, ...)
#else
#define CITraceW(format, ...) {printf("[CIV-W]: "format"\n", ##__VA_ARGS__);}
#define CITraceI(format, ...) {printf("[CIV-I]: "format"\n", ##__VA_ARGS__);}
#if PRINT_VERBOSE
#define CITraceV(format, ...) {printf("[CIV-V]: "); printf(format"\n", ##__VA_ARGS__);}
#else
#define CITraceV(format, ...)
#endif // PRINT_VERBOSE
#endif // RELEASE_VERSION
#endif // OMX_DECODER
#else  /* disable the output */
#define CITraceW(format, ...)
#define CITraceE(format, ...)
#define CITraceI(format, ...)
#define CITraceV(format, ...)
#endif

#if LOG_HWDEC_TIME
#define DECLARE_TIMER()  CI_U32 time1
#if defined(WIN32) || defined(UNDER_CE)

#define START_TIMER()                           \
    do{                                         \
        time1 = GetTickCount();                 \
    }while(0)
#define END_TIMER()                                         \
    do{                                                     \
        CITraceV("Hwdec take %d", GetTickCount()-time1);    \
    }while(0)

#elif defined(__GNUC__)
#include <sys/time.h>
#define START_TIMER()                                   \
    do{                                                 \
        struct timeval tv;                              \
        gettimeofday(&tv, 0);                           \
        time1 = (CI_U32)(tv.tv_sec * 1000 + tv.tv_usec / 1000);   \
    }while(0)
#define END_TIMER()                                                     \
    do{                                                                 \
        struct timeval tv;                                              \
        gettimeofday(&tv, 0);                                          \
        CITraceV("Hwdec take %d",                                       \
                 (tv.tv_sec * 1000 + tv.tv_usec / 1000)-time1);         \
    }while(0)
#endif // defined(WIN32) || defined(UNDER_CE)

#else

#define DECLARE_TIMER()
#define START_TIMER()
#define END_TIMER()

#endif // LOG_HWDEC_TIME

#endif
