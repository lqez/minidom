#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int xml_utf8_to_euckr( int argc, char * argv [] )
{
	minidom::doc dom;
	CHECK_EQUAL( dom.loadFile( minidom::doc::XML, "utf8.xml", "euc-kr" ), MINIDOM_SUCCESS ); 
	dom.print( cout, true );
	CHECK_EQUAL_STR( dom.get( "/text/hangul" )->toChars(), "ÇÑ±Û" );
	return 0;
}
