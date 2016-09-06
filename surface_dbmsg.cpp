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

#define LOG_TAG "surface_dbmsg"
//#define LOG_NDEBUG 0

#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>

#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/Surface.h>
#include <ui/DisplayInfo.h>
#include <android/native_window.h>

#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkColor.h"
#include "SkPaint.h"
#include "SkPoint.h"
#include "SkRect.h"
#include "SkTypes.h"

#include "surface_dbmsg.h"

namespace android {

class surface_dbmsg_core : public RefBase {
public:
    surface_dbmsg_core();
    virtual ~surface_dbmsg_core();
    int skia_object_init(int tx_sz, int tx_color, int width, int height);
    int surface_init(int x, int y, int width, int height);
    void show(int bg_color, char *tx, int sz);
    void clear_log(int bg_color);
    sp<SurfaceComposerClient> composerClient;
    sp<SurfaceControl> control;
    sp<Surface> surface;
    DisplayInfo info;
    ANativeWindow_Buffer outBuffer;
    SkPaint paint;
    SkBitmap drawTextBitmap;
    SkCanvas *drawTextCanvas;
    SkPoint point;
};

surface_dbmsg_core::surface_dbmsg_core()
{
    ALOGV("surface_dbmsg_core()");
}

surface_dbmsg_core::~surface_dbmsg_core()
{
    if (drawTextCanvas)
        free(drawTextCanvas);

    ALOGV("~surface_dbmsg_core()");
}

int surface_dbmsg_core::skia_object_init(int tx_sz, int tx_color, int width, int height)
{
    if (tx_sz < 8 || tx_sz > 20) {
        tx_sz = 12;
    }

    paint.setColor(tx_color);
    paint.setTextSize(SkIntToScalar(tx_sz));
    paint.setTextAlign(SkPaint::kLeft_Align);
    paint.setAntiAlias(1);
    paint.setSubpixelText(1);
    paint.setLCDRenderText(1);

    drawTextBitmap.setConfig(SkBitmap::kARGB_8888_Config, width, height);
    drawTextBitmap.allocPixels();
    //drawTextBitmap.setPixels(img);
    drawTextCanvas = new SkCanvas(drawTextBitmap);
    if (!drawTextCanvas) {
        ALOGE("SkCanvas alloc error");
        return NO_MEMORY;
    }

    point = SkPoint::Make(1.0f, (SkScalar)tx_sz);

    return NO_ERROR;
}

int surface_dbmsg_core::surface_init(int x, int y, int width, int height)
{
    composerClient = new SurfaceComposerClient;
    if (NO_ERROR != composerClient->initCheck()) {
        ALOGE("composer client init error");
        return NO_INIT;
    }

    sp<IBinder> display(SurfaceComposerClient::getBuiltInDisplay(
            ISurfaceComposer::eDisplayIdMain));
    SurfaceComposerClient::getDisplayInfo(display, &info);

    ALOGV("display is %ld x %ld\n", info.w, info.h);

    if (width > info.w || height > info.h || width < 0 || height < 0)
        return BAD_VALUE;

    control = composerClient->createSurface(
            String8("Surface Debug MSG"),
            width,
            height,
            PIXEL_FORMAT_RGBA_8888,
            0);

    if (control == 0) {
        ALOGE("SurfaceControl is NULL\n");
        return NO_INIT;
    }

    if (!control->isValid()) {
        ALOGE("SurfaceControl is not valid\n");
        return NO_INIT;
    }

    SurfaceComposerClient::openGlobalTransaction();
    if (NO_ERROR != control->setPosition(x, y)) {
        ALOGE("SurfaceControl setPosition error\n");
        SurfaceComposerClient::closeGlobalTransaction();
        return UNKNOWN_ERROR;
    }

    if (NO_ERROR != control->setLayer(INT_MAX)) {
        ALOGE("SurfaceControl setLayer error\n");
        SurfaceComposerClient::closeGlobalTransaction();
        return UNKNOWN_ERROR;
    }

    if (NO_ERROR != control->show()) {
        ALOGE("SurfaceControl show error\n");
        SurfaceComposerClient::closeGlobalTransaction();
        return UNKNOWN_ERROR;
    }
    SurfaceComposerClient::closeGlobalTransaction();

    surface = control->getSurface();
    if (surface == 0) {
        ALOGE("Surface is NULL\n");
        return UNKNOWN_ERROR;
    }

    return NO_ERROR;
}

void surface_dbmsg_core::show(int bg_color, char *tx, int sz)
{
    char *surface_img;
    char *bitmap_img;

    drawTextCanvas->drawColor(bg_color, SkXfermode::kClear_Mode);
    drawTextCanvas->drawColor(bg_color, SkXfermode::kSrc_Mode);
    drawTextCanvas->drawText(tx, sz, point.fX, point.fY, paint);
    bitmap_img = (char *)drawTextBitmap.getAddr(0, 0);
    surface->lock(&outBuffer, NULL);
    surface_img = reinterpret_cast<char*>(outBuffer.bits);
    memcpy(surface_img, bitmap_img, drawTextBitmap.getSafeSize());
    surface->unlockAndPost();
}

void surface_dbmsg_core::clear_log(int bg_color)
{
    char *surface_img;
    char *bitmap_img;

    drawTextCanvas->drawColor(bg_color, SkXfermode::kClear_Mode);
    bitmap_img = (char *)drawTextBitmap.getAddr(0, 0);
    surface->lock(&outBuffer, NULL);
    surface_img = reinterpret_cast<char*>(outBuffer.bits);
    memcpy(surface_img, bitmap_img, drawTextBitmap.getSafeSize());
    surface->unlockAndPost();
}

surface_dbmsg::surface_dbmsg()
{
    ALOGV("surface_dbmsg()");
}

surface_dbmsg::~surface_dbmsg()
{
    ALOGV("~surface_dbmsg()");
}

int surface_dbmsg::setup(int x, int y, int width, int height, int tx_sz, int bg_color, int tx_color)
{
    this->x = x;
    this->y = y;
    this->width = ((width + SK_DRAW_TEXT_WIDTH_UNIT) / SK_DRAW_TEXT_WIDTH_UNIT) * 80;
    this->height = height;
    this->bg_color = bg_color;
    this->tx_color = tx_color;
    ALOGV("setup x[%d], y[%d], w[%d], h[%d], tx_sz[%d], bg[0x%x], tx_color[0x%x]",
            this->x, this->y, this->width, this->height, tx_sz, this->bg_color, this->tx_color);

    if (dbmsg_core != 0) {
        ALOGV("surface_dbmsg clear for new surface_dbmsg");
        dbmsg_core.clear();
    }

    dbmsg_core = new surface_dbmsg_core();
    if (dbmsg_core == 0) {
        ALOGE("surface_dbmsg surface memory alloc error");
        return NO_MEMORY;
    }

    if (NO_ERROR != dbmsg_core->surface_init(this->x, this->y, this->width, this->height)) {
        ALOGE("surface_dbmsg surface init error");
        return UNKNOWN_ERROR;
    }

    if (NO_ERROR != dbmsg_core->skia_object_init(tx_sz, this->tx_color, this->width, this->height)) {
        ALOGE("surface_dbmsg skia init error");
        return UNKNOWN_ERROR;
    }

    return OK;
}

void surface_dbmsg::print(const char *fmt, ...)
{
    va_list ap;
    char buf[512];
    va_start(ap, fmt);
    vsnprintf(buf, 512, fmt, ap);
    va_end(ap);

    dbmsg_core->show(bg_color, buf, strlen(buf));
}

void surface_dbmsg::clear_log(void)
{
    dbmsg_core->clear_log(bg_color);
}
}; // namespace android
