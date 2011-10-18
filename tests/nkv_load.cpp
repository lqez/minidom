#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int nkv_load( int argc, char * argv [] )
{
    minidom::doc dom;
    CHECK_EQUAL( dom.loadFile( minidom::doc::NKV, "test.nkv" ), MINIDOM_SUCCESS ); 
    dom.print( cout, false );
    return 0;
}
