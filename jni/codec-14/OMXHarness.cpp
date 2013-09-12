
#include "OMXHarness.h"

#include <media/stagefright/DataSource.h>
//#include <media/stagefright/MediaDebug.h>  // CHECK_EQ
#include <media/stagefright/MediaSource.h>  // OMXCodec uses this interface to pull data
#include <media/stagefright/MetaData.h>
#include <media/stagefright/OMXCodec.h>
#include <media/stagefright/MediaExtractor.h>

#include <media/stagefright/OMXClient.h>
#include <media/stagefright/TimeSource.h>
#include <SoftwareRenderer.h>

#include <cutils/properties.h>  // for get_property


#include "dp.h"

#include <media/IOMX.h>
#include <utils/threads.h>

#include <OMX_Component.h>


//#include <DummyAudioSource.h>
#include <CidanaMediaSource.h>

#define LOGI DP
#define LOGE DP

using namespace android;

class CodecImpl
{
public:
	CodecImpl() : mInitCheck(-1), mTimeSource(NULL), mWidth(0), mHeight(0), mSoftwareRenderer(NULL)
	{
		mInitCheck = initOMX();
		LoadSystemInfo();
	}

	~CodecImpl()
	{
		if (mPlatform != NULL)
		{
			free(mPlatform);
			mPlatform = NULL;
		}

		if (mDevice != NULL)
		{
			free(mDevice);
			mDevice = NULL;
		}
	}

	int initCheck() const;

	int Open(const char * fileName, sp<ANativeWindow> surface);
	int ShowOneFrame(void);
	int QueryVideoSize(int & width, int & height);
	int Close(void);

private:
	int mInitCheck;
	sp<IOMX> mOMX;
	int initOMX();

	void LoadSystemInfo();  // not necessary any more
	void UpdateVideoSize(sp<MetaData> meta);
	int InitRenderer(sp<MetaData> meta);

	CodecImpl(const Harness &);
	CodecImpl &operator=(const Harness &);

private:  // for "open - show - close"
	bool mUseRemoteRender;
	sp<ANativeWindow> mNativeWindow;
	SoftwareRenderer * mSoftwareRenderer;
	sp<MediaSource> mCodecVideo;
	SystemTimeSource * mTimeSource;
	int mWidth;
	int mHeight;

	bool mIsFormatChangedReceived;
	// System info
	char * mPlatform;  // e.g. OMAP4
	char * mDevice;  // e.g. 'spider' for MOTO RAZR
	int mSDK;  // equals to Android API level

	CCidanaMediaSource* pPrivateSourceVideo;
	int mFrameCnt;
};

Harness::Harness()
{
	mCodecImpl = new CodecImpl();
}

Harness::~Harness()
{
	delete mCodecImpl;
}

int Harness::initCheck() const
{
	return mCodecImpl->initCheck();
}

int Harness::Open(const char * fileName, ANativeWindow * surface)
{
	return mCodecImpl->Open(fileName, surface);
}

int Harness::ShowOneFrame(void)
{
	return mCodecImpl->ShowOneFrame();
}

int Harness::QueryVideoSize(int & width, int & height)
{
	return mCodecImpl->QueryVideoSize(width, height);
}

int Harness::Close(void)
{
	return mCodecImpl->Close();
}

int CodecImpl::initCheck() const {
	return mInitCheck;
}

void ListOMXCodec(sp<IOMX> omx)
{
	List<IOMX::ComponentInfo> listCodec;
	if (0 != omx->listNodes(&listCodec))
	{
		DP("Failed to list codec.");
	}

	List<IOMX::ComponentInfo>::iterator iter;
	for (iter = listCodec.begin(); iter != listCodec.end(); ++iter)
	{
		DP("OMX component: %s", (const char*)iter->mName);
		for (List<String8>::iterator iterRole = iter->mRoles.begin(); iterRole != iter->mRoles.end(); ++iterRole)
		{
			String8 & role = *iterRole;
			DP("        Role: %s", (const char*)role);
		}
	}
}

int CodecImpl::initOMX() {
	OMXClient omxClient;
	omxClient.connect();
	mOMX = omxClient.interface();
	if (mOMX != NULL)
	{
		ListOMXCodec(mOMX);
	}
	return mOMX != 0 ? 0 : -1;
}

sp<MediaExtractor> CreateExtractorFromURI(const char *uri) {
	LOGI("Try to create extractor for file %s", uri);
	DataSource::RegisterDefaultSniffers();
	sp<DataSource> source = DataSource::CreateFromURI(uri);

	if (source == NULL) {
		return NULL;
	}

	return MediaExtractor::Create(source);
	//  return new MPEG2TSExtractor(source);
}

static bool g_bExitRenderThread;

unsigned char buff[1024*1024];

void * DataFeedThread(void * param)
{
	FILE* mFile;
	DP("        DataFeedThread running");
	mFile = fopen("/sdcard/test/videodump.264","rb");
	if(mFile == 0)
	{
		DP("        DataFeedThread failed to open file");
		return 0;
	}

	DP("        DataFeedThread open file success");

	CCidanaMediaSource* pSource = (CCidanaMediaSource*)(param);
	g_bExitRenderThread = false;

	while(!g_bExitRenderThread)
	{
	
		DP("start read data , mFile:%p buff:%p \n",mFile,buff);
	       int len = fread(buff, 1, 30*1024, mFile);
		DP("read data:%p,len:%d \n",buff,len);

		pSource->Decode(buff, len, 0);
		usleep(10*1000);
	}
	fclose(mFile);
	return 0;
}
int CodecImpl::Open(const char * fileName, sp<ANativeWindow> surface)
{

#if 1
	sp<MediaExtractor> extractor = CreateExtractorFromURI(fileName);
	if (extractor == NULL)
	{
		LOGE("Fail to create extractor");
		return -1;
	}

	sp<MediaSource> sourceVideo;
	for (size_t i = 0; i < extractor->countTracks(); ++i) 
	{
		sp<MetaData> meta = extractor->getTrackMetaData(i);

		const char *trackMIME;
		meta->findCString(kKeyMIMEType, &trackMIME);

		if (!strncmp(trackMIME, "video/", 6)) {
			sourceVideo = extractor->getTrack(i);
			DP("Select track, type %s", trackMIME);
			break;
		}
	}

	if (sourceVideo == NULL)
	{
		LOGE("No supported A/V track found");
		return -1;
	}
#else
//	sp<MediaSource> sourceVideo = DummyAudioSource::Create(fileName);
	sp<CCidanaMediaSource> sourceTemp = CCidanaMediaSource::Create(720,576);
	pPrivateSourceVideo = sourceTemp.get();
	sp<MediaSource> sourceVideo = sourceTemp ;//CCidanaMediaSource::Create();
	static pthread_t _thread;
	if (0 != pthread_create(&_thread, NULL, DataFeedThread, pPrivateSourceVideo))
	{
		LOGE("RenderTest - render thread init failed\n");
		return -1;
	}

#endif

	mNativeWindow = surface;

	const char *mime = NULL;
	sp<MetaData> metaVideo = sourceVideo->getFormat();
	metaVideo->findCString(kKeyMIMEType, &mime);
	if (mime != NULL)
	{
		DP("Try to create HW decoder for %s", mime);
	}

	mCodecVideo = OMXCodec::Create(
			mOMX, metaVideo, false /* createEncoder */,
			sourceVideo, NULL, OMXCodec::kHardwareCodecsOnly, mNativeWindow);

	if (mCodecVideo == NULL)
	{
		LOGE("No HW decoder found");
		return -1;
	}

	mCodecVideo->start();

	int32_t videoWidth, videoHeight;
	sourceVideo->getFormat()->findInt32(kKeyWidth, &videoWidth);
	sourceVideo->getFormat()->findInt32(kKeyHeight, &videoHeight);

	sp<MetaData> meta = mCodecVideo->getFormat();
	int32_t format;
	const char *component;
	int32_t decodedWidth, decodedHeight;
	meta->findInt32(kKeyColorFormat, &format);
	meta->findCString(kKeyDecoderComponent, &component);
	meta->findInt32(kKeyWidth, &decodedWidth);
	meta->findInt32(kKeyHeight, &decodedHeight);

	if (strcmp(component, "OMX.Nvidia.mpeg2v.decode") == 0)
	{
		mUseRemoteRender = false;
	}
	else
	{
		mUseRemoteRender = true;
	}
	LOGI("Component %s, source frame %dx%d, output frame %dx%d, color format %d", component, videoWidth, videoHeight, decodedWidth, decodedHeight, format);
	mWidth = videoWidth;
	mHeight = videoHeight;

	int64_t durationUs;
	if (sourceVideo->getFormat()->findInt64(kKeyDuration, &durationUs))
	{
		LOGI("stream duration is %lld us (%.2f secs)", durationUs, durationUs / 1E6);
	}

	mIsFormatChangedReceived = false;


	return 0;
}

void CodecImpl::UpdateVideoSize(sp<MetaData> meta)
{
	int32_t cropLeft, cropTop, cropRight, cropBottom;
	if (!meta->findRect(kKeyCropRect, &cropLeft, &cropTop, &cropRight, &cropBottom))
	{
		int32_t width, height;
		meta->findInt32(kKeyWidth, &width);
		meta->findInt32(kKeyHeight, &height);

		cropLeft = cropTop = 0;
		cropRight = width - 1;
		cropBottom = height - 1;

		DP("got dimensions only %d x %d", width, height);
	}
	else
	{
		DP("got crop rect %d, %d, %d, %d", cropLeft, cropTop, cropRight, cropBottom);
	}

	mWidth = 0;
	if (meta->findInt32(kKeyDisplayWidth, &mWidth)) {
		DP("Display width %d", mWidth);
	}
	else
	{
		mWidth = cropRight - cropLeft + 1;
	}
	mHeight = 0;
	if (meta->findInt32(kKeyDisplayHeight, &mHeight)) {
		DP("Display height %d", mHeight);
	}
	else
	{
		mHeight = cropBottom - cropTop + 1;
	}
}

int CodecImpl::InitRenderer(sp<MetaData> meta)
{
	int32_t rotationDegrees;
	if (!meta->findInt32(kKeyRotation, &rotationDegrees))
	{
		rotationDegrees = 0;
	}

	const int scalingMode = NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW;
	if (native_window_set_scaling_mode(mNativeWindow.get(), scalingMode) != 0)  // MUST for some platforms
	{
		DP("Failed to set scaling mode");
	}

	unsigned int transform;
	switch (rotationDegrees)
	{
	case 0: transform = 0; break;
	case 90: transform = HAL_TRANSFORM_ROT_90; break;
	case 180: transform = HAL_TRANSFORM_ROT_180; break;
	case 270: transform = HAL_TRANSFORM_ROT_270; break;
	default: transform = 0; break;
	}
	if (transform != 0)
	{
		if (native_window_set_buffers_transform(mNativeWindow.get(), transform) != 0)
		{
			DP("Failed to set buffer transform");
		}
	}

	if (!mUseRemoteRender)
	{
		mSoftwareRenderer = new SoftwareRenderer(mNativeWindow, meta);  // softrenderer needs meta data to set display and crop rectangle
	}

	return 0;
}

int CodecImpl::ShowOneFrame(void)
{
	if (mCodecVideo == NULL)
	{
		return -1;
	}

	if (mTimeSource == NULL)
	{
		mFrameCnt =0;
		mTimeSource = new SystemTimeSource;
	}

	MediaSource::ReadOptions options;
	status_t err;
	MediaBuffer *buffer;

//	LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
	err = mCodecVideo->read(&buffer, &options);
//	LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
	options.clearSeekTo();
//	LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
	if (err == INFO_FORMAT_CHANGED)
	{
//		LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
		mIsFormatChangedReceived = true;

		sp<MetaData> meta = mCodecVideo->getFormat();
		UpdateVideoSize(meta);
		InitRenderer(meta);
		return 1;
	}
	else if (err == OK) 
	{
//		LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
		if (!mIsFormatChangedReceived)
		{
//			LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
			mIsFormatChangedReceived = true;
			sp<MetaData> meta = mCodecVideo->getFormat();
			UpdateVideoSize(meta);
			InitRenderer(meta);
		}
		//CHECK(buffer != NULL);
		if (buffer->range_length() == 0) 
		{
			buffer->release();
			buffer = NULL;
		}
		else
		{
//			LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
			int64_t timeUs;
			buffer->meta_data()->findInt64(kKeyTime, &timeUs);
			//LOGI("Get frame timeUs(%lld), offset %u, size %u", timeUs, buffer->range_offset(), buffer->range_length());
			int64_t mediaTime = mTimeSource->getRealTimeUs();
			if (mediaTime < timeUs)
			{
				//LOGI("Media time %lld, early %lld us", mediaTime, timeUs - mediaTime);
				usleep(timeUs - mediaTime);
			}


			mFrameCnt++;
			if(mFrameCnt %30 == 0)
			{
				LOGE("fps:%lld\n",mFrameCnt*1000000/mTimeSource->getRealTimeUs());
			}
			// do render
			status_t err;
			if (mUseRemoteRender)
			{
//				LOGE("%s mNativeWindow->queueBuffer wnd:%p , buff:%p\n",__func__,
//					mNativeWindow.get(), buffer->graphicBuffer().get());

				err = mNativeWindow->queueBuffer(mNativeWindow.get(), buffer->graphicBuffer().get());
				if (err != 0) {
					LOGE("queueBuffer failed with error %s (%d)", strerror(-err), -err);
				}
//				LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
			}
			else
			{
//				LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
				if (mSoftwareRenderer == NULL)
				{
					sp<MetaData> meta = mCodecVideo->getFormat();
					mSoftwareRenderer = new SoftwareRenderer(mNativeWindow, meta);  // softrenderer needs meta data to set display and crop rectangle
				}
				unsigned char * data = (unsigned char *)buffer->data() + buffer->range_offset();
				size_t size = buffer->range_length();
//				LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
				mSoftwareRenderer->render(data, size, NULL);
//				LOGE("[%s]L:%d \n",__FUNCTION__,__LINE__);
			}

			/////////////////////////////
			sp<MetaData> metaData = buffer->meta_data();
			metaData->setInt32(kKeyRendered, 1);

			buffer->release();
			buffer = NULL;
		}

		return 0;
	}
	else
	{
		return -1;
	}
}

int CodecImpl::QueryVideoSize(int & width, int & height)
{
	width = mWidth;
	height = mHeight;
	return 0;
}

int CodecImpl::Close(void)
{
	DP("%s IN", __FUNCTION__);
	g_bExitRenderThread = true;
	usleep(100*1000);

	if (mCodecVideo != NULL)
	{
		mCodecVideo->stop();
		mCodecVideo.clear();
		DP("%s - decoder released", __FUNCTION__);
	}
	//mRender.clear();
	mNativeWindow.clear();
	if (mSoftwareRenderer != NULL)
	{
		delete mSoftwareRenderer;
		mSoftwareRenderer = NULL;
	}
	DP("%s - renderer released", __FUNCTION__);
	if (mTimeSource != NULL)
	{
		delete mTimeSource;
		DP("%s - time source released", __FUNCTION__);
	}

	DP("%s EXIT", __FUNCTION__);
	return 0;
}

void CodecImpl::LoadSystemInfo()
{
	char buf[PROPERTY_VALUE_MAX];
	int len;
	len = property_get("ro.board.platform", buf, "");
	if (len > 0)
	{
		mPlatform = strdup(buf);
	}
	else
	{
		mPlatform = NULL;
	}

	len = property_get("ro.product.board", buf, "");
	if (len > 0)
	{
		mDevice = strdup(buf);
	}
	else
	{
		mDevice = NULL;
	}

	mSDK = 0;
	len = property_get("ro.build.version.sdk", buf, "");
	if (len > 0)
	{
		int sdk;
		if (sscanf(buf, "%d", &sdk) == 1)
		{
			mSDK = sdk;
		}
	}

	DP("System info: Platform %s, Device %s, SDK %d", mPlatform, mDevice, mSDK);
}
