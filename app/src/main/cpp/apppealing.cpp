#include <jni.h>
#include <android/log.h>
#include <cstdio>
#include <string>
#include <filesystem>
#include "dobby.h"

#define TAG "AppSealing"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

namespace fs = std::filesystem;

unsigned int (*orig_alarm)(unsigned int seconds);
int (*orig_kill)(int pid, int sig);
void (*orig_exit)(int status);
int (*orig_remove)(const char *pathname);
FILE* (*orig_fopen)(const char *pathname, const char *mode);

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

FILE* fopen_hook(const char* pathname, const char* mode) {
    std::string pathname_s = std::string(pathname);
    std::string pre = "/data";
    std::string want = "app_payload_lib";

    std::size_t path_index = pathname_s.find(want);

    if ((pathname_s.compare(0, pre.size(), pre) == 0) && (path_index != std::string::npos)) {
        LOGI("fopen(%s, %s) called", pathname, mode);
    }

    return (*orig_fopen)(pathname, mode);
}

int remove_hook(const char* pathname) {
    std::string pathname_s = std::string(pathname);
    std::string pre = "/data";
    std::string want = "app_payload_lib";

    std::size_t path_index = pathname_s.find(want);

    if ((pathname_s.compare(0, pre.size(), pre) == 0) && (path_index != std::string::npos)) {
        LOGI("remove(%s) redirected", pathname);
        fs::path path = fs::path(pathname);
        std::string filename = path.filename();
        std::string parent = pathname_s.substr(0, path_index);
        fs::path dump_dir = fs::path(parent) / "apppeal_dump";
        try {
            fs::create_directory(dump_dir);
        } catch (const fs::filesystem_error& ex) {
            LOGI("Error creating directory %s", ex.what());
        }

        try {
            fs::path dump_path = dump_dir / filename;
            LOGI("Saving %s to %s", pathname, dump_path.c_str());
            fs::rename(pathname, dump_path);
            LOGI("Saved %s to %s", pathname, dump_path.c_str());
        } catch (const fs::filesystem_error& ex) {
            LOGI("Error saving %s, reason: %s", pathname, ex.what());
        }
    }

    return (*orig_remove)(pathname);
}

void attempt_hook(const char* name, void* target, void* hook, void** og) {
    if (target) {
        LOGI("Found alarm at %p", target);
        DobbyHook(target, hook, og);
        LOGI("Hooked alarm");
    } else {
        LOGI("Failed to find %s", name);
    }
}

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("Finding symbols...");
    void *alarm = DobbySymbolResolver(NULL, "alarm");
    void *kill = DobbySymbolResolver(NULL, "kill");
    void *exit = DobbySymbolResolver(NULL, "exit");
    void *remove = DobbySymbolResolver(NULL, "remove");
    void *fopen = DobbySymbolResolver(NULL, "fopen");

    attempt_hook("alarm", alarm, (void *) alarm_hook, (void**) &orig_alarm);
    attempt_hook("kill", kill, (void *) kill_hook, (void**) &orig_kill);
    attempt_hook("exit", exit, (void *) exit_hook, (void**) &orig_exit);
    attempt_hook("remove", remove, (void *) remove_hook, (void**) &orig_remove);
    attempt_hook("fopen", fopen, (void *) fopen_hook, (void**) &orig_fopen);

    return JNI_VERSION_1_6;
}