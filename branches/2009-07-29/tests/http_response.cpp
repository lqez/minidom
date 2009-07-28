#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int http_response( int argc, char * argv [] )
{
	minidom::doc dom;
	CHECK_EQUAL( dom.loadFile( minidom::doc::HTTP, "http_response.txt" ), MINIDOM_SUCCESS ); 
	CHECK_EQUAL_STR( dom.get( "result" )->toChars(), "HTTP/1.1 200 OK" );
	dom.print( cout, false );
	return 0;
}
