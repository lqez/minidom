#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int ini_dump( int argc, char * argv [] )
{
	minidom::doc dom;
	CHECK_EQUAL( dom.loadFile( minidom::doc::INI, "setting.ini" ), MINIDOM_SUCCESS ); 

	char buf[4096];
	size_t size = 4096;
	CHECK_EQUAL( dom.dumpString( minidom::doc::INI, buf, &size ), MINIDOM_SUCCESS );

	CHECK_EQUAL_STR_WITH_FILE( buf, "setting.ini" );
	return 0;
}
