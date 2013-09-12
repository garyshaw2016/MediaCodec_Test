/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #define LOG_NDEBUG 0
#define LOG_TAG "DummyAudioSource"

//#include <android/log.h>

#include <utils/Log.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/MetaData.h>

#include "DummyAudioSource.h"


namespace android {

//static
sp<DummyAudioSource> DummyAudioSource::Create(
        const char * fileName) {

    LOGI("Create ");
    return new DummyAudioSource(fileName);

}

DummyAudioSource::DummyAudioSource(
        const char * fileName)
    : mBufferGroup(NULL) {

	strcpy(mfileName, fileName);
	mFrameSize = 30*1024;

//	sprintf(mfileName, "/mnt/sdcard/Alexander_Trailer_720x480_2M_30fps.264");
	sprintf(mfileName, "/sdcard/test/videodump.264");
    LOGI("File Path = %s", mfileName);
    LOGI("Constructor: X");
}

DummyAudioSource::~DummyAudioSource() {
    /* Do nothing here? */
    LOGI("~DummyAudioSource");
}

void DummyAudioSource::setDuration(int64_t audioDurationUs) {
    LOGI("setDuration: %lld us ",
        audioDurationUs);

    Mutex::Autolock autoLock(mLock);
}

status_t DummyAudioSource::start(MetaData *params) {
    LOGI("start: E");
    status_t err = OK;

    mTimeStampUs = 0;

	mFile = fopen(mfileName,"rb");
	if(mFile == 0)
		return BAD_VALUE;

	LOGI("read file %s \n",mfileName);

    mBufferGroup = new MediaBufferGroup;
    mBufferGroup->add_buffer(
            new MediaBuffer(mFrameSize));

    LOGI("start: X");
    return err;
}

status_t DummyAudioSource::stop() {
    LOGI("stop");

    delete mBufferGroup;
    mBufferGroup = NULL;

    return OK;
}


sp<MetaData> DummyAudioSource::getFormat() {
    LOGI("getFormat");

    sp<MetaData> meta = new MetaData;

    meta->setInt32(kKeyWidth, 720);
    meta->setInt32(kKeyHeight, 576);
    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_AVC );
    meta->setCString(kKeyDecoderComponent, "DummyAudioSource");

    int64_t durationUs = 60*1000*1000;
    meta->setInt64(kKeyDuration, durationUs);

    return meta;
}
int gCount =0;
status_t DummyAudioSource::read(
        MediaBuffer **out, const MediaSource::ReadOptions *options) {


/*

	if(gCount ==2)
		return INVALID_OPERATION;
*/
    LOGI("read: E");

    int64_t seekTimeUs;
    ReadOptions::SeekMode mode;

    if (options && options->getSeekTo(&seekTimeUs, &mode)) {
        CHECK(seekTimeUs >= 0);
        mTimeStampUs = seekTimeUs;
	 LOGI("read: seek to %d \n",mTimeStampUs );
    }


    MediaBuffer *buffer;
    status_t err = mBufferGroup->acquire_buffer(&buffer);
    if (err != OK) {
        LOGE("Failed to acquire buffer from mBufferGroup: %d", err);
        return err;
    }

//    MediaBuffer *buffer = new MediaBuffer(mFrameSize);

    switch(gCount)
    {
    case 0:
    	mFrameSize = 11220;
    	break;
    case 1:
    	mFrameSize = 3547;
    	break;
    case 2:
    	mFrameSize = 4733;
    	break;
    case 3:
    	mFrameSize = 2933;
    	break;
    case 4:
    	mFrameSize = 6496;
    	break;
    case 5:
    	mFrameSize = 6602;
    	break;
    default:
    	mFrameSize = 30*1024;
    	break;

    }
    int len = fread((uint8_t *) buffer->data(), 1, mFrameSize, mFile);

	 char* pPointer = (char*)buffer->data();

	 LOGE("read data size: %d \n",mFrameSize);
	 char Buff[1024];
		 memset(Buff, 0 ,sizeof(Buff));
	 for(int i=0;i<128;i++)
	 {
		 char Buff_num[10];
		 memset(Buff_num, 0 ,sizeof(Buff_num));
		sprintf(Buff_num, "%02x ",pPointer[i]);
		strcat(Buff,Buff_num);
	 }
	 LOGE("%s  \n",Buff);

    if(len != mFrameSize)
    {
	 LOGI("read failed %d \n",len);
	fseek(mFile, 0, SEEK_SET);
	}
		
    buffer->set_range(buffer->range_offset(), mFrameSize);
    buffer->meta_data()->setInt64(kKeyTime, mTimeStampUs);

//    LOGI("read: offset  = %d, size = %d, mTimeStampUs = %lld",
//             buffer->range_offset(), buffer->size(), mTimeStampUs);

    mTimeStampUs = mTimeStampUs + 30*1000;

    *out = buffer;
    LOGI("read: X");
	gCount++;

    return OK;
}

}// namespace android
