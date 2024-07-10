#ifndef __NLS_WRAPPER_H__
#define __NLS_WRAPPER_H__

#ifdef ENABLE_NLS
# include <libintl.h>

# define _(msg)   gettext (msg)
#else
# define _(msg)   msg
#endif

#endif  /* __NLS_WRAPPER_H__ */
