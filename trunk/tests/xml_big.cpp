#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"
#include <time.h>

/*
	Create 'big.xml' on tests directory before running the test.
	Use 'junkxml.cpp' to create a big junk xml file.
*/

int xml_big( int argc, char * argv [] )
{
	clock_t start = clock();

	minidom::doc dom;
	CHECK_EQUAL( dom.loadFile( minidom::doc::XML, "big.xml" ), MINIDOM_SUCCESS ); 

	std::cout << ((clock()-start)*1000) / CLOCKS_PER_SEC << "ms\r\n";
	return 0;
}
