/*

    minidom - a minimized dom/path library
    See README for copyright and license information.

*/

#ifndef _MINIDOM_ERROR_H_
#define _MINIDOM_ERROR_H_

#define MINIDOM_ERROR                                           0
#define MINIDOM_SUCCESS                                         1

// Error
#define MINIDOM_ERROR_FileNotFound                              -1
#define MINIDOM_ERROR_FileOpenError                             -2
#define MINIDOM_ERROR_FileWriteError                            -3
#define MINIDOM_ERROR_FileReadError                             -4
#define MINIDOM_ERROR_PermissionDenied                          -5
#define MINIDOM_ERROR_ReadOnlyFile                              -6
#define MINIDOM_ERROR_FailToGetFiletime                         -7
#define MINIDOM_ERROR_NotValidFileName                          -8
#define MINIDOM_ERROR_CannotCreateFile                          -11
#define MINIDOM_ERROR_ZeroFileSize                              -14
#define MINIDOM_ERROR_FailToSetFiletime                         -17
#define MINIDOM_ERROR_NotValidXMLFormat                         -20
#define MINIDOM_ERROR_NonAllocatedBuffer                        -28
#define MINIDOM_ERROR_NullPointerBuffer                         -29
#define MINIDOM_ERROR_AlreadyAllocated                          -31
#define MINIDOM_ERROR_SourceStringisNull                        -44
#define MINIDOM_ERROR_CannotCopyToItself                        -45
#define MINIDOM_ERROR_FileAlreadyExists                         -46
#define MINIDOM_ERROR_CannotCreateIconvInstance                 -50
#define MINIDOM_ERROR_UnsupportedEncoding                       -51
#define MINIDOM_ERROR_UnsupportedDocumentType                   -52
#define MINIDOM_ERROR_NotEnoughBuffer                           -53
#define MINIDOM_ERROR_NotValidDocumentFormat                    -54
#define MINIDOM_ERROR_DumpWasNotEnabled                         -99
#define MINIDOM_ERROR_NotEnoughMemory                           -100
#define MINIDOM_ERROR_NotEnoughDiscSpace                        -101
#define MINIDOM_ERROR_CancelByCallback                          -200

#endif /* _MINIDOM_ERROR_H_ */
