/*

    minidom - php serialized object parser
    See README for copyright and license information.

*/

namespace minidom
{
    int doc::writePHP( char* buf, size_t* size, void* conv )
    {
#if defined( MINIDOM_ENABLE_DUMP )
        return MINIDOM_ERROR_UnsupportedDocumentType;
#else /* MINIDOM_ENABLE_DUMP */
        return MINIDOM_ERROR_DumpWasNotEnabled;
#endif
    }

    int doc::parsePHP( const char* buf )
    {
        return MINIDOM_ERROR_UnsupportedDocumentType;
    }
}

