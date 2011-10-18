/*

    minidom - http header parser
    Copyright (c) 2009-2011 Park Hyun woo(ez.amiryo@gmail.com)

    See README for copyright and license information.

*/

namespace minidom
{
    int doc::writeHTTP( char* buf, size_t* size, void* conv )
    {
        /* minidom doesn't support to dump 'http header' format. */
        return MINIDOM_ERROR_UnsupportedDocumentType;
    }

    int doc::parseHTTP( const char* buf )
    {
#if defined( MINIDOM_SUPPORT_HTTP )
        enum {
            PROTOCOL,
            VERSION,
            CODE,
            DESCRIPTION,
            KEY,
            VALUE,
            CRLF,
            BOUNDARY,
            CONTENT,
        };

        int status = PROTOCOL;
        char c;

        const char* offset = buf;
        bool bError = false;
        bool bEnd = false;
        string strK;
        string strV;

        node* elemNode = NULL;
        nodeVec_.push_back( this );
        strK = "result";
        node* resultNode = add( convertString(strK) );
        strK = "header";
        node* headerNode = add( convertString(strK) );
        strK = "content";
        node* contentNode = add( convertString(strK) );
        strK.clear();
        
        while( *buf )
        {
            c = *buf;
            switch( status )
            {
            case PROTOCOL:
            case VERSION:
            case CODE:
            case DESCRIPTION:
                if( c == '\r' )
                {
                    strK = "description";
                    elemNode = resultNode->add( convertString(strK) );
                    elemNode->v_ = convertString(strV);

                    resultNode->v_ = get("/result/protocol")->toString()
                        + "/"
                        + get("/result/version")->toString()
                        + " "
                        + get("/result/code")->toString()
                        + " "
                        + strV;
                    strK.clear();
                    strV.clear();
                    status = CRLF;
                    break;
                }
                else if( ( c == '/' ) || ( c == ' ' ) )
                {
                    strK = (status==PROTOCOL)?"protocol":((status==VERSION)?"version":"code");
                    elemNode = resultNode->add( convertString(strK) );
                    elemNode->v_ = convertString(strV);
                    strK.clear();
                    strV.clear();
                    status = (status==PROTOCOL)?VERSION:((status==VERSION)?CODE:DESCRIPTION);
                }
                else
                    strV += c;
                break;
            case KEY:
                if( strK.size() == 0 )
                {
                    if( c == '\r' )
                    {
                        status = BOUNDARY;
                        break;
                    }

                    if( ( c == ' ' ) || ( c == '\t' ) )
                        break;
                }

                if( c != ':' )
                    strK += c;
                else
                {
                    elemNode = headerNode->add( convertString(strK) );
                    strK.clear();
                    status = VALUE;
                }
                break;
            case VALUE:
                if( strV.size() == 0 )
                {
                    if( ( c == ' ' ) || ( c == '\t' ) )
                        break;
                }

                if( c == '\r' )
                {
                    elemNode->v_ = convertString(strV);
                    strV.clear();
                    status = CRLF;
                    break;
                }
                else
                    strV += c;
                break;
            case CRLF:
            case BOUNDARY:
                if( c == '\n' )
                    status = (status==BOUNDARY)?CONTENT:KEY;
                else
                    bError = true;
                break;
            case CONTENT:
                {
                    char bufSize[64];
                    sprintf( bufSize, "%d", (int)(buf-offset) );
                    strK = "offset";
                    strV = bufSize;
                    elemNode = contentNode->add( convertString(strK), true );
                    elemNode->v_ = convertString(strV);
                //  int contentLength = get( "/header/Content-Length" )->toInt();
                //  if( contentLength > 0 )
                //      contentNode->v_.assign( buf, contentLength );
                }
                bEnd = true;
                break;
            }
            if( bError )
                return MINIDOM_ERROR_NotValidDocumentFormat;
            if( bEnd )
                break;
            ++buf;
        }

        return MINIDOM_SUCCESS;
#else /* MINIDOM_SUPPORT_HTTP */
        return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
    }
}

