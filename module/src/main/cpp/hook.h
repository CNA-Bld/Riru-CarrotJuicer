#ifndef RIRU_CARROTJUICER_HOOK_H
#define RIRU_CARROTJUICER_HOOK_H

#include <jni.h>
#include "log.h"

static int enable_hack;
static void* libnative_handle = NULL;
static char* game_data_dir = NULL;

int isGame(JNIEnv* env, jstring appDataDir);

void* hook_native_thread(void* arg);

#define GAME_PACKAGE_NAME "jp.co.cygames.umamusume"

#define HOOK_DEF(ret, func, ...) \
  ret (*orig_##func)(__VA_ARGS__); \
  ret new_##func(__VA_ARGS__)

#endif //RIRU_CARROTJUICER_HOOK_H
