#include <jni.h>
#include <android/log.h>
#include "dobby.h"

#define TAG "AppSealing"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

unsigned int (*orig_alarm)(unsigned int seconds);
int (*orig_kill)(int pid, int sig);
void (*orig_exit)(int status);

unsigned int alarm_hook(unsigned int seconds) {
    LOGI("alarm(%d) redirected", seconds);
    return 0;
}

int kill_hook(int pid, int sig) {
    LOGI("kill(%d, %d) redirected", pid, sig);
    return 0;
}

void exit_hook(int status) {
    LOGI("exit(%d) redirected", status);
}

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("Finding symbols...");
    void *alarm = DobbySymbolResolver(NULL, "alarm");
    void *kill = DobbySymbolResolver(NULL, "kill");
    void *exit = DobbySymbolResolver(NULL, "exit");

    if (alarm) {
        LOGI("Found alarm at %p", alarm);
        DobbyHook((void *) alarm, (void *) alarm_hook, (void **) &orig_alarm);
        LOGI("Hooked alarm");
    } else {
        LOGI("Failed to find alarm");
    }

    if (kill) {
        LOGI("Found kill at %p", kill);
        DobbyHook((void *) kill, (void *) kill_hook, (void **) &orig_kill);
        LOGI("Hooked kill");
    } else {
        LOGI("Failed to find kill");
    }

    if (exit) {
        LOGI("Found exit at %p", exit);
        DobbyHook((void *) exit, (void *) exit_hook, (void **) &orig_exit);
        LOGI("Hooked exit");
    } else {
        LOGI("Failed to find exit");
    }

    return JNI_VERSION_1_6;
}