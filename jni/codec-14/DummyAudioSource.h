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

#ifndef DUMMY_AUDIOSOURCE_H_
#define DUMMY_AUDIOSOURCE_H_

#include <media/stagefright/MediaSource.h> 


namespace android {

class MetaData;
struct MediaBufferGroup;

struct DummyAudioSource : public MediaSource {

public:
    static sp<DummyAudioSource> Create(
	const char * fileName);

    virtual status_t start(MetaData *params = NULL);
    virtual status_t stop();
    virtual sp<MetaData> getFormat();

    virtual status_t read(
                MediaBuffer **buffer,
                const MediaSource::ReadOptions *options = NULL);

    void setDuration(int64_t audioDurationUs);

protected:
    virtual ~DummyAudioSource();

private:
    char mfileName[256];
    Mutex mLock;

    MediaBufferGroup *mBufferGroup;
	FILE* mFile;
	int mFrameSize;
    DummyAudioSource(const char * fileName);

    // Don't call me
    DummyAudioSource(const DummyAudioSource &);
    DummyAudioSource &operator=(const DummyAudioSource &);

	int64_t mTimeStampUs;


};

}//namespace android


#endif //DUMMY_AUDIOSOURCE_H_

