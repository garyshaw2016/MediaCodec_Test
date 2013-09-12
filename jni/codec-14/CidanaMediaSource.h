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

#ifndef CIDANA_MEDIASOURCE_H_
#define CIDANA_MEDIASOURCE_H_


#include <media/stagefright/MediaSource.h> 

#include "ci_codec_type.h"
#include "ci_hwdec_debug.h"
#include "ci_hwdec_type.h"
#include "bits.h"

namespace android {

class MetaData;
struct MediaBufferGroup;

struct CCidanaMediaSource : public MediaSource {

public:
    static sp<CCidanaMediaSource> Create(int width, int height);

    virtual status_t start(MetaData *params = NULL);
    virtual status_t stop();
    virtual sp<MetaData> getFormat();

    virtual status_t read(
                MediaBuffer **buffer,
                const MediaSource::ReadOptions *options = NULL);


	void setDuration(int64_t audioDurationUs);
	int Decode(unsigned char * sample, int sampleSize, long long pts);

protected:
    virtual ~CCidanaMediaSource();

private:
    char mfileName[256];
    Mutex mLock;

	MediaBufferGroup *mBufferGroup;
	MediaBuffer *mBuffer;

	FILE* mFile;
	int mFrameSize;
	int mWidth;
	int mHeight;
    CCidanaMediaSource(int width, int height);

    // Don't call me
    CCidanaMediaSource(const CCidanaMediaSource &);
    CCidanaMediaSource &operator=(const CCidanaMediaSource &);


	status_t FindStartCode(CI_U8 *pCurr,
		CI_U8 *pBufEnd,CI_BITS* pBits,CI_S32 vtype);
	status_t FindH264Data(CI_H264_SYNTAX *pSyntax, CI_RAW_PIC *pPic, 
		CI_S32 vtype, unsigned char * pData, char* pSize);
	status_t GetSPS_PPS(CI_RAW_PIC *pRawPic,unsigned char * csd
		,char* pCsd_size);

	int64_t mTimeStampUs;
	int frameCount;
	unsigned char sps[64];
	char sps_size;
	
	unsigned char pps[64];
	char pps_size;

};

}//namespace android


#endif //CIDANA_MEDIASOURCE_H_

