#include <locale.h>
#include <stddef.h>

#include <libusb.h>

#include "config.h"
#include "utils.h"

#ifndef NDEBUG
# include <stdio.h>

# include "../libusb_stuffs.h"
#endif

#ifdef ENABLE_NLS
# include <libintl.h>
#endif

void
l10n_init (void)
{
  int err;

  // follow system
  if (!setlocale (LC_ALL, ""))
  {
#ifndef NDEBUG
    fprintf (stderr, "setlocale() failed\n");
#endif
    return;
  }

  err = libusb_setlocale (setlocale (LC_MESSAGES, NULL));
#ifdef NDEBUG
  (void)err;
#else
  if (err < 0)
    libusb_perror (err, "libusb_setlocale() failed");
#endif

#ifdef ENABLE_NLS
  if (!bindtextdomain (PACKAGENAME, LOCALEDIR))
  {
#ifndef NDEBUG
    perror ("bindtextdomain() failed");
#endif
    return;
  }

  if (!textdomain (PACKAGENAME))
  {
#ifndef NDEBUG
    perror ("textdomain() failed");
#endif
    return;
  }
#endif
}
