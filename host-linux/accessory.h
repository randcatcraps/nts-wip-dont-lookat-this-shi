#ifndef __ACCESSORY_UTILS_H__
#define __ACCESSORY_UTILS_H__

#include <libusb.h>

int
to_accessory_mode (int fd,
                   libusb_device_handle **out_accessory);

#endif  /* __ACCESSORY_UTILS_H__ */
