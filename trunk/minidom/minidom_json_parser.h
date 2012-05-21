/*

    minidom - json parser
    See README for copyright and license information.

*/
#include <sstream>

namespace minidom
{
    int doc::writeJSON( char* buf, size_t* size, void* conv )
    {
#if defined( MINIDOM_ENABLE_DUMP )
#if defined( MINIDOM_SUPPORT_JSON )
        size_t s = 0;
        size_t len;
        int nBracketing = 0;
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

            if( s == 0 )
            {
                c = strnpad( c, "{", 1 );
                c = strnpad( c, MINIDOM_LINEFEED, sizeof(MINIDOM_LINEFEED)-1 );
            }

            if( i == NULL )
            {   // pop me
                // indent
                for( size_t t = 0; t < stack.size()-1; ++t )
                    c = strnpad( c, "\t", 1 );

                c = strnpad( c, "}", 1 );
                if( a != this )
                    c = strnpad( c, ",", 1 );
                stack.pop_back();
            }
            else
            {
                // indent
                for( size_t t = 0; t < stack.size() + (nBracketing>0)?1:0; ++t )
                    c = strnpad( c, "\t", 1 );

                if( nBracketing == 0 )
                {
                    c = strnpad( c, "\"", 1 );
                    c = strnpad( c, i->k_.c_str(), i->k_.size() );
                    c = strnpad( c, "\"", 1 );
                    c = strnpad( c, ":", 1 );

                    if( i->childVec_.size() == 0 )
                        if( i->next() )
                            if( i->next()->childVec_.size() == 0 )
                                if( i->k_ == i->next()->k_ )
                                {
                                    c = strnpad( c, "[", 1 );
                                    nBracketing = 1;
                                }
                }
                else
                {
                    nBracketing = 2;
                    if( i->next() )
                    {
                        if( i->k_ != i->next()->k_ )
                            nBracketing = 3;
                    }
                    else
                        nBracketing = 3;

                }

                if( nBracketing != 1 )
                {
                    if( i->v_.size() )
                    {
                        c = strnpad( c, " \"", 2 );
                        c = strnpad( c, i->v_.c_str(), i->v_.size() );
                        c = strnpad( c, "\"", 1 );
                    }

                    stack.back().second = i->next();

                    if( i->childVec_.size() > 0 )
                    {
                        stack.push_back( make_pair( i, i->childVec_.front() ) );
                        c = strnpad( c, " {", 2 );
                    }
                    else
                    {
                        if( i->next() )
                            c = strnpad( c, ",", 1 );
                    }
                }

                if( nBracketing == 3 )
                {
                    nBracketing = 0;
                    c = strnpad( c, MINIDOM_LINEFEED, sizeof(MINIDOM_LINEFEED)-1 );
                    for( size_t t = 0; t < stack.size(); ++t )
                        c = strnpad( c, "\t", 1 );
                    c = strnpad( c, "]", 1 );
                }
            }
            c = strnpad( c, MINIDOM_LINEFEED, sizeof(MINIDOM_LINEFEED)-1 );

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
#else /* MINIDOM_SUPPORT_JSON */
        return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
#else /* MINIDOM_ENABLE_DUMP */
        return MINIDOM_ERROR_DumpWasNotEnabled;
#endif
    }

    int doc::parseJSON( const char* buf )
    {
#if defined( MINIDOM_SUPPORT_JSON )
        enum {
            VALUE_OR_BRACKET,
            KEY,
            COLON,
            VALUE,
            COMMA,
        };

        int status = VALUE_OR_BRACKET;
        char c;

        char quot = 0;
        bool bError = false;
        bool bEnd = false;
        string strKV;
        string oldK;

        node* elemNode = this, *newNode = NULL;
        nodeVec_.push_back( this );
        
        while( *buf )
        {
            c = *buf;
            switch( status ) 
            {
            case COMMA:
                if( c == ',' )
                {
                    if( !newNode->array_ ){
                        status = KEY;
                    }
                    else
                    {
                        newNode = elemNode->add( newNode->k_ );
                        status = VALUE_OR_BRACKET;
                    }
                    break;
                }
                else if( c == '}' )
                {
                    if(newNode != NULL) newNode = newNode->parent_;
                    elemNode = elemNode->parent_;
                    break;
                }
                else if( c == ']' )
                {
                    break;
                }
                break;
            case VALUE_OR_BRACKET:
                if( strKV.size() == 0 )
                {
                    if( ( c == ' ' ) || ( c == '\t' ) || ( c == '\r' ) || ( c == '\n') )
                        break;
                }

                if( c == '{' )
                {
                    if( newNode != NULL )
                        elemNode = newNode;
                    status = KEY;
                    break;
                }
                else
                    status = VALUE;
                // no break here
            case KEY:
            case VALUE:
                if( strKV.size() == 0 )
                {
                    if( ( c == ' ' ) || ( c == '\t' ) || ( c == '\r' ) || ( c == '\n') )
                        break;
                    else if( c == '[' )
                    {
                        newNode->array_ = true;
                        status = VALUE_OR_BRACKET;
                        break;
                    }
                    else if( ( c == '"' ) || ( c == '\'' ) )
                    {
                        quot = c;
                        break;
                    }
                }

                if( ( ( quot && c == quot ) 
                    || ( !quot && ( ( c == ' ' ) || ( c == ',' ) || ( c == ':' ) || ( c == '\r' ) || ( c == '\t' ) || ( c == '\n') ) ) ) && ( strKV.size() > 0 ) )
                {
                    quot = 0;
                    if( status == KEY )
                    {
                        oldK = strKV;
                        newNode = elemNode->add( convertString(oldK) );
                        if( c == ':' )
                            status = VALUE_OR_BRACKET;
                        else
                            status = COLON;
                    }

                    if( status == VALUE )
                    {
                        newNode->v_ = convertString(strKV);
                        status = COMMA;
                    }
                    strKV.clear();
                    if( c == ',' )
                        continue;
                }
                else
                {
                    strKV += c;
                }
                break;
            case COLON:
                if( ( c == ' ' ) || ( c == '\t' ) || ( c == '\r' ) || ( c == '\n') )
                    break;
                else if( c != ':' )
                {
                    bError = true;
                    break;
                }
                status = VALUE_OR_BRACKET;
                break;
            }
            if( bError )
                return MINIDOM_ERROR_NotValidDocumentFormat;
            if( bEnd )
                break;
            ++buf;
        }

        return MINIDOM_SUCCESS;
#else /* MINIDOM_SUPPORT_JSON */
        return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
    }
}

