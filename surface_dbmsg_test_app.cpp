
#define LOG_NDEBUG 0
#define LOG_TAG "surface_dbmsg_test_app"
#include <utils/Log.h>

#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include "surface_dbmsg.h"

int main(int argc, char **argv) {
    using namespace android;

    ProcessState::self()->startThreadPool();

    sp<surface_dbmsg> sfd = new surface_dbmsg();
    sfd->setup(0, 100, 180, 50, 14, SD_ColorBLUE, SD_ColorYELLOW);
    sfd->print("This is the Text #1");
    sleep(1);

    sfd->setup(0, 200, 240, 50, 14, SD_ColorTRANSPARENT, SD_ColorGREEN);
    sfd->print("This is the Text #2");
    sleep(1);

    return 0;
}
