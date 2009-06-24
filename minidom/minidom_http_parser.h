/*

minidom - http header parser

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
		NV(nodeVec_)->push_back( this );
		strK = "result";
		node* resultNode = createNode( convertString(strK), this );
		strK = "header";
		node* headerNode = createNode( convertString(strK), this );
		strK = "content";
		node* contentNode = createNode( convertString(strK), this );
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
					elemNode = createNode( convertString(strK), resultNode );
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
					elemNode = createNode( convertString(strK), resultNode );
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
					elemNode = createNode( convertString(strK), headerNode );
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
					_itoa( buf - offset, bufSize, 10 );
					strK = "offset";
					strV = bufSize;
					elemNode = createNode( convertString(strK), contentNode, true );
					elemNode->v_ = convertString(strV);
				//	int contentLength = get( "/header/Content-Length" )->toInt();
				//	if( contentLength > 0 )
				//		contentNode->v_.assign( buf, contentLength );
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

