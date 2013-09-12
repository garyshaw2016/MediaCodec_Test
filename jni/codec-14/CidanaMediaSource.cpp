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
#define LOG_TAG "CCidanaMediaSource"

//#include <android/log.h>

#include <utils/Log.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaBufferGroup.h>
#include <media/stagefright/MetaData.h>

#include "CidanaMediaSource.h"
#define LOGE ALOGE
#define LOGI ALOGI
#define LOGW ALOGW
#define LOGV ALOGV
namespace android {

struct packet
{
    int datalen;
    unsigned char *data;
};

class CPacketQueue
{
public:
    CPacketQueue()
    {
        m_datasize = 0;
       database_cnt  = 0 ;
	database_cnt_Read = 0 ;

        memset(database, 0 ,sizeof(database));
    }

    ~CPacketQueue()
    {
        Clear();

    }

    void Clear()
    {
        m_datasize = 0;

    }

	int Write(unsigned char *data, int length)
	{
		if(data && length > 0)
		{		


				packet *p = new packet;
				p->data = new unsigned char[length];
				p->datalen = length;
				memcpy(p->data, data, length);

				database[database_cnt] = p;	
				database_cnt++;
				m_datasize += length;

				return length;
		}
		else
		{
			//DP("[CSemQueue] Write invalid Queue!\n");
		}
		return -1;
	}

    int Read(unsigned char *buffer, int length)
    {
	    if(buffer && length > 0)
        {
		if(database_cnt_Read >=database_cnt )
			return 0;
		packet *p= database[database_cnt_Read];

		
		int ret = p->datalen < length ? p->datalen : length;

		memcpy(buffer, p->data, ret);

//		if(database_cnt_Read !=database_cnt -1)
			delete []p->data;
			delete p;
			database_cnt_Read++;

		return ret;
        }
		else
		{
			//DP("[CSemQueue] Read invalid Queue!\n");
		}
        return -1;
    }

private:
	packet* database[10000];
	int database_cnt ;
	int database_cnt_Read ;
    int m_ProductNumber;
    int m_datasize;
};

CPacketQueue gQueue;
//static
sp<CCidanaMediaSource> CCidanaMediaSource::Create(int width, int height) 
{
     return new CCidanaMediaSource(width, height);
}

CCidanaMediaSource::CCidanaMediaSource(int width, int height)
    : mBufferGroup(NULL) {

    mWidth = width;
    mHeight = height;

	//strcpy(mfileName, fileName);
//	mFrameSize = 30*1024;

//	sprintf(mfileName, "/mnt/sdcard/Alexander_Trailer_720x480_2M_30fps.264");
//    LOGI("File Path = %s", mfileName);
    LOGI("Constructor: X");
}

CCidanaMediaSource::~CCidanaMediaSource() {
    /* Do nothing here? */
    LOGI("~CCidanaMediaSource");
}

void CCidanaMediaSource::setDuration(int64_t audioDurationUs) {
    LOGI("setDuration: %lld us ",
        audioDurationUs);

    Mutex::Autolock autoLock(mLock);
}

status_t CCidanaMediaSource::start(MetaData *params) {
    LOGI("start: E");
    status_t err = OK;

    mTimeStampUs = 0;
	frameCount = 0;
/*
	mFile = fopen("/sdcard/1.264","wb");
	if(mFile == 0)
		return BAD_VALUE;
*/

    mBufferGroup = new MediaBufferGroup;

    mBufferGroup->add_buffer(new MediaBuffer(1024*1024));

    LOGI("start: X");

    return err;
}

status_t CCidanaMediaSource::stop() {
    LOGI("stop");

//	fclose(mFile);
    delete mBufferGroup;
    mBufferGroup = NULL;

    return OK;
}


sp<MetaData> CCidanaMediaSource::getFormat() {
    sp<MetaData> meta = new MetaData;

    meta->setInt32(kKeyWidth, mWidth);
    meta->setInt32(kKeyHeight, mHeight);
    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_AVC );
    meta->setCString(kKeyDecoderComponent, "CCidanaMediaSource");

    int64_t durationUs = 60*1000*1000;
    meta->setInt64(kKeyDuration, durationUs);

    return meta;
}

unsigned char privatebuffer[1024*1024];

status_t CCidanaMediaSource::read(
        MediaBuffer **out, const MediaSource::ReadOptions *options) {

//    LOGE("read: E");


    int64_t seekTimeUs;
    ReadOptions::SeekMode mode;

    if (options && options->getSeekTo(&seekTimeUs, &mode)) {
        CHECK(seekTimeUs >= 0);
        mTimeStampUs = seekTimeUs;
	 LOGI("read: seek to %d \n",mTimeStampUs );
    }

/*
    MediaBuffer *buffer;
    status_t err = mBufferGroup->acquire_buffer(&buffer);
    if (err != OK) {
        LOGE("Failed to acquire buffer from mBufferGroup: %d", err);
        return err;
    }
*/


	int ret = 0;


	do
	{
		ret = gQueue.Read(privatebuffer, sizeof(privatebuffer));
		if(ret <=0)
			usleep(10*1000);
	}while(ret <=0);

	mFrameSize = ret;

        status_t err =  mBufferGroup->acquire_buffer(&mBuffer);
        if (err != OK) {
            LOGE("Failed to acquire buffer from mBufferGroup: %d", err);
            return err;
        }



/*
    int len = fread((uint8_t *) buffer->data(), 1, mFrameSize, mFile);
    if(len != mFrameSize)
    {
	 LOGI("read failed %d \n",len);
	fseek(mFile, 0, SEEK_SET);
	}
	*/
//	 LOGE("read memcpy: %p,%p,%d \n",mBuffer->data(), privatebuffer,mFrameSize);
	
	memcpy(mBuffer->data(), privatebuffer,mFrameSize);

	 char* pPointer = (char*)mBuffer->data();

	 LOGE("read data size: %d \n",mFrameSize);
	 char Buff[1024];
	 memset(Buff, 0 ,sizeof(Buff));
	 for(int i=0;i<16;i++)
	 {
		 char Buff_num[10];
		 memset(Buff_num, 0 ,sizeof(Buff_num));
		sprintf(Buff_num, "%02x ",pPointer[i]);
		strcat(Buff,Buff_num);
	 }
	 LOGE("%s  \n",Buff);

    mBuffer->set_range(mBuffer->range_offset(), mFrameSize);
    mBuffer->meta_data()->clear();
    mBuffer->meta_data()->setInt64(kKeyTime, mTimeStampUs);
		

    LOGI("read: mTimeStampUs = %lld",            mTimeStampUs);

    mTimeStampUs = mTimeStampUs + 30*1000;

    *out = mBuffer;
//    LOGE("read: X");

    return OK;
}


#ifndef GET_NALU_TYPE
#define GET_NALU_TYPE(x) ((x)&0x1f)
#endif

status_t CCidanaMediaSource::FindStartCode(CI_U8 *pCurr,
	CI_U8 *pBufEnd,CI_BITS* pBits,CI_S32 vtype)
{
	CI_BITS bits = {0};

	CI_S32 newPacket = 0;
	CI_S32 code = 0xffffffff;
	CI_S32 type = 0;
	do 
	{
		code = (code<<8) | (*pCurr++);
		if((code&0xffffff) == 0x000001)
		{
			type = *pCurr;
			// prepare start code
			StartBits(&bits, pCurr-1, (CI_S32)(pBufEnd-pCurr));
			pCurr++;

			type = GET_NALU_TYPE(type);
			if((vtype!=0 )&& (type != vtype))
				continue;

			newPacket = 1;
		}
	} while (pCurr<pBufEnd-1 && newPacket==0);

	if(newPacket==0)	return CI_EFAIL;

	memcpy(pBits, &bits, sizeof(bits));
	return CI_SOK;
}

status_t CCidanaMediaSource::FindH264Data(CI_H264_SYNTAX *pSyntax, CI_RAW_PIC *pPic, 
	CI_S32 vtype, unsigned char * pData, char* pSize)
{
	CI_U8 *pFrm = pPic->pFrm;
	CI_S32 frmLen = pPic->frmLen;
	CI_U8 *pBufEnd = pFrm + frmLen;
	CI_U8 *pCurr = pFrm;

	CI_BITS bits, *pBits = &bits;
	status_t ret = CI_SOK;


	CI_U32 dataSize =0;
	ret = FindStartCode(pCurr, pBufEnd, pBits,vtype);
	if(ret == CI_SOK)
	{
		CI_U8 *pCurrFind = pBits->pu8Start;		
		pCurr = pBits->pu8Start;
		ret = FindStartCode(pCurr, pBufEnd, pBits,0);
		if(ret == CI_SOK)
			dataSize = pBits->pu8Start - pCurrFind;
		else
			dataSize = pBits->u32BufLen;

		memcpy(pData, pCurr -3,dataSize);
		*pSize = dataSize;

		return CI_SOK;
	}

	return CI_EFAIL;

}

status_t CCidanaMediaSource::GetSPS_PPS(CI_RAW_PIC *pRawPic,unsigned char * csd
,char* pCsd_size)
{
	CI_H264_SYNTAX syntax = {0};
	
	FindH264Data(&syntax, pRawPic, NALU_TYPE_SPS, sps,&(sps_size));
	FindH264Data(&syntax, pRawPic, NALU_TYPE_PPS, pps,&(pps_size));

	if((sps_size == 0) || (pps_size == 0))
		return CI_EABORT;
		
	memcpy(csd, sps, sps_size);
	memcpy(csd + sps_size, pps, pps_size);

	*pCsd_size = sps_size + pps_size;
	return CI_SOK;
}

int CCidanaMediaSource::Decode(unsigned char * sample, int sampleSize, long long pts)
{
	if(sampleSize > 1024*1024)
	{
	    LOGE("buff too small , need enalrge to %d \n",sampleSize);
		return OK;
	}
//	    LOGE("Decode %p ,%d ,pts: %lld \n",sample, sampleSize,pts);

		
//	LOGE("decode : E");
#if 0
	if(frameCount == 0)
	{
		unsigned char csd[256]; 
		char csd_size=0 ;

		CI_RAW_PIC RawPic;
		RawPic.pFrm = sample;
		RawPic.frmLen = sampleSize;

		status_t ret = GetSPS_PPS(&RawPic,csd,&csd_size);
		if(ret != OK)
			return ret;

		CITraceW("GetSPSPPS:%d \n",csd_size);


		char buf[256*1024];
		memset(buf,0, sizeof(buf));

		for(int i=0;i<csd_size;i++)
		{
			char buf2[8];
			sprintf(buf2, "%02x ", csd[i]);
			strcat(buf, buf2);
		}
		CITraceW("%s \n",buf);

		gQueue.Write(csd, csd_size);

	}
#endif


	int ret = 0;
	do
	{
		ret = gQueue.Write(sample, sampleSize);
		if(ret == -1)
			usleep(10*1000);
	}while(ret == -1);

//	LOGE("decode : X");

//	memcpy(sampleBuff, sample ,sampleSize);
//	g_sampleSize = sampleSize;
/*
    MediaBuffer *buffer= new MediaBuffer(sampleSize);
    
 	memcpy((uint8_t *) buffer->data(), sample, sampleSize);
	buffer->set_range(0, sampleSize);
	buffer->meta_data()->setInt64(kKeyTime, pts);

	mBufferGroup->add_buffer(buffer);

	if(pts == 0)
		mTimeStampUs += 30*1000;
		*/

	++frameCount;

    return OK;
}

}// namespace android


