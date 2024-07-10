#ifndef  _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <dialog.h>
#include <libusb.h>

#include "../inc/android_accessory.h"
#include "dlg_lsusb.h"
#include "l10n/nls_wrap.h"
#include "libusb_stuffs.h"

#define MAX_DEVICES         64

#define MAX_NAME_LEN        128

#define INFO_UNKNOWN        _("Unknown")

#define NODE_FMT            "%03d/%03d"

#define NODE_LEN            (3 + 1 + 3 + 1)

#define DISP_NAME_FMT       "%04x:%04x %s - %s"

#define DISP_NAME_LEN       (4 + 1 + 4 + 1 + MAX_NAME_LEN + 1 + 1 + 1 + MAX_NAME_LEN + 1)

#define STRTAB_SIZ          (MAX_DEVICES * (NODE_LEN + DISP_NAME_LEN))

// special values
#define DLG_W_AUTO          0
#define DLG_H_AUTO          0
#define DLG_MENU_H_AUTO     0

#define DLG_INTERACTIVE     true

#define LINUX_USB_BUS_DIR   "/dev/bus/usb"

typedef char *names_t[2 * MAX_DEVICES];

static inline int
_fmt_dev_lst (names_t arr, char s[STRTAB_SIZ],
              int *num, bool *trunc, bool *accessory)
{
  int err;
  libusb_device **usbs;
  int i;
  libusb_device *usb;
  int j;
  uint8_t bus;
  uint8_t addr;
  struct libusb_device_descriptor desc;
  libusb_device_handle *dev;
  char manufacturer[MAX_NAME_LEN];
  char product[MAX_NAME_LEN];
  int len;

  err = libusb_get_device_list (LIBUSB_CTX_DEF, &usbs);
  if (err < 0)
  {
    libusb_perror (err, "libusb_get_device_list() failed");
    return -1;
  }

  j = 0;
  for (i = 0; (usb = usbs[i]); i++)
  {
    if (2 * MAX_DEVICES == j)
    {
      // too many devices
      *trunc = true;
      break;
    }

    bus = libusb_get_bus_number (usb);
    addr = libusb_get_device_address (usb);

    err = libusb_get_device_descriptor (usb, &desc);
    if (err < 0)
    {
#ifndef NDEBUG
      libusb_perror(err, "libusb_get_device_descriptor() %03d/%03d failed", bus, addr);
#endif
      continue;
    }

    err = libusb_open (usb, &dev);
    if (err < 0)
    {
#ifndef NDEBUG
      libusb_perror(err, "libusb_open() %03d/%03d failed", bus, addr);
#endif
      continue;
    }

    err = libusb_get_string_descriptor_ascii (dev, desc.iManufacturer,
                                              (void *)manufacturer, sizeof(manufacturer));
    if (err < 0)
    {
#ifndef NDEBUG
      libusb_perror(err, "libusb_get_string_descriptor_ascii() manufacturer %03d/%03d failed", bus, addr);
#endif
      strcpy (manufacturer, INFO_UNKNOWN);
    }

    if (desc.idVendor == VID_ACCESSORY
        && (desc.idProduct == PID_ACCESSORY
            || desc.idProduct == PID_ACCESSORY_WITH_ADB))
      *accessory = true;

    err = libusb_get_string_descriptor_ascii (dev, desc.iProduct,
                                              (void *)product, sizeof(product));
    if (err < 0)
    {
#ifndef NDEBUG
      libusb_perror(err, "libusb_get_string_descriptor_ascii() product %03d/%03d failed", bus, addr);
#endif
      strcpy (product, INFO_UNKNOWN);
    }

    sprintf (s, NODE_FMT, bus, addr);
    arr[j++] = s;
    s += NODE_LEN + 1;

    len = sprintf (s, DISP_NAME_FMT, desc.idVendor, desc.idProduct, manufacturer, product);
    arr[j++] = s;
    s += len + 1;

    libusb_close (dev);
  }

  *num = j / 2;

  libusb_free_device_list (usbs, true);

  return 0;
}

int
dlg_sel_usb (void)
{
  char *strtab;
  int dfd;
  names_t lst;
  int ndevs;
  bool trunc = false;
  bool accessory = false;
  int fd = -1;

  strtab = malloc (STRTAB_SIZ);
  if (!strtab)
  {
    perror ("malloc() strtab failed");
    return -1;
  }

  dfd = open (LINUX_USB_BUS_DIR, O_PATH | O_DIRECTORY);
  if (dfd < 0)
    perror ("open() usb busdir failed");

  if (_fmt_dev_lst (lst, strtab, &ndevs, &trunc, &accessory) < 0)
    goto fail;

  init_dialog (stdin, stdout);
  dialog_vars.ok_label = _("OK");
  dialog_vars.erase_on_exit = true;

  if (!ndevs)
  {
    dialog_msgbox (_("Info"),
                   _("No available USB device was found."
                     " (Maybe try to unlock your device's lockscreen first."
                     " If you're sure the device is connected,"
                     " it's probably due to you do not have the sufficient permission.)"
                     "\n\n"
                     "Now press OK to quit."),
                   DLG_H_AUTO, DLG_W_AUTO, DLG_INTERACTIVE);
    goto out;
  }

  if (accessory)
  {
    dialog_msgbox (_("Warning: Problem detected!"),
                   _("NOTE: One or more connected USB devices are already in accessory mode."
                     "\n\n"
                     "If they are unrelated devices, please remove them;\n"
                     "If it is the target device, please disconnect and reconnect."
                     "\n\n"
                     "When finished, please restart this program.\n"
                     "Now press OK to quit."),
                   DLG_H_AUTO, DLG_W_AUTO, DLG_INTERACTIVE);
    goto out;
  }

  if (trunc)
    dialog_msgbox (_("Warning!"),
                   _("Too many USB devices were found,"
                     " some of them might not be displayed in the list."),
                   DLG_H_AUTO, DLG_W_AUTO, DLG_INTERACTIVE);

  if (dialog_menu (_("List of currently connected USB device(s):"),
                   _("Please select an Android device... (No hot-plug)"),
                   DLG_H_AUTO, DLG_W_AUTO, DLG_MENU_H_AUTO, ndevs, lst))
    goto out;  // cancelled

  fd = openat (dfd, dialog_vars.input_result, O_RDWR);
  if (fd < 0)
  {
    dialog_msgbox (_("Error opening device!"),
                   strerror (errno),
                   DLG_H_AUTO, DLG_W_AUTO, DLG_INTERACTIVE);
    goto out;
  }

out:
  end_dialog ();

fail:
  free (strtab);
  close (dfd);

  return fd;
}
