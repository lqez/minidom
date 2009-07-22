/* testutil.h from haje01's GX library */
#ifndef __TESTUTIL_H__
#define __TESTUTIL_H__

#include <iostream>
#include <fcntl.h>
#if defined(MINIDOM_PLATFORM_WINDOWS)
#include <io.h>
#pragma warning ( disable : 4819 )
#pragma warning ( disable : 4996 )
#else
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

using namespace std;

#if defined( WIN32 )
#define CHECK(x) \
	{if(!(x)) { printf("Assertion Failed : %s, %d\n",__FILE__,__LINE__); \
		cout << flush; \
		_asm{int 0x03}}}
#else
#define CHECK(x) \
	{if(!(x)) { printf("Assertion Failed : %s, %d\n",__FILE__,__LINE__); \
		cout << flush; \
		__asm__("int $0x03");}}
#endif

#define CHECK_CLOSE( M_A, M_B, M_EPSILON ) \
	CHECK( fabs( M_A - M_B ) < M_EPSILON );
#define CHECK_EQUAL( M_A, M_B ) \
	CHECK( M_A == M_B )
#define CHECK_EQUAL_STR( M_A, M_B ) \
	CHECK( strcmp((M_A), (M_B)) == 0 )

static void CHECK_EQUAL_STR_WITH_FILE( const char* src, const char* filename )
{
	int		h;
	char*	buf;
	size_t	size;
	
	h = open( filename, O_RDONLY | O_BINARY );
	CHECK( h >= 0 );

	size = lseek( h, 0, SEEK_END );
	CHECK( size != 0 );

	lseek( h, 0, SEEK_SET );

	buf = (char*)malloc( size+1 );
	CHECK( buf != NULL );

	// put '\0' at last
	((char*)buf)[size] = '\0';

	CHECK_EQUAL( size, read( h, buf, size ) );

	CHECK_EQUAL_STR( buf, buf );

	free( buf );
	close( h );
}

#endif // __TESTUTIL_H__
