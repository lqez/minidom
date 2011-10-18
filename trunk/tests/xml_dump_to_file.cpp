#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int xml_dump_to_file( int argc, char * argv [] )
{
    minidom::doc dom;
    CHECK_EQUAL( dom.loadFile( minidom::doc::XML, "simple.xml" ), MINIDOM_SUCCESS ); 
    int res = dom.dumpFile( minidom::doc::XML, "dump_simple.xml" );
    //CHECK_EQUAL(  );
    cout << res;
    return 0;
}
