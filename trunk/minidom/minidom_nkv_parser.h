/*

minidom - nkv parser

	nkv means 'Numbered Key=Value format'.
	Example:
		#:KEY=#:VALUE&...
		2:id=4:lqez&8:password=...

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
	int doc::writeNKV( char* buf, size_t* size, void* conv )
	{
#if defined( MINIDOM_ENABLE_DUMP )
#if defined( MINIDOM_SUPPORT_NKV )
		size_t s = 0;
		size_t len;
		char tmp[4096];
#if defined( MINIDOM_SUPPORT_ICONV )
		size_t len_conv;
		char tmp_conv[8192];
#endif
		list<node*> nodes;
		nodes.push_back( this );
		while( nodes.size() != 0 )
		{
			node* a = nodes.front();
			for( NLI iter = NL(a->childList_)->begin(); iter != NL(a->childList_)->end(); ++iter )
				nodes.push_back( *iter );
			/* Note: NKV does not support 'attribute'
			for( NLI iter = NL(a->attrList_)->begin(); iter != NL(a->attrList_)->end(); ++iter )
				nodes.push_back( *iter );
			*/
			if( a->k_.size() != 0 )
			{
				if( s == 0 )
					sprintf( tmp, "%d:%s=%d:%s",
						a->k_.size(), a->k_.c_str(), a->v_.size(), a->v_.c_str() );
				else
					sprintf( tmp, "&%d:%s=%d:%s",
						a->k_.size(), a->k_.c_str(), a->v_.size(), a->v_.c_str() );

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
		char tmp[4096];
		bool bError = false;
		bool bEnd = false;
		string strKV;
		string strLength;

		node* elemNode = this, *newNode = NULL;
		NV(nodeVec_)->push_back( this );
		
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

				elemNode = createNode( convertString(strKV), elemNode );
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

