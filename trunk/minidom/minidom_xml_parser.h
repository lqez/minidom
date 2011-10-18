/*

    minidom - xml parser
    Copyright (c) 2009-2011 Park Hyun woo(ez.amiryo@gmail.com)

    See README for copyright and license information.

*/

namespace minidom
{
    int doc::writeXML( char* buf, size_t* size, void* conv )
    {
#if defined( MINIDOM_ENABLE_DUMP )
#if defined( MINIDOM_SUPPORT_XML )
        size_t s = 0;
        size_t len;
        char tmp[minidom_buffer_size];
#if defined( MINIDOM_SUPPORT_ICONV )
        size_t len_conv;
        char tmp_conv[minidom_buffer_size*2];
#endif  
        list<pair<node*,node*> > stack;
        if( childVec_.size() == 0 )
        {
            *size = 0;
            return MINIDOM_SUCCESS;
        }

        stack.push_back( make_pair(this, childVec_.front()) );

        while( stack.size() != 0 )
        {
            node* a = stack.back().first;
            node* i = stack.back().second;

            char* c = tmp;
            if( i == NULL )
            {   // pop me
                if( a != this )
                {
                    // indent
                    for( size_t t = 0; t < stack.size()-2; ++t )
                        c = strnpad( c, "\t", 1 );

                    c = strnpad( c, "</", 2 );
                    c = strnpad( c, a->k_.c_str(), a->k_.size() );
                    c = strnpad( c, ">", 1 );
                    c = strnpad( c, MINIDOM_LINEFEED, sizeof(MINIDOM_LINEFEED)-1 );
                }
                stack.pop_back();
            }
            else
            {
                // indent
                for( size_t t = 0; t < stack.size()-1; ++t )
                    c = strnpad( c, "\t", 1 );

                c = strnpad( c, "<", 1 );
                c = strnpad( c, i->k_.c_str(), i->k_.size() );
                if( i->attrVec_.size() > 0 )
                {
                    for( NVI iter = i->attrVec_.begin(); iter != i->attrVec_.end(); ++iter )
                    {
                        c = strnpad( c, " ", 1 );
                        c = strnpad( c, (*iter)->k_.c_str(), (*iter)->k_.size() );
                        c = strnpad( c, "=\"", 2 );
                        c = strnpad( c, (*iter)->v_.c_str(), (*iter)->v_.size() );
                        c = strnpad( c, "\"", 1 );
                    }
                    c = strnpad( c, ">", 1 );
                }
                else
                    c = strnpad( c, ">", 1 );

                c = strnpad( c, i->v_.c_str(), i->v_.size() );

                stack.back().second = i->next();
                if( i->childVec_.size() > 0 )
                {
                    stack.push_back( make_pair( i, i->childVec_.front() ) );
                    c = strnpad( c, MINIDOM_LINEFEED, sizeof(MINIDOM_LINEFEED)-1 );
                }
                else
                {
                    c = strnpad( c, "</", 2 );
                    c = strnpad( c, i->k_.c_str(), i->k_.size() );
                    c = strnpad( c, ">", 1 );
                    c = strnpad( c, MINIDOM_LINEFEED, sizeof(MINIDOM_LINEFEED)-1 );
                }
            }

            len = (size_t)(c - tmp);
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
        if( *size )
            *buf = '\0';
        *size = s;
        return MINIDOM_SUCCESS;
#else /* MINIDOM_SUPPORT_XML */
        return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
#else /* MINIDOM_ENABLE_DUMP */
        return MINIDOM_ERROR_DumpWasNotEnabled;
#endif
    }

    int doc::parseXML( const char* buf )
    {
#if defined( MINIDOM_SUPPORT_XML )
        enum {
            READY,
            PI_MODE,
            ELEMENT_NAME,
            ATTRIBUTE_NAME,
            ATTRIBUTE_VALUE,
            TEXT,
            END_ELEMENT_NAME,
        };

        int status = READY;
        char c;

        string strKey;
        string strValue;

        bool quot = false;
        bool newline = false;
        bool piMode = false;

        node* elemNode = this;
        node* attrNode = NULL;
        nodeVec_.push_back( this );
        
        while( *buf )
        {
            c = *buf;
            switch( status )
            {
            case READY:
                if( c == '<' )
                {
                    status = ELEMENT_NAME;
                    strKey = "";
                }
                break;
            case ELEMENT_NAME:
                if( ( c == ' ' ) || ( c == '\t' ) || ( c  == '>' ) )
                {
                    elemNode = elemNode->add( convertString(strKey) );
                    status = ( c == '>' )?TEXT:ATTRIBUTE_NAME;
                    strKey = "";
                    strValue = "";
                    if( c == '>' && piMode )
                    {
                        elemNode = elemNode->parent_;
                        piMode = false;
                    }
                }
                else if( c == '/' )
                {
                    status = END_ELEMENT_NAME;
                }
                else
                {
                    if( ( c == '!' ) || ( c == '?' ) )
                        piMode = true;
                    strKey += c;
                }
                break;
            case ATTRIBUTE_NAME:
                if( ( c == '=' ) || ( ( ( c == ' ' ) || ( c == '>' ) ) && ( strKey.size() != 0 ) ) )
                {
                    attrNode = elemNode->add( convertString(strKey), "", true );
                    if( c == ' ' )
                        status = ATTRIBUTE_NAME;
                    else if( c == '>' )
                    {
                        elemNode = elemNode->parent_;
                        status = TEXT;
                    }
                    else
                        status = ATTRIBUTE_VALUE;
                    strKey = "";
                }
                else if( ( ( c == ' ' ) || ( c == '?' ) || ( c == '\t' ) ) && ( strKey.size() == 0 ) )
                {
                    break;
                }
                else if( ( c == '/' ) && ( strKey.size() == 0 ) )
                {
                    elemNode = elemNode->parent_;
                    status = READY;
                }
                else if( ( c == '>' ) && ( strKey.size() == 0 ) )
                {
                    if( piMode )
                    {
                        elemNode = elemNode->parent_;
                        piMode = false;
                    }
                    status = TEXT;
                }
                else
                {
                    strKey += c;
                }
                break;
            case ATTRIBUTE_VALUE:
                if( ( c == '"' ) || ( c == '\'' ) || ( c == ' ' && !quot ) )
                {
                    quot = !quot;
                    if( !quot )
                    {
                        if( attrNode )
                        {
#ifdef MINIDOM_ENABLE_ICONV
                            if( piMode )
                                if( attrNode->k_ == "encoding" )
                                    if( attrNode->parent_->k_ == "?xml" )
                                    {
                                        srcEncoding_ = strValue;
                                        int res = initIconv();
                                        if( res != MINIDOM_SUCCESS )
                                            return res;
                                    }
                            attrNode->v_ = convertString(strValue);
#else
                            attrNode->v_ = strValue;
#endif /* MINIDOM_ENABLE_ICONV */
                            attrNode = NULL;
                        }
                        else
                            return MINIDOM_ERROR_NotValidXMLFormat;

                        strValue = "";
                        status = ATTRIBUTE_NAME;
                    }
                }
                else
                {
                    strValue += c;
                }
                break;
            case TEXT:
                if( c == '<' )
                {
                    if( elemNode )
                    {
                        if( strValue.size() >= sizeof(MINIDOM_LINEFEED) )
                        {
                            const char *last = strValue.c_str();
                            last += strValue.size()-sizeof(MINIDOM_LINEFEED)+1;
                            if( strcmp( last, MINIDOM_LINEFEED ) == 0 )
                                strValue.resize( strValue.size() - (sizeof(MINIDOM_LINEFEED)-1) );
                        }

                        status = ELEMENT_NAME;
                        elemNode->v_ = convertString(strValue);
                    }
                    else
                        return MINIDOM_ERROR_NotValidXMLFormat;

                    strValue = "";
                }
                else
                {
                    if( strValue.size() == 0 || newline )
                    {
                        if( ( c == '\t' ) || ( c == '\r' ) || ( c == '\n' ) || ( c == ' ' ) )
                        {
                            break;
                        }
                    }

                    if( ( c == '\r' ) || ( c == '\n' ) )
                    {
                        if( !newline )
                        {
                            newline = true;
                            strValue += MINIDOM_LINEFEED;
                        }
                    }
                    else
                    {
                        strValue += c;
                        newline = false;
                    }
                }
                break;
            case END_ELEMENT_NAME:
                if( c == '>' )
                {
                    elemNode = elemNode->parent_;
                    status = READY;
                    strKey = "";
                }
                else
                {
                    strKey += c;
                }
                break;
            }

            if( !elemNode ) // FINISH!
                break;
            ++buf;
        }

        return MINIDOM_SUCCESS;
#else /* MINIDOM_SUPPORT_XML */
        return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
    }
}

