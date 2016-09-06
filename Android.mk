LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= surface_dbmsg.cpp

LOCAL_SHARED_LIBRARIES := \
	liblog libutils libbinder \
        libgui libcutils libui libskia

LOCAL_C_INCLUDES:= \
	$(TOP)/external/skia/include/core

LOCAL_CFLAGS += -Wno-multichar

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= libsurface_dbmsg

include $(BUILD_SHARED_LIBRARY)
##############################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= surface_dbmsg_test_app.cpp

LOCAL_SHARED_LIBRARIES := \
	liblog libutils libbinder \
        libcutils libsurface_dbmsg

LOCAL_C_INCLUDES:= 

LOCAL_CFLAGS += -Wno-multichar

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= surface_dbmsg_test_app

include $(BUILD_EXECUTABLE)
