
#include "com_cidana_testcustomdatasource_SurfaceLayer.h"

#include <stdio.h>

#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <android/native_window_jni.h>

//#include <media/stagefright/MediaExtractor>
#include "dp.h"

//#include <gui/Surface.h>

//#define TEST_ACODEC 1

#define LOGE DP
#define LOGI DP

static char * g_url;

typedef struct
{
	jobject   obj;
	jmethodID mid;
} Java_Callback_t;

typedef struct
{
	Java_Callback_t onReceiveMessage;
} Java_Callback_List_t;
Java_Callback_List_t g_javaCallbackList;

struct fields_t 
{
    jfieldID    context;
    jfieldID    surface;
    /* actually in android.view.Surface XXX */
    jfieldID    surface_native;

    jmethodID   post_event;
};
static fields_t fields;

JavaVM* g_JavaVM = NULL;

static bool g_bExitRenderThread;
static ANativeWindow * g_nativeWindow;
static pthread_t g_thread;

static const int MSG_VIDEO_SIZE_CHANGED = 0;
static const int MSG_PLAYBACK_END = 1;

static void RenderTest(ANativeWindow * surface);
static void FindCallbackFunctions(JNIEnv * env, jobject obj);
static void SendMessage(int msg, int arg1, int arg2);

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	jint nResult = JNI_VERSION_1_6;

	g_JavaVM = vm;
	return nResult;
}

void JNICALL Java_com_cidana_testcustomdatasource_SurfaceLayer_native_1init(JNIEnv * env, jclass thiz)
{
	LOGE("native_1init in\n");

//	setsu();
//	int ret = system("chmod 777 /dev/pmem_adsp");
//	LOGE("system call failed, ret = %d\n", ret);

	jclass clazz;

    clazz = env->FindClass("com/cidana/testcustomdatasource/SurfaceLayer");
    if (clazz == NULL) 
	{
    //    jniThrowException(env, "java/lang/RuntimeException", "Can't find com/app/surfacemark/SurfaceLayer");
		LOGE("Can't find com/cidana/testcustomdatasource/SurfaceLayer");
        return;
    }

	fields.surface = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");
    if (fields.surface == NULL) 
	{
     //   jniThrowException(env, "java/lang/RuntimeException", "Can't find SurfaceLayer.mSurface");
		LOGE("Can't find SurfaceLayer.mSurface");
        return;
    }

	jclass surface = env->FindClass("android/view/Surface");
    if (surface == NULL) {
    //    jniThrowException(env, "java/lang/RuntimeException", "Can't find android/view/Surface");
		LOGE("Can't find android/view/Surface");
        return;
    }

	fields.surface_native = env->GetFieldID(surface, "mNativeSurface", "I");
    if (fields.surface_native == NULL) {
   //     jniThrowException(env, "java/lang/RuntimeException", "Can't find Surface.mSurface");
		LOGE("Can't find Surface.mSurface");
        return;
    }

	LOGI("native_1init out - succssfully\n");
}

void JNICALL Java_com_cidana_testcustomdatasource_SurfaceLayer_native_1setup(JNIEnv * env, jobject thiz, jobject)
{
	LOGE("native_1setup in\n");
	FindCallbackFunctions(env, thiz);
}

void JNICALL Java_com_cidana_testcustomdatasource_SurfaceLayer_native_1start(JNIEnv * env, jobject thiz, jstring jUrl)
{
	LOGE("native_1start in\n");
	const char * url = env->GetStringUTFChars(jUrl, NULL);
	if (url == NULL)
	{
		LOGE("null url.");
		return;
	}
	g_url = strdup(url);
	env->ReleaseStringUTFChars(jUrl, url);

	jobject jsurface = env->GetObjectField(thiz, fields.surface);
	//Surface * surface = (Surface *)env->GetIntField(jsurface, fields.surface_native);
	ANativeWindow * surface = (ANativeWindow *)ANativeWindow_fromSurface(env, jsurface);

	RenderTest(surface);
}

void JNICALL Java_com_cidana_testcustomdatasource_SurfaceLayer_native_1stop(JNIEnv * env, jobject thiz)
{
	LOGE("native_1stop in\n");
	g_bExitRenderThread = true;
	usleep(100*1000);
	if (g_url != NULL)
	{
		free(g_url);
		g_url = NULL;
	}
}


unsigned long long GetTickCount(void)
{
	timeval curTime = { 0, 0 }; 
	gettimeofday(&curTime, NULL);
	unsigned long long nowTime = curTime.tv_sec * 1000 + curTime.tv_usec / 1000;
	return nowTime;
}

#include "OMXHarness.h"

void * RenderThread(void * param)
{
	ANativeWindow * surfaceTextureClient = (ANativeWindow *)param;

	Harness * h = new Harness;
	if (h->initCheck() != 0)
	{
		delete h;
		return 0;
	}

	const char * url = g_url;

	if (h->Open(url, surfaceTextureClient) != 0)
	{
		delete h;
		return 0;
	}

	while (!g_bExitRenderThread)
	{
		int ret = h->ShowOneFrame();
		if (Harness::MSG_VIDEO_SIZE_CHANGED == ret)
		{
			int width, height;
			h->QueryVideoSize(width, height);

			SendMessage(MSG_VIDEO_SIZE_CHANGED, width, height);

		}
		else if (ret != 0)
		{
			SendMessage(MSG_PLAYBACK_END, 0, 0);
			break;
		}
	}


	h->Close();
	delete h;
	ANativeWindow_release(surfaceTextureClient);
	LOGI("Exit play loop");
	return 0;
}

void RenderTest(ANativeWindow * surface)
{
	LOGE("RenderTest in\n");

	LOGE("RenderTest - Surface ptr %p\n", surface);

	g_bExitRenderThread = false;
	if (0 != pthread_create(&g_thread, NULL, RenderThread, surface))
	{
		LOGE("RenderTest - render thread init failed\n");
		return;
	}
}

void SetupCallbackFunction(JNIEnv* env, jobject obj, Java_Callback_t* pFunc, const char* pCBFuncName, const char* pSig)
{
	if(NULL == env || NULL == pFunc || NULL == pCBFuncName || NULL == pSig)
	{
		return;
	}

	pFunc->obj = env->NewGlobalRef(obj);
	jclass cls = env->GetObjectClass(obj);
	pFunc->mid = env->GetMethodID(cls, pCBFuncName, pSig);
}

void FindCallbackFunctions(JNIEnv * env, jobject obj)
{
	SetupCallbackFunction(env, obj, &g_javaCallbackList.onReceiveMessage, "onReceiveMessage", "(III)V");
}

void SendMessage(int msg, int arg1, int arg2)
{
	JNIEnv* env;
	bool needDetach = false;
	g_JavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
	if (env == NULL)
	{
		needDetach = true;
		int status = g_JavaVM->AttachCurrentThread(&env, NULL);
		if(status < 0)
		{
			DP("Attach Current thread failed!!");
		}
	}

	env->MonitorEnter(g_javaCallbackList.onReceiveMessage.obj);
	env->CallVoidMethod(g_javaCallbackList.onReceiveMessage.obj, g_javaCallbackList.onReceiveMessage.mid, msg, arg1, arg2);
	env->MonitorExit(g_javaCallbackList.onReceiveMessage.obj);

	if (needDetach)
	{
		g_JavaVM->DetachCurrentThread();
	}
}
