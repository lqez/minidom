#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int json_dump( int argc, char * argv [] )
{
    minidom::doc dom;
    CHECK_EQUAL( dom.loadFile( minidom::doc::JSON, "contact.json" ), MINIDOM_SUCCESS ); 

    char buf[4096];
    size_t size = 4096;
    CHECK_EQUAL( dom.dumpString( minidom::doc::JSON, buf, &size ), MINIDOM_SUCCESS );

    cout << "dump result:\n" << buf << "\n";

    CHECK_EQUAL_STR_WITH_FILE( buf, "contact.json" );
    return 0;
}
