#include "../minidom/minidom.h"
#include "../minidom/minidom_error.h"
#include "testutil.h"

int ini_get( int argc, char * argv [] )
{
	minidom::doc dom;
	CHECK_EQUAL( dom.loadFile( minidom::doc::INI, "setting.ini" ), MINIDOM_SUCCESS ); 
	CHECK_EQUAL_STR( dom.get( "LAST_GAME/LAST_LVS_PVP_HUNT" )->toChars(), "P_howe_hunt" );
	CHECK_EQUAL_STR( dom.get( "GRAPHIC/RESOLUTION" )->toChars(), "800 600" );
	return 0;
}
