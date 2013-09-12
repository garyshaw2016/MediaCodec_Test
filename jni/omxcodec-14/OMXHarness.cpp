
#include "OMXHarness.h"

#include <media/stagefright/DataSource.h>
#include <media/stagefright/MediaSource.h>  // OMXCodec uses this interface to pull data
#include <media/stagefright/MetaData.h>
#include "OMXCodec.h"
#include <media/stagefright/MediaExtractor.h>

#include <media/stagefright/OMXClient.h>
#include <media/stagefright/TimeSource.h>
#include <SoftwareRenderer.h>

#include <cutils/properties.h>  // for get_property


#include "dp.h"

#include <media/IOMX.h>
#include <utils/threads.h>

#include <OMX_Component.h>


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
}

int CodecImpl::Open(const char * fileName, sp<ANativeWindow> surface)
{
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

	mNativeWindow = surface;

	const char *mime = NULL;
	sp<MetaData> metaVideo = sourceVideo->getFormat();
	metaVideo->findCString(kKeyMIMEType, &mime);
	if (mime != NULL)
	{
		DP("Try to create HW decoder for %s", mime);
	}

	mCodecVideo = CiOMXCodec::Create(
			mOMX, metaVideo, false /* createEncoder */,
			sourceVideo, NULL, CiOMXCodec::kHardwareCodecsOnly, mNativeWindow);

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
		mTimeSource = new SystemTimeSource;
	}

	MediaSource::ReadOptions options;
	status_t err;
	MediaBuffer *buffer;

	err = mCodecVideo->read(&buffer, &options);
	options.clearSeekTo();
	if (err == INFO_FORMAT_CHANGED)
	{
		mIsFormatChangedReceived = true;

		sp<MetaData> meta = mCodecVideo->getFormat();
		UpdateVideoSize(meta);
		InitRenderer(meta);
		return 1;
	}
	else if (err == OK) 
	{
		if (!mIsFormatChangedReceived)
		{
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
			int64_t timeUs;
			buffer->meta_data()->findInt64(kKeyTime, &timeUs);
			LOGI("Get frame timeUs(%lld), offset %u, size %u", timeUs, buffer->range_offset(), buffer->range_length());
			int64_t mediaTime = mTimeSource->getRealTimeUs();
			if (mediaTime < timeUs)
			{
				//LOGI("Media time %lld, early %lld us", mediaTime, timeUs - mediaTime);
				usleep(timeUs - mediaTime);
			}

			// do render
			status_t err;
			if (mUseRemoteRender)
			{
				err = mNativeWindow->queueBuffer(mNativeWindow.get(), buffer->graphicBuffer().get());
				if (err != 0) {
					DP("queueBuffer failed with error %s (%d)", strerror(-err), -err);
				}
			}
			else
			{
				if (mSoftwareRenderer == NULL)
				{
					sp<MetaData> meta = mCodecVideo->getFormat();
					mSoftwareRenderer = new SoftwareRenderer(mNativeWindow, meta);  // softrenderer needs meta data to set display and crop rectangle
				}
				unsigned char * data = (unsigned char *)buffer->data() + buffer->range_offset();
				size_t size = buffer->range_length();
				mSoftwareRenderer->render(data, size, NULL);
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
