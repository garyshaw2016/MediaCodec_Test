/*
 * Copyright (C) 2009 The Android Open Source Project
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

#ifndef OMX_HARNESS_H_

#define OMX_HARNESS_H_


class CodecImpl;
class ANativeWindow;

struct Harness
{
    Harness();

    int initCheck() const;

	int Open(const char * fileName, ANativeWindow * surface);
	// return 0 for success, 1 for video size changed, -1 for error
	int ShowOneFrame(void);
	int Close(void);
	int QueryVideoSize(int & width, int & height);

    virtual ~Harness();

    static const int MSG_VIDEO_SIZE_CHANGED = 1;
private:
	CodecImpl * mCodecImpl;
};

#endif  // OMX_HARNESS_H_
