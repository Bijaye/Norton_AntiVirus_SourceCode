# Makefile for NAVAPI NLM tester

TARGET_NAME=TEST
.INCLUDE $(QUAKEINC)\PLATFORM.MAK

$(TARGET_NAME).NLM: \
    test.obj \
    $(TARGET_NAME).den \
    navapi.imp \
    navapi.exp \
    $(Q_SYMKRNL).lib \
