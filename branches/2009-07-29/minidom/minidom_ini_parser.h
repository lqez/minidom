/*

minidom - ini parser

Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)
http://studio.amiryo.com/minidom

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

namespace minidom
{
	int doc::writeINI( char* buf, size_t* size, void* conv )
	{
#if defined( MINIDOM_ENABLE_DUMP )
#if defined( MINIDOM_SUPPORT_INI )
		size_t s = 0;
		size_t len;
		char tmp[4096];
#if defined( MINIDOM_SUPPORT_ICONV )
		size_t len_conv;
		char tmp_conv[8192];
#endif
		list<pair<node*,node*> > stack;
		if( NL(this->childList_)->size() == 0 )
		{
			*size = 0;
			return MINIDOM_SUCCESS;
		}

		stack.push_back( make_pair(this, NL(this->childList_)->front()) );

		while( stack.size() != 0 )
		{
			node* a = stack.back().first;
			node* i = stack.back().second;

			char* c = tmp;
			if( i == NULL )
			{	// pop me
				if( a != this )
					sprintf( tmp, "%s", MINIDOM_LINEFEED );
				stack.pop_back();
			}
			else
			{
				if( stack.size() == 2 )
					sprintf( tmp, "%s = %s%s", i->k_.c_str(), i->v_.c_str(), MINIDOM_LINEFEED );
				else if( stack.size() == 1 )
					sprintf( tmp, "[%s]%s", i->k_.c_str(), MINIDOM_LINEFEED );

				stack.back().second = i->next();
				if( NL(i->childList_)->size() > 0 )
					stack.push_back( make_pair( i, NL(i->childList_)->front() ) );
			}

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
		if( *size )
			*buf = '\0';
		*size = s;
		return MINIDOM_SUCCESS;
#else /* MINIDOM_SUPPORT_INI */
		return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
#else /* MINIDOM_ENABLE_DUMP */
		return MINIDOM_ERROR_DumpWasNotEnabled;
#endif
	}

	int doc::parseINI( const char* buf )
	{
#if defined( MINIDOM_SUPPORT_INI )
		enum {
			KEY,
			EQUAL,
			VALUE,
		};

		int status = KEY;
		char c;

		size_t length = 0;
		bool quot = false;
		char countQuot = '"';
		string strKV;
		string strLength;

		node* elemNode = this;
		NV(nodeVec_)->push_back( this );
		
		while( *buf )
		{
			c = *buf;
			switch( status )
			{
			case KEY:
			case VALUE:
				if( ( !quot && ( ( (status == KEY) && (c == ' ' || c == '\t') ) || c == '\r' || c == '\n' ) )
					|| ( quot && ( c == countQuot ) ) )
				{
					if( strKV.size() > 0 )
					{
						if( status == KEY )
						{
							elemNode = createNode( convertString(strKV), elemNode );
							strKV.clear();

							if( c == ']' )
								status = KEY;
							else
								status = EQUAL;

							quot = false;
						}
						else
						{
							elemNode->v_ = convertString(strKV);
							elemNode = elemNode->parent_;
							strKV.clear();
							status = KEY;
						}
					}
				}
				else if( ( c == '"' ) || ( c == '[' ) )
				{
					switch( c )
					{
					case '"':	countQuot = '"';	break;
					case '[':	countQuot = ']';	break;
					case '\'':	countQuot = '\'';	break;
					}
					quot = !quot;
					if( this != elemNode )
					{
						// i got another category
						elemNode = elemNode->parent_;
					}
				}
				else if( quot || ( ( c != '\r' ) && ( c != '\n' ) && 
					( ( c != ' ' && c != '\t' ) || ( ( status == VALUE ) && ( strKV.size() > 0 ) ) ) ) )
				{
					strKV += c;
				}
				break;
			case EQUAL:
				if( c == '=' )
					status = VALUE;
				break;
			}
			++buf;
		}
		return MINIDOM_SUCCESS;
#else /* MINIDOM_SUPPORT_INI */
		return MINIDOM_ERROR_UnsupportedDocumentType;
#endif
	}
}
