#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int xml_get( int argc, char * argv [] )
{
	minidom::doc dom;
	CHECK_EQUAL( dom.loadFile( minidom::doc::XML, "inventory.xml" ), MINIDOM_SUCCESS ); 
	CHECK_EQUAL_STR( dom.get( "author/first-name" )->toChars(), "Joe" );
	return 0;
}
