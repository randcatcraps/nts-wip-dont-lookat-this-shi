#include "com_example_app_MainActivity.h"

JNIEXPORT jint JNICALL
Java_com_example_app_MainActivity_getNum (JNIEnv *env, jobject thiz)
{
  // do stuffs here
  return 3 + 4;
}
