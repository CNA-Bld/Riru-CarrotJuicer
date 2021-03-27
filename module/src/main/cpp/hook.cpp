#include "hook.h"
#include <unistd.h>
#include <dlfcn.h>
#include <dobby.h>
#include <fstream>
#include <sys/stat.h>
#include <chrono>

int isGame(JNIEnv *env, jstring appDataDir) {
    if (!appDataDir)
        return 0;
    const char *app_data_dir = env->GetStringUTFChars(appDataDir, nullptr);
    int user = 0;
    static char package_name[256];
    if (sscanf(app_data_dir, "/data/%*[^/]/%d/%s", &user, package_name) != 2) {
        if (sscanf(app_data_dir, "/data/%*[^/]/%s", package_name) != 1) {
            package_name[0] = '\0';
            LOGW("can't parse %s", app_data_dir);
            return 0;
        }
    }
    if (strcmp(package_name, GAME_PACKAGE_NAME) == 0) {
        LOGI("detect game: %s", package_name);
        game_data_dir = new char[strlen(app_data_dir) + 1];
        strcpy(game_data_dir, app_data_dir);
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 1;
    } else {
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 0;
    }
}

std::string current_time() {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    return std::to_string(ms.count());
}

HOOK_DEF(int,
         LZ4_decompress_safe_ext,
         char *src,
         char *dst,
         int compressedSize,
         int dstCapacity) {
    int ret = orig_LZ4_decompress_safe_ext(src, dst, compressedSize, dstCapacity);

    auto outPath = std::string(game_data_dir).append("/files/CarrotJuicer/")
            .append(current_time()).append("R.msgpack");
    std::ofstream outStream(outPath);
    outStream << std::string(dst, dstCapacity);
    outStream.close();
    LOGI("wrote response to %s", outPath.c_str());

    return ret;
}

HOOK_DEF(int,
         LZ4_compress_default_ext,
         char *src,
         char *dst,
         int srcSize,
         int dstCapacity) {
    int ret = orig_LZ4_compress_default_ext(src, dst, srcSize, dstCapacity);

    auto outPath = std::string(game_data_dir).append("/files/CarrotJuicer/")
            .append(current_time()).append("Q.msgpack");
    std::ofstream outStream(outPath);
    outStream << std::string(src, srcSize);
    outStream.close();
    LOGI("wrote request to %s", outPath.c_str());

    return ret;
}

HOOK_DEF(void*,
         __loader_dlopen,
         const char *filename,
         int flags,
         const void *caller_addr) {
    void *handle = orig___loader_dlopen(filename, flags, caller_addr);

    if (!libnative_handle && strstr(filename, "libnative.so")) {
        libnative_handle = handle;
        LOGI("Got libnative handle: %p!", libnative_handle);
    }

    return handle;
}


void *hook_native_thread(void *arg) {
    LOGI("hook_native_thread: %d", gettid());

    void *libdl_handle = dlopen("libdl.so", RTLD_LAZY);
    void *__loader_dlopen_addr = dlsym(libdl_handle, "__loader_dlopen");
    LOGI("__loader_dlopen at: %p", __loader_dlopen_addr);
    DobbyHook(__loader_dlopen_addr, (void *) new___loader_dlopen,
              (void **) &orig___loader_dlopen);

    while (!libnative_handle) {
        sleep(5);
    }

    mkdir(std::string(game_data_dir).append("/files/CarrotJuicer/").c_str(), 0700);

    void *LZ4_decompress_safe_ext_addr =
            dlsym(libnative_handle, "LZ4_decompress_safe_ext");
    LOGI("LZ4_decompress_safe_ext at: %p", LZ4_decompress_safe_ext_addr);
    DobbyHook(LZ4_decompress_safe_ext_addr, (void *) new_LZ4_decompress_safe_ext,
              (void **) &orig_LZ4_decompress_safe_ext);

    void *LZ4_compress_default_ext_addr =
            dlsym(libnative_handle, "LZ4_compress_default_ext");
    LOGI("LZ4_compress_default_ext at: %p", LZ4_compress_default_ext_addr);
    DobbyHook(LZ4_compress_default_ext_addr, (void *) new_LZ4_compress_default_ext,
              (void **) &orig_LZ4_compress_default_ext);

    return nullptr;
}
