#ifndef __NTS_PROTOCOL_H__
#define __NTS_PROTOCOL_H__

#include <stdint.h>

struct shake_pkt {
  uint16_t scr_w;
  uint16_t scr_h;
};

// https://developer.android.com/reference/android/view/MotionEvent#constants
#define DROID_ACTION_DOWN               0x0000
#define DROID_ACTION_UP                 0x0001
#define DROID_ACTION_MOVE               0x0002
#define DROID_ACTION_POINTER_DOWN       0x0005
#define DROID_ACTION_POINTER_UP         0x0006
#define DROID_ACTION_MASK               0x00ff

#define POINTER_ID_K_X                  1
#define POINTER_ID_K_Z                  2

#define POINTER_ID_MASK                 0b11110000
#define POINTER_ID_SHIFT                4

struct report_pkt {
  uint16_t x;
  uint16_t y;
  char flags;
};

#endif  /* __NTS_PROTOCOL_H__ */
