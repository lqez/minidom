#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int xml_query( int argc, char * argv [] )
{
    minidom::doc dom;
    CHECK_EQUAL( dom.loadFile( minidom::doc::XML, "inventory.xml" ), MINIDOM_SUCCESS ); 

    minidom::selector s = dom.query( "book" );
    CHECK_EQUAL( s.size(), 4 );
    CHECK_EQUAL_STR( s.at(1)->get("@style")->toChars(), "textbook" ); 
    s.printResult();

    s = s.query( "/author/degree" );
    CHECK_EQUAL( s.size(), 2 );
    CHECK_EQUAL_STR( s.at(0)->get( "@from" )->toChars(), "Trenton U" );

    return 0;
}
