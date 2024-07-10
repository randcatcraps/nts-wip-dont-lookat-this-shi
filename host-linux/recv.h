#ifndef __NTS_HOST_H__
#define __NTS_HOST_H__

#include <libusb.h>

int
nts_host_main (libusb_device_handle *accessory);

#endif  /* __NTS_HOST_H__ */
