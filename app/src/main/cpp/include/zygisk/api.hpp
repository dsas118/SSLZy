#pragma once
#include <jni.h>
#include <string>
#include <vector>

#define ZYGISK_API_VERSION 4

namespace zygisk {

struct Api;
struct AppSpecializeArgs;
struct ServerSpecializeArgs;

class ModuleBase {
public:
    virtual void onLoad(Api *api, JNIEnv *env) {}
    virtual void preAppSpecialize(AppSpecializeArgs *args) {}
    virtual void postAppSpecialize(const AppSpecializeArgs *args) {}
    virtual void preServerSpecialize(ServerSpecializeArgs *args) {}
    virtual void postServerSpecialize(const ServerSpecializeArgs *args) {}
};

struct AppSpecializeArgs {
    jstring &nice_name;
    jstring &app_data_dir;
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jobjectArray &rlimits;
    jint &mount_external;
    jstring &se_info;
    jstring &instruction_set;
    jstring &app_data_dir_creat;
    jstring &app_data_dir_creat2;
    jboolean &is_child_zygote;
    jboolean &is_top_app;
    jstring &package_name;
    jstring &app_data_dir_creat3;
    jstring &app_data_dir_creat4;
};

struct ServerSpecializeArgs {
    jint &uid;
    jint &gid;
    jintArray &gids;
    jint &runtime_flags;
    jlong &permitted_capabilities;
    jlong &effective_capabilities;
};

struct Api {
    int (*getApiVersion)();
    void (*setOption)(int);
    void (*hookJniNativeMethods)(JNIEnv *, const char *, JNINativeMethod *, int);
    void (*pltHookRegister)(const char *, const char *, void *, void **);
    void (*pltHookExclude)(const char *, const char *);
    bool (*pltHookCommit)();
    int (*connectCompanion)(int);
    int (*getModuleDir)(int);
    void (*exemptFd)(int);
};

} // namespace zygisk

#define REGISTER_ZYGISK_MODULE(clazz) \
extern "C" [[gnu::visibility("default")]] void zygisk_module_entry( \
    zygisk::Api *api, JNIEnv *env) { \
    static clazz module; \
    module.onLoad(api, env); \
} \
extern "C" [[gnu::visibility("default")]] void zygisk_module_pre_app_specialize( \
    zygisk::AppSpecializeArgs *args) { \
    static clazz module; \
    module.preAppSpecialize(args); \
} \
extern "C" [[gnu::visibility("default")]] void zygisk_module_post_app_specialize( \
    const zygisk::AppSpecializeArgs *args) { \
    static clazz module; \
    module.postAppSpecialize(args); \
}