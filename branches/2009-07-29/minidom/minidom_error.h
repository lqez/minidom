/*

minidom - a minimized dom/path library

Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)
http://studio.amiryo.com/minidom

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef _MINIDOM_ERROR_H_
#define _MINIDOM_ERROR_H_

#define MINIDOM_ERROR											0
#define MINIDOM_SUCCESS											1

// Error
#define	MINIDOM_ERROR_FileNotFound								-1
#define	MINIDOM_ERROR_FileOpenError								-2
#define	MINIDOM_ERROR_FileWriteError							-3
#define	MINIDOM_ERROR_FileReadError								-4
#define	MINIDOM_ERROR_PermissionDenied							-5
#define	MINIDOM_ERROR_ReadOnlyFile								-6
#define	MINIDOM_ERROR_FailToGetFiletime							-7
#define	MINIDOM_ERROR_NotValidFileName							-8
#define	MINIDOM_ERROR_CannotCreateFile							-11
#define	MINIDOM_ERROR_ZeroFileSize								-14
#define	MINIDOM_ERROR_FailToSetFiletime							-17
#define MINIDOM_ERROR_NotValidXMLFormat							-20
#define	MINIDOM_ERROR_NonAllocatedBuffer						-28
#define	MINIDOM_ERROR_NullPointerBuffer							-29
#define	MINIDOM_ERROR_AlreadyAllocated							-31
#define MINIDOM_ERROR_SourceStringisNull						-44
#define MINIDOM_ERROR_CannotCopyToItself						-45
#define	MINIDOM_ERROR_FileAlreadyExists							-46
#define	MINIDOM_ERROR_CannotCreateIconvInstance					-50
#define	MINIDOM_ERROR_UnsupportedEncoding						-51
#define	MINIDOM_ERROR_UnsupportedDocumentType					-52
#define MINIDOM_ERROR_NotEnoughBuffer							-53
#define	MINIDOM_ERROR_NotValidDocumentFormat					-54
#define MINIDOM_ERROR_DumpWasNotEnabled							-99
#define	MINIDOM_ERROR_NotEnoughMemory							-100
#define	MINIDOM_ERROR_NotEnoughDiscSpace						-101
#define	MINIDOM_ERROR_CancelByCallback							-200

#endif /* _MINIDOM_ERROR_H_ */
