#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"
#include <time.h>
#if defined(MINIDOM_PLATFORM_WINDOWS)
#include <sys/stat.h>
#endif


int xml_big( int argc, char * argv [] )
{
#if defined(MINIDOM_PLATFORM_WINDOWS)
	struct _stat s;
	if( 0 != _stat( "big.xml", &s ) )
#else
	struct stat s;
	if( 0 != stat( "big.xml", &s ) )
#endif
	{
/*
		Create 'big.xml' on tests directory before running the test.
		Use 'junkxml.cpp' to create a big junk xml file.
*/
		return 0;
	}

	clock_t start = clock();

	minidom::doc dom;
	CHECK_EQUAL( dom.loadFile( minidom::doc::XML, "big.xml" ), MINIDOM_SUCCESS ); 

	std::cout << ((clock()-start)*1000) / CLOCKS_PER_SEC << "ms\r\n";
	return 0;
}
