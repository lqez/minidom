
#ifndef _LIBICONV_H
#define _LIBICONV_H

#ifdef EXPORT_ICONV
	#define EXPORT_ICONV_DECL __declspec(dllexport)
#else
	#define EXPORT_ICONV_DECL
#endif

#define _LIBICONV_VERSION 0x010B    /* version number: (major<<8) + minor */
extern int _libiconv_version; /* Likewise */
/* Define iconv_t ourselves. */
#undef iconv_t
#define iconv_t libiconv_t
typedef void* iconv_t;

/* Get size_t declaration.
   Get wchar_t declaration if it exists. */
#include <stddef.h>

/* Get errno declaration and values. */
#include <errno.h>
#ifndef EILSEQ
#define EILSEQ @EILSEQ@
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBICONV_PLUG
#define iconv_open libiconv_open
#endif
extern iconv_t EXPORT_ICONV_DECL iconv_open (const char* tocode, const char* fromcode);

/* Converts, using conversion descriptor `cd', at most `*inbytesleft' bytes
   starting at `*inbuf', writing at most `*outbytesleft' bytes starting at
   `*outbuf'.
   Decrements `*inbytesleft' and increments `*inbuf' by the same amount.
   Decrements `*outbytesleft' and increments `*outbuf' by the same amount. */
#ifndef LIBICONV_PLUG
#define iconv libiconv
#endif
extern size_t EXPORT_ICONV_DECL iconv (iconv_t cd, const char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft);

/* Frees resources allocated for conversion descriptor `cd'. */
#ifndef LIBICONV_PLUG
#define iconv_close libiconv_close
#endif
extern int EXPORT_ICONV_DECL iconv_close (iconv_t cd);


#ifndef LIBICONV_PLUG

/* Nonstandard extensions. */

/* Control of attributes. */
#define iconvctl libiconvctl
extern int EXPORT_ICONV_DECL iconvctl (iconv_t cd, int request, void* argument);

/* Hook performed after every successful conversion of a Unicode character. */
typedef void (*iconv_unicode_char_hook) (unsigned int uc, void* data);
/* Hook performed after every successful conversion of a wide character. */
typedef void (*iconv_wide_char_hook) (wchar_t wc, void* data);
/* Set of hooks. */
struct iconv_hooks {
  iconv_unicode_char_hook uc_hook;
  iconv_wide_char_hook wc_hook;
  void* data;
};

/* Fallback function.  Invoked when a small number of bytes could not be
   converted to a Unicode character.  This function should process all
   bytes from inbuf and may produce replacement Unicode characters by calling
   the write_replacement callback repeatedly.  */
typedef void (*iconv_unicode_mb_to_uc_fallback)
             (const char* inbuf, size_t inbufsize,
              void (*write_replacement) (const unsigned int *buf, size_t buflen,
                                         void* callback_arg),
              void* callback_arg,
              void* data);
/* Fallback function.  Invoked when a Unicode character could not be converted
   to the target encoding.  This function should process the character and
   may produce replacement bytes (in the target encoding) by calling the
   write_replacement callback repeatedly.  */
typedef void (*iconv_unicode_uc_to_mb_fallback)
             (unsigned int code,
              void (*write_replacement) (const char *buf, size_t buflen,
                                         void* callback_arg),
              void* callback_arg,
              void* data);
//#if @HAVE_WCHAR_T@
///* Fallback function.  Invoked when a number of bytes could not be converted to
//   a wide character.  This function should process all bytes from inbuf and may
//   produce replacement wide characters by calling the write_replacement
//   callback repeatedly.  */
//typedef void (*iconv_wchar_mb_to_wc_fallback)
//             (const char* inbuf, size_t inbufsize,
//              void (*write_replacement) (const wchar_t *buf, size_t buflen,
//                                         void* callback_arg),
//              void* callback_arg,
//              void* data);
///* Fallback function.  Invoked when a wide character could not be converted to
//   the target encoding.  This function should process the character and may
//   produce replacement bytes (in the target encoding) by calling the
//   write_replacement callback repeatedly.  */
//typedef void (*iconv_wchar_wc_to_mb_fallback)
//             (wchar_t code,
//              void (*write_replacement) (const char *buf, size_t buflen,
//                                         void* callback_arg),
//              void* callback_arg,
//              void* data);
//#else
///* If the wchar_t type does not exist, these two fallback functions are never
//   invoked.  Their argument list therefore does not matter.  */
typedef void (*iconv_wchar_mb_to_wc_fallback) ();
typedef void (*iconv_wchar_wc_to_mb_fallback) ();
//#endif
/* Set of fallbacks. */
struct iconv_fallbacks {
  iconv_unicode_mb_to_uc_fallback mb_to_uc_fallback;
  iconv_unicode_uc_to_mb_fallback uc_to_mb_fallback;
  iconv_wchar_mb_to_wc_fallback mb_to_wc_fallback;
  iconv_wchar_wc_to_mb_fallback wc_to_mb_fallback;
  void* data;
};

/* Requests for iconvctl. */
#define ICONV_TRIVIALP            0  /* int *argument */
#define ICONV_GET_TRANSLITERATE   1  /* int *argument */
#define ICONV_SET_TRANSLITERATE   2  /* const int *argument */
#define ICONV_GET_DISCARD_ILSEQ   3  /* int *argument */
#define ICONV_SET_DISCARD_ILSEQ   4  /* const int *argument */
#define ICONV_SET_HOOKS           5  /* const struct iconv_hooks *argument */
#define ICONV_SET_FALLBACKS       6  /* const struct iconv_fallbacks *argument */

/* Listing of locale independent encodings. */
#define iconvlist libiconvlist
extern void EXPORT_ICONV_DECL iconvlist (int (*do_one) (unsigned int namescount,
                                      const char * const * names,
                                      void* data),
                       void* data);

/* Canonicalize an encoding name.
   The result is either a canonical encoding name, or name itself. */

extern EXPORT_ICONV_DECL const char* iconv_canonicalize (const char * name);

/* Support for relocatable packages.  */

/* Sets the original and the current installation prefix of the package.
   Relocation simply replaces a pathname starting with the original prefix
   by the corresponding pathname with the current prefix instead.  Both
   prefixes should be directory names without trailing slash (i.e. use ""
   instead of "/").  */
extern void EXPORT_ICONV_DECL libiconv_set_relocation_prefix (const char *orig_prefix,
					    const char *curr_prefix);

#endif


#ifdef __cplusplus
}
#endif


#endif /* _LIBICONV_H */
