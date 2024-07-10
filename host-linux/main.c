#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include <libusb.h>

#include "accessory.h"
#include "dlg_lsusb.h"
#include "l10n/utils.h"
#include "libusb_stuffs.h"
#include "recv.h"
#include "uinput.h"

static libusb_device_handle *accessory_ = NULL;

static void
_cleanup (void)
{
  if (accessory_)
    libusb_close (accessory_);

  libusb_exit (LIBUSB_CTX_DEF);
}

static void
_quit_normally (int signo)
{
  (void)signo;
  exit (0);
}

static inline void
_reg_sig_handlers (void)
{
  int i;
  int quit_sigs[] = { SIGHUP,  SIGINT,  SIGPIPE,
                      SIGQUIT, SIGTERM, 0 };

  for (i = 0; quit_sigs[i]; )
    signal (quit_sigs[i++], &_quit_normally);
}

int
main ()
{
  int err;
  int fd;

  // init locales
  l10n_init ();

  if (init_nts_drv () < 0)
    return 1;

  err = libusb_init_context (LIBUSB_CTX_DEF,
                             NULL,  /* options */
                             0  /* num_options */);
  if (err < 0)
  {
    libusb_perror (err, "libusb_init_context() failed");
    return 1;
  }

  atexit (&_cleanup);
  _reg_sig_handlers ();

  fd = dlg_sel_usb ();
  if (fd < 0)
    return 1;  // err or cancelled

  err = to_accessory_mode (fd, &accessory_);

  close (fd);

  if (err < 0)
    return 1;

  return nts_host_main (accessory_);
}
