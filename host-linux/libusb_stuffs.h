#ifndef __LIBUSB_STUFFS_H__
#define __LIBUSB_STUFFS_H__

#include <stddef.h>
#include <stdio.h>

#include <libusb.h>

// special value
#define LIBUSB_CTX_DEF    NULL

#define libusb_perror(err, fmt, ...)                    \
{                                                       \
  fprintf (stderr, fmt, ##__VA_ARGS__);                 \
  fprintf (stderr, ": %s\n", libusb_strerror (err));    \
}

#endif  /* __LIBUSB_STUFFS_H__ */
