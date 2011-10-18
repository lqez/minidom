/*

    minidom - nkv parser
    See README for copyright and license information.

    nkv means 'Numbered Key=Value format'.
    Example:
        #:KEY=#:VALUE&...
        2:id=4:lqez&8:password=...

*/

namespace minidom
{
    int doc::writeNKV( char* buf, size_t* size, void* conv )
    {
#if defined( MINIDOM_ENABLE_DUMP )
#if defined( MINIDOM_SUPPORT_NKV )
        size_t s = 0;
        size_t len;
        char tmp[minidom_buffer_size];
#if defined( MINIDOM_SUPPORT_ICONV )
        size_t len_conv;
        char tmp_conv[minidom_buffer_size*2];
#endif
        list<node*> nodes;
        nodes.push_back( this );
        while( nodes.size() != 0 )
        {
            node* a = nodes.front();
            for( NVI iter = a->childVec_.begin(); iter != a->childVec_.end(); ++iter )
                nodes.push_back( *iter );
            /* Note: NKV does not support 'attribute'
            for( NLI iter = NL(a->attrList_)->begin(); iter != NL(a->attrList_)->end(); ++iter )
                nodes.push_back( *iter );
            */
            if( a->k_.size() != 0 )
            {
                if( s == 0 )
                    sprintf( tmp, "%d:%s=%d:%s",
                        (int)a->k_.size(), a->k_.c_str(), (int)a->v_.size(), a->v_.c_str() );
                else
                    sprintf( tmp, "&%d:%s=%d:%s",
                        (int)a->k_.size(), a->k_.c_str(), (int)a->v_.size(), a->v_.c_str() );

                len = strlen(tmp);
#if defined( MINIDOM_SUPPORT_ICONV )
                const char * original_tmp_conv = tmp_conv;
                iconv( conv, &tmp, len, &tmp_conv, &len_conv );
                *len_conv = tmp_conv - original_tmp_conv;
                *tmp_conv = '\0';
                swap( tmp, original_tmp_conv );
#endif
                if( *size )
                {
                    if( s + len >= *size ) 
                        return MINIDOM_ERROR_NotEnoughBuffer;

                    strncpy( buf, tmp, len );
                    buf += len;
                }
#if defined( MINIDOM_SUPPORT_ICONV )
                swap( tmp, original_tmp_conv );
#endif
                s += len;
            }
            nodes.pop_front();
        }
        if( *size )
            *buf = '\0';
        *size = s;
        return MINIDOM_SUCCESS;
#else /* MINIDOM_SUPPORT_NKV */
        return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
#else /* MINIDOM_ENABLE_DUMP */
        return MINIDOM_ERROR_DumpWasNotEnabled;
#endif
    }

    int doc::parseNKV( const char* buf )
    {
#if defined( MINIDOM_SUPPORT_NKV )
        enum {
            KEY_LENGTH,
            KEY,
            EQUAL,
            VALUE_LENGTH,
            VALUE,
            AMPERSAND_OR_NULL,
        };

        int status = KEY_LENGTH;
        char c;

        size_t length = 0;
        char tmp[minidom_buffer_size];
        bool bError = false;
        bool bEnd = false;
        string strKV;
        string strLength;

        node* elemNode = this, *newNode = NULL;
        nodeVec_.push_back( this );
        
        while( *buf )
        {
            c = *buf;
            switch( status )
            {
            case KEY_LENGTH:
            case VALUE_LENGTH:
                if( c == ':' )
                {
                    if( status == KEY_LENGTH )
                        status = KEY;
                    else
                        status = VALUE;
                    length = atoi( strLength.c_str() );
                    strLength = "";
                }
                else
                {
                    if( ( c < '0' ) || ( c > '9' ) )
                    {
                        bError = true;
                        break;
                    }
                    strLength += c;
                }
                ++buf;
                break;
            case KEY:
                strncpy( tmp, buf, length );
                tmp[length] = '\0';
                strKV = tmp;
                buf += length;

                elemNode = elemNode->add( convertString(strKV) );
                status = EQUAL;
                break;
            case EQUAL:
                if( c != '=' )
                {
                    bError= true;
                    break;
                }
                status = VALUE_LENGTH;
                ++buf;
                break;
            case VALUE:
                strncpy( tmp, buf, length );
                tmp[length] = '\0';
                strKV = tmp;
                buf += length;

                elemNode->v_ = convertString(strKV);
                status = AMPERSAND_OR_NULL;
                break;
            case AMPERSAND_OR_NULL:
                elemNode = elemNode->parent_;
                if( c == '&' )
                    status = KEY_LENGTH;
                else if( ( c == '\0' ) || ( c == '\n' ) || ( c == '\r' ) )
                    bEnd = true;
                else
                    bError = true;
                ++buf;
                break;
            }
            if( bError )
                return MINIDOM_ERROR_NotValidDocumentFormat;
            if( bEnd )
                break;
        }

        return MINIDOM_SUCCESS;
#else /* MINIDOM_SUPPORT_NKV */
        return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
    }
}

