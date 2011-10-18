#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int nkv_dump( int argc, char * argv [] )
{
    minidom::doc dom;
    CHECK_EQUAL( dom.loadFile( minidom::doc::NKV, "test.nkv" ), MINIDOM_SUCCESS ); 
    dom.add( "testment", "true" );

    char buf[4096];
    size_t size = 4096;
    CHECK_EQUAL( dom.dumpString( minidom::doc::NKV, buf, &size ), MINIDOM_SUCCESS );
    CHECK_EQUAL_STR( buf, 
        "2:id=4:lqez&8:password=16:0123456789ABCDEF&6:status=5:crazy&8:testment=4:true" );
    return 0;
}
