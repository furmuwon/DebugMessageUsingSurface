/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef ANDROID_SURFACE_DBMSG_H
#define ANDROID_SURFACE_DBMSG_H

#include <utils/RefBase.h>

#define SD_ColorTRANSPARENT 0x00000000

#define SD_ColorBLACK       0xFF000000
#define SD_ColorDKGRAY      0xFF444444
#define SD_ColorGRAY        0xFF888888
#define SD_ColorLTGRAY      0xFFCCCCCC
#define SD_ColorWHITE       0xFFFFFFFF

#define SD_ColorRED         0xFFFF0000
#define SD_ColorGREEN       0xFF00FF00
#define SD_ColorBLUE        0xFF0000FF
#define SD_ColorYELLOW      0xFFFFFF00
#define SD_ColorCYAN        0xFF00FFFF
#define SD_ColorMAGENTA     0xFFFF00FF

#define SK_DRAW_TEXT_WIDTH_UNIT 80

namespace android {

class surface_dbmsg_core;

class surface_dbmsg : public RefBase {
public:
    surface_dbmsg();
    virtual ~surface_dbmsg();
    int setup(int x, int y, int width, int height, int tx_sz, int bg_color, int tx_color);
    void print(const char *fmt, ...);
    void clear_log(void);
private:
    sp<surface_dbmsg_core> dbmsg_core;
    int width, height, x, y;
    int bg_color;
    int tx_color;
};

}; // namespace android

#endif // ANDROID_SURFACE_DBMSG_H
