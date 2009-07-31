/*

minidom - json parser

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

			if( s == 0 )
			{
				c = strnpad( c, "{", 1 );
				c = strnpad( c, MINIDOM_LINEFEED, sizeof(MINIDOM_LINEFEED)-1 );
			}

			if( i == NULL )
			{	// pop me
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

					if( NL(i->childList_)->size() == 0 )
						if( i->next() )
							if( NL(i->next()->childList_)->size() == 0 )
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

					if( NL(i->childList_)->size() > 0 )
					{
						stack.push_back( make_pair( i, NL(i->childList_)->front() ) );
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

		size_t length = 0;
		char quot = 0;
		bool bError = false;
		bool bEnd = false;
		bool bArrayMode = false;
		bool bBeginArray = false;
		string strKV;
		string oldK;

		node* elemNode = this, *newNode = NULL;
		NV(nodeVec_)->push_back( this );
		
		while( *buf )
		{
			c = *buf;
			switch( status ) 
			{
			case COMMA:
				if( c == ',' )
				{
					if( !bArrayMode )
						status = KEY;
					else
						status = VALUE_OR_BRACKET;
					break;
				}
				else if( c == '}' )
				{
					elemNode = elemNode->parent_;
					break;
				}
				else if( c == ']' )
				{
					bArrayMode = false;
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
						status = VALUE;
						bArrayMode = true;
						bBeginArray = true;
						break;
					}
					else if( ( c == '"' ) || ( c == '\'' ) )
					{
						quot = c;
						break;
					}
				}

				if( ( ( quot && c == quot ) 
					|| ( !quot && ( ( c == ' ' ) || ( c == ',' ) || ( c == ':' ) || ( c == '\r' ) || ( c == '\n') ) ) ) && ( strKV.size() > 0 ) )
				{
					quot = 0;
					if( ( status == KEY ) || ( bArrayMode && !bBeginArray ) )
					{
						if( status == KEY )
							oldK = strKV;
						newNode = createNode( convertString(oldK), elemNode );
						if( status == KEY )
						{
							if( c == ':' )
								status = VALUE_OR_BRACKET;
							else
								status = COLON;
						}
					}

					if( status == VALUE )
					{
						newNode->v_ = convertString(strKV);
						bBeginArray = false;
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
				if( c != ':' )
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

