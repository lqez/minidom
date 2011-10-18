#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int xml_dump( int argc, char * argv [] )
{
    minidom::doc dom;
    CHECK_EQUAL( dom.loadFile( minidom::doc::XML, "simple.xml" ), MINIDOM_SUCCESS ); 

    char buf[4096];
    size_t size = 4096;
    CHECK_EQUAL( dom.dumpString( minidom::doc::XML, buf, &size ), MINIDOM_SUCCESS );

    CHECK_EQUAL_STR_WITH_FILE( buf, "simple.xml" );
    return 0;
}
