#include <fcntl.h>
#include <unistd.h>

#include "com_notabletsar_app_ControllerActivity.h"

#include "../inc/protocol.h"

static int fd_ = -1;
static int last_mv_dx = -1;
static int last_mv_dy = -1;

JNIEXPORT void JNICALL
Java_com_notabletsar_app_ControllerActivity_setUSBFdJNI (JNIEnv *env, jclass clz, jint fd)
{
  (void)env;
  (void)clz;

  fd_ = fd;
}

JNIEXPORT void JNICALL
Java_com_notabletsar_app_ControllerActivity_closeUSBFdIfAnyJNI (JNIEnv *env, jclass clz)
{
  (void)env;
  (void)clz;

  close (fd_);
}

#define send_buff(fd, pkt) \
  write (fd, pkt, sizeof (*pkt))

JNIEXPORT void JNICALL
Java_com_notabletsar_app_ControllerActivity_doHandShakeJNI (JNIEnv *env, jclass clz, jint scr_w, jint scr_h)
{
  struct shake_pkt pkt = {
    .scr_w = scr_w,
    .scr_h = scr_h,
  };

  (void)env;
  (void)clz;

  send_buff(fd_, &pkt);
}

JNIEXPORT void JNICALL
Java_com_notabletsar_app_ControllerActivity_reportToHostJNI (JNIEnv *env, jclass clz, jint action, jint cur_id, jfloat x, jfloat y)
{
  int dx;
  int dy;
  struct report_pkt pkt;

  (void)env;
  (void)clz;

  dx = x;
  dy = y;

  action &= DROID_ACTION_MASK;
  switch (action)
  {
    case DROID_ACTION_MOVE:
      if (dx == last_mv_dx && dy == last_mv_dy)
        return;  // non-primary finger

      last_mv_dx = dx;
      last_mv_dy = dy;

      // fall-through

    /* primary finger */
    case DROID_ACTION_DOWN:
    case DROID_ACTION_UP:
      pkt.flags = action;
      break;

    /* non-primary fingers */
    case DROID_ACTION_POINTER_DOWN:
    case DROID_ACTION_POINTER_UP:
      pkt.flags = action | cur_id << POINTER_ID_SHIFT;
      break;

    default:
      return;  // discard
  }

  pkt.x = dx;
  pkt.y = dy;

  send_buff(fd_, &pkt);
}
