#include <jni.h>
#include <android/log.h>
#include <cstring>
#include <string>
#include <vector>
#include <dlfcn.h>
#include "zygisk/api.hpp"
#include "dobby.h"

#define LOG_TAG "SSLZy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Original function pointers
static int (*orig_SSL_set_verify)(void *ssl, int mode, void *callback) = nullptr;
static int (*orig_X509_verify_cert)(void *ctx) = nullptr;

// Hook callbacks
int hook_SSL_set_verify(void *ssl, int mode, void *callback) {
    LOGI("Bypassing SSL_set_verify");
    return orig_SSL_set_verify(ssl, 0, nullptr); // SSL_VERIFY_NONE
}

int hook_X509_verify_cert(void *ctx) {
    LOGI("Bypassing X509_verify_cert");
    return 1; // Return 1 for success
}

class SSLZyModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Get package name
        const char *raw_pkg = nullptr;
        if (args->nice_name) {
            raw_pkg = api->getEnv()->GetStringUTFChars(args->nice_name, nullptr);
        }

        if (raw_pkg) {
            std::string pkg(raw_pkg);
            api->getEnv()->ReleaseStringUTFChars(args->nice_name, raw_pkg);
            
            // Filter: Apply only to specific apps if needed, or all
            // For now, apply globally
            LOGI("Injecting into: %s", pkg.c_str());
            hook_native_functions();
        }
    }

private:
    zygisk::Api *api;

    void hook_native_functions() {
        void *handle = dlopen("libssl.so", RTLD_NOW);
        if (!handle) {
            handle = dlopen("libboringssl.so", RTLD_NOW);
        }
        
        if (handle) {
            void *set_verify = dlsym(handle, "SSL_set_verify");
            void *verify_cert = dlsym(handle, "X509_verify_cert");

            if (set_verify) {
                DobbyHook(set_verify, (void*) hook_SSL_set_verify, (void**) &orig_SSL_set_verify);
                LOGI("Hooked SSL_set_verify");
            }
            if (verify_cert) {
                DobbyHook(verify_cert, (void*) hook_X509_verify_cert, (void**) &orig_X509_verify_cert);
                LOGI("Hooked X509_verify_cert");
            }
        } else {
            LOGE("Failed to load libssl.so or libboringssl.so");
        }
    }
};

REGISTER_ZYGISK_MODULE(SSLZyModule)