/*

	minidom - a minimized dom/path library
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

	See README for copyright and license information.

*/

#include <fcntl.h>
#include <errno.h>
#include <iostream>

#include "minidom.h"
#include "minidom_error.h"

#include <list>
#include <vector>
#ifdef MINIDOM_ENABLE_MAP
#include <map>
#endif
#include <algorithm>


#if defined(MINIDOM_PLATFORM_WINDOWS)
#include <io.h>

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define new new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif

#else
#include <unistd.h>
#include <string.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef MINIDOM_TARGET_ENCODING
	#define MINIDOM_TARGET_ENCODING "iso-8859-1"
#endif

#ifdef MINIDOM_BUFFER_SIZE
	const size_t minidom_buffer_size = atoi(MINIDOM_BUFFER_SIZE);
#else
	const size_t minidom_buffer_size = 4096;
#endif

#if defined(MINIDOM_PLATFORM_WINDOWS)
#pragma warning ( disable : 4819 )
#pragma warning ( disable : 4996 )
#define MINIDOM_LINEFEED "\r\n"
#elif defined(MINIDOM_PLATFORM_LINUX)
#define MINIDOM_LINEFEED "\r"
#elif defined(MINIDOM_PLATFORM_BSD)
#define MINIDOM_LINEFEED "\n"
#elif defined(MINIDOM_PLATFORM_MACOS)
#define MINIDOM_LINEFEED "\n"
#endif

using namespace std;
using namespace minidom;

// real-type for STL
typedef std::list<node*> NodeList;
typedef std::list<node*>::iterator NLI;
typedef std::vector<node*> NodeVector;
typedef std::vector<node*>::iterator NVI;
#define NL(X) ((NodeList*)(X))
#define NV(X) ((NodeVector*)(X))

#ifdef MINIDOM_ENABLE_MAP
typedef std::multimap<std::string,node*> NodeMap;
typedef std::multimap<std::string,node*>::iterator NMI;
#define NM(X) ((NodeMap*)(X))
#endif

#if defined( MINIDOM_ENABLE_DUMP )
char* strnpad( char* c, const char* s, size_t n )
{
	strncpy( c, s, n );
	c += n;
	return c;
}
#endif

// include parsers
#include "minidom_xml_parser.h"
#include "minidom_ini_parser.h"
#include "minidom_json_parser.h"
#include "minidom_nkv_parser.h"
#include "minidom_http_parser.h"

bool algorithm::compare( const string& dst, const string& src, size_t jump )
{
	if( jump == 0 )
		return( dst == src );
	
	const char* c = &dst[jump];
	return( c == src );
}

bool algorithm::reverse_compare( const string& dst, const string& src, size_t jump )
{
	if( dst.size() > src.size() )
	{
		const char* c = dst.c_str();
		c += ( dst.size()-src.size() );
		return( c == src );
	}
	else if( dst.size() < src.size() )
	{
		return false;
	}
	return( dst == src );
}

node::node()
{
	k_ = v_ = path_ = "";
	next_ = prev_ = parent_ = NULL;

	attrList_ = new NodeList();
	childList_ = new NodeList();
}

node::~node()
{
	k_ = v_ = path_ = "";
	next_ = prev_ = parent_ = NULL;

	NL(attrList_)->clear();
	NL(childList_)->clear();
	delete( NL(attrList_) );
	delete( NL(childList_) );
}

void node::print( ostream& stream, bool useIndent, size_t indent )
{
	if( useIndent )
	{
		for( size_t t = 0; t < indent; ++t )
			stream << "    ";
		stream << k_ << " = " << v_;
	}
	else
	{
		stream << path_ << " = " << v_;
	}

	if( NL(attrList_)->size() > 0 )
	{
		stream << "[ ";
		for( NLI iter = NL(attrList_)->begin(); iter != NL(attrList_)->end(); ++iter )
			stream << (*iter)->k_ << "=\"" << (*iter)->v_ << "\" ";
		stream << "]";
	}
	stream << "\n";
	for( NLI iter = NL(childList_)->begin(); iter != NL(childList_)->end(); ++iter )
		(*iter)->print( stream, useIndent, indent+1 );
}

node* node::firstChild()
{
	if( NL(childList_)->size() == 0 )
		return NULL;
	return *NL(childList_)->begin();
}

node* node::lastChild()
{
	if( NL(childList_)->size() == 0 )
		return NULL;
	return *NL(childList_)->rbegin();
}

node* node::firstAttr()
{
	if( NL(attrList_)->size() == 0 )
		return NULL;
	return *NL(attrList_)->begin();
}

node* node::lastAttr()
{
	if( NL(attrList_)->size() == 0 )
		return NULL;
	return *NL(attrList_)->rbegin();
}

void node::addChild( node* child )
{
	child->parent_ = this;
	if( NL(childList_)->size() != 0 )
	{
		node* endOfChild = *NL(childList_)->rbegin();
		endOfChild->next_ = child;
		child->prev_ = endOfChild;
	}
	NL(childList_)->push_back( child );

	child->path_ = path_ + "/" + child->k_;
}

void node::addAttr( node* attr )
{
	attr->parent_ = this;
	if( NL(attrList_)->size() != 0 )
	{
		node* endOfAttr = *NL(attrList_)->rbegin();
		endOfAttr->next_ = attr;
		attr->prev_ = endOfAttr;
	}
	NL(attrList_)->push_back( attr );

	attr->path_ = path_ + "@" + attr->k_;
}

node* node::get( const std::string& path, size_t no )
{
	return getNode( path, no, false );
}

size_t node::count( const std::string& path )
{
	return (size_t)getNode( path, 0, true );
}

node* node::getNode( const std::string& path, size_t no, bool getCount )
{
	if( path.size() == 0 )
		return NULL;

	list<node*> nodes;
	nodes.push_back( this );

	bool(*cf)( const string&, const string&, size_t ) = 
		(path[0] == '/')?
		&algorithm::compare
		:
		&algorithm::reverse_compare;

	size_t c = 0;
	while( nodes.size() != 0 )
	{
		node* a = nodes.front();
		for( NLI iter = NL(a->childList_)->begin(); iter != NL(a->childList_)->end(); ++iter )
			nodes.push_back( *iter );
		for( NLI iter = NL(a->attrList_)->begin(); iter != NL(a->attrList_)->end(); ++iter )
			nodes.push_back( *iter );

		if( cf( a->path_, path, path_.size() ) )
		{
			if( ( c == no ) && ( !getCount ) )
				return a;
			++c;
		}
		nodes.pop_front();
	}

	if( getCount )
		return (node*)c;

	return NULL;
}

node* node::add( const char* k, const char* v, bool bAttribute )
{
	node* newNode = new node;
	newNode->doc_ = doc_;
	newNode->k_ = k;
	doc_->convertString(newNode->k_);
	newNode->v_ = v;
	doc_->convertString(newNode->v_);

	if( bAttribute )
		addAttr( newNode );
	else
		addChild( newNode );

#ifdef MINIDOM_ENABLE_MAP
	NM(doc_->nodes_)->insert( make_pair( newNode->path_, newNode ) );
#else
	NL(doc_->nodes_)->push_back( newNode );
#endif

	return newNode;
}

node* node::add( const char* k, int v, bool bAttribute )
{
	char buf[64];
	sprintf( buf, "%d", v );
	return add( k, buf, bAttribute );
}

node* node::add( const char* k, double v, bool bAttribute )
{
	char buf[64];
	sprintf( buf, "%f", v );
	return add( k, buf, bAttribute );
}

node* node::add( const std::string& k, const std::string& v, bool bAttribute )
{
	return add( k.c_str(), v.c_str(), bAttribute );
}

node* node::add( const std::string& k, int v, bool bAttribute )
{
	return add( k.c_str(), v, bAttribute );
}

node* node::add( const std::string& k, double v, bool bAttribute )
{
	return add( k.c_str(), v, bAttribute );
}

std::string& node::toString()
{ 
	static std::string null_string = "";
	if( this ) 
		return v_; 
	else 
		return null_string;
}

const char* node::toChars()
{
	static char null_char[] = "";
	if( this ) 
		return v_.c_str();
	else
		return null_char;
}

int node::toInt()
{
	if( this )
		return atoi( v_.c_str() );
	else
		return 0;
}

double node::toDouble()
{
	if( this )
		return atof( v_.c_str() );
	else
		return 0.0f;
}

selector::selector()
{
	nodeVec_ = new NodeVector();
}

selector::~selector()
{
	NV(nodeVec_)->clear();
	delete( NV(nodeVec_) );
}

node* selector::at( size_t i )
{
	if( ( i < 0 ) || ( i >= NV(nodeVec_)->size() ) )
		return NULL;
	return (*NV(nodeVec_))[i];
}

void selector::printResult( ostream& stream )
{
	for( NVI iter = NV(nodeVec_)->begin(); iter != NV(nodeVec_)->end(); ++iter )
	{
		stream << (*iter)->k_ << " = " << (*iter)->v_ << " (" << (*iter)->path_ << ") ";
		if( NL((*iter)->attrList_)->size() > 0 )
		{
			stream << "[ ";
			for( NLI iter2 = NL((*iter)->attrList_)->begin(); iter2 != NL((*iter)->attrList_)->end(); ++iter2 )
				stream << (*iter2)->k_ << "=\"" << (*iter2)->v_ << "\" ";
			stream << "]";
		}
		stream << "\n";
	}
}

size_t selector::size()
{
	return NV(nodeVec_)->size();
}

int selector::query( const string& query, selector* s )
{
	if( query.size() == 0 )
		return MINIDOM_ERROR;
	if( s == 0 )
		return MINIDOM_ERROR_NullPointerBuffer;

	NodeList queue;
	list<size_t> pqueue;
	
	for( NVI iter = NV(nodeVec_)->begin(); iter != NV(nodeVec_)->end(); ++iter )
	{
		queue.push_back( *iter );
		pqueue.push_back( (*iter)->path_.size() );
	}

	bool(*cf)( const string&, const string&, size_t ) = 
		(query[0] == '/')?
		&algorithm::compare
		:
		&algorithm::reverse_compare;

	while( queue.size() != 0 )
	{
		node* a = queue.front();
		size_t prefix = pqueue.front();

		for( NLI iter = NL(a->childList_)->begin(); iter != NL(a->childList_)->end(); ++iter )
		{
			queue.push_back( *iter );
			pqueue.push_back( prefix );
		}

		if( cf( &a->path_[prefix], query, 0 ) )
			NV(s->nodeVec_)->push_back( a );

		queue.pop_front();
		pqueue.pop_front();
	}

	return MINIDOM_SUCCESS;
}

selector& selector::query( const string& query )
{
	if( query.size() == 0 )
		return *this;

	selector* s = new selector();
	NodeList queue;
	list<size_t> pqueue;
	
	for( NVI iter = NV(nodeVec_)->begin(); iter != NV(nodeVec_)->end(); ++iter )
	{
		queue.push_back( *iter );
		pqueue.push_back( (*iter)->path_.size() );
	}

	bool(*cf)( const string&, const string&, size_t ) = 
		(query[0] == '/')?
		&algorithm::compare
		:
		&algorithm::reverse_compare;

	while( queue.size() != 0 )
	{
		node* a = queue.front();
		size_t prefix = pqueue.front();

		for( NLI iter = NL(a->childList_)->begin(); iter != NL(a->childList_)->end(); ++iter )
		{
			queue.push_back( *iter );
			pqueue.push_back( prefix );
		}

		if( cf( &a->path_[prefix], query, 0 ) )
			NV(s->nodeVec_)->push_back( a );

		queue.pop_front();
		pqueue.pop_front();
	}

	return *s;
}

doc::doc()
:nodes_(0), iconv_(0)
{
	#ifdef MINIDOM_ENABLE_MAP
		nodes_ = new NodeMap();
	#else
		nodes_ = new NodeList();
	#endif

	doc_ = this;
	srcEncoding_ = MINIDOM_TARGET_ENCODING;
	dstEncoding_ = MINIDOM_TARGET_ENCODING;
}

doc::~doc()
{
	clear();
	if( nodes_ )
	{
#ifdef MINIDOM_ENABLE_MAP
		delete( NM(nodes_) );
#else
		delete( NL(nodes_) );
#endif
	}

#ifdef MINIDOM_ENABLE_ICONV
	if( iconv_ )
		iconv_close( iconv_ );
#endif
}

#ifdef MINIDOM_ENABLE_ICONV
inline string& doc::convertString( string& str )
{
	if( srcEncoding_ == dstEncoding_ )
		return str;

	if( !iconv_ )
		return str;

	char buf[minidom_buffer_size];
#if defined( MINIDOM_PLATFORM_WINDOWS )
	const char *src = str.c_str();
#else
	char *src = const_cast<char *>(str.c_str());
#endif
	char *dst = &buf[0];
	size_t srcSize = str.size();
	size_t dstSize = minidom_buffer_size;
	iconv( iconv_, &src, &srcSize, &dst, &dstSize );
	*dst = '\0';
	str = buf;
	return str;
}
#else
inline string& doc::convertString( string& str )
{
	return str;
}
#endif

int doc::initIconv()
{
#ifdef MINIDOM_ENABLE_ICONV
	if( iconv_ ) iconv_close( iconv_ );

	iconv_ = iconv_open( dstEncoding_.c_str(), srcEncoding_.c_str() );
	if( iconv_ == (iconv_t)-1 )
	{
		if( errno == EINVAL )
			return MINIDOM_ERROR_UnsupportedEncoding;
		return MINIDOM_ERROR_CannotCreateIconvInstance;
	}
#endif
	return MINIDOM_SUCCESS;
}

int doc::dumpString( DOCTYPE type, 
		char* target,
		size_t* size,
		const std::string targetEncoding )
{
#ifdef MINIDOM_ENABLE_ICONV
	iconv_t conv = iconv_open( dstEncoding_.c_str(), targetEncoding.c_str() );
	if( conv == (iconv_t)-1 )
	{
		if( errno == EINVAL )
			return MINIDOM_ERROR_UnsupportedEncoding;
		return MINIDOM_ERROR_CannotCreateIconvInstance;
	}
#else
	void* conv = NULL;
#endif
	int res;
	switch( type )
	{
	case XML:	res = writeXML( target, size, conv ); break;
	case INI:	res = writeINI( target, size, conv ); break;
	case JSON:	res = writeJSON( target, size, conv ); break;
	case NKV:	res = writeNKV( target, size, conv ); break;
	case HTTP:	res = writeHTTP( target, size, conv ); break;
	default:	res = MINIDOM_ERROR_UnsupportedDocumentType; break;
	}
#ifdef MINIDOM_ENABLE_ICONV
	iconv_close( conv );
#endif
	return res;
}

int doc::dumpFile( DOCTYPE type, 
		const std::string& filename, 
		const std::string targetEncoding )
{
	char* buf = NULL;
	size_t size = 0;

	// get size of dumpped string
	int res = dumpString( type, buf, &size, targetEncoding );
	if( res == MINIDOM_SUCCESS )
	{
		++size;	// +1 for '\0'
		buf = (char*)malloc( size );
		// real dump
		res = dumpString( type, buf, &size, targetEncoding );
		if( res != MINIDOM_SUCCESS )
			return res;

		int h = open( filename.c_str(), O_CREAT | O_RDWR | O_BINARY );
		if( h < 0 )
			return MINIDOM_ERROR_FileOpenError;

		if( size != write( h, buf, size ) )
			return MINIDOM_ERROR_FileWriteError;

		close( h );
		free(buf);
	}
	return res;
}

int doc::loadString( DOCTYPE type, const char* text, const string targetEncoding )
{
	clear();
	if( targetEncoding.size() != 0 )
		dstEncoding_ = targetEncoding;
	else
		dstEncoding_ = MINIDOM_TARGET_ENCODING;

	switch( type )
	{
	case XML:	return parseXML( text );
	case INI:	return parseINI( text );
	case JSON:	return parseJSON( text );
	case NKV:	return parseNKV( text );
	case HTTP:	return parseHTTP( text );
	default:	return MINIDOM_ERROR_UnsupportedDocumentType;
	}
	return MINIDOM_ERROR;
}

int doc::loadFile( DOCTYPE type, const string& filename, const string targetEncoding )
{
	dstEncoding_ = targetEncoding;

	long	size;
	int		h;
	void*	buf;
	
	h = open( filename.c_str(), O_RDONLY | O_BINARY );
	if( h < 0 )
		return MINIDOM_ERROR_FileOpenError;

	size = lseek( h, 0, SEEK_END );
	if( size == 0 )
		return MINIDOM_ERROR_ZeroFileSize;

	lseek( h, 0, SEEK_SET );

	buf = malloc( size+1 );
	if( buf == NULL )
		return MINIDOM_ERROR_NotEnoughMemory;

	// put '\0' on the last
	((char*)buf)[size] = '\0';

	if( size != read( h, buf, size ) )
		return MINIDOM_ERROR_FileReadError;

	int res = loadString( type, (char*)buf, targetEncoding );

	free( buf );
	close( h );

	return res;
}

#ifdef MINIDOM_ENABLE_MAP
node* doc::getNode( std::string query, size_t no, bool getCount )
{
	if( getCount )
		return (node*)NM(nodes_)->count( query );

	pair<NMI,NMI> ret = NM(nodes_)->equal_range( query );
	size_t c = 0;
	NMI iter = ret.first;
	while( iter != ret.second )
	{
		if( no == c )
			return iter->second;
		++c;
		++iter;
	}
	return NULL;
}
#else
inline node* doc::getNode( std::string query, size_t no, bool getCount )
{
	return node::getNode( query, no, getCount );
}
#endif

void doc::clear()
{
	NV(nodeVec_)->clear();

#ifdef MINIDOM_ENABLE_MAP
	NMI iter = NM(nodes_)->begin();
	while( iter != NM(nodes_)->end() )
	{
		delete iter->second;
		++iter;
	}
	NM(nodes_)->clear();
#else
	NLI iter = NL(nodes_)->begin();
	while( iter != NL(nodes_)->end() )
	{
		delete *iter;
		++iter;
	}
	NL(nodes_)->clear();
#endif
}

size_t doc::size()
{
#ifdef MINIDOM_ENABLE_MAP
	return NM(nodes_)->size();
#else
	return NL(nodes_)->size();
#endif
}

node* doc::createNode( std::string& key, node* parent, bool attribute )
{
	node* newNode = new node;
	newNode->doc_ = this;
	newNode->k_ = key;
	if( attribute )
		parent->addAttr( newNode );
	else
		parent->addChild( newNode );
#ifdef MINIDOM_ENABLE_MAP
	NM(nodes_)->insert( make_pair( newNode->path_, newNode ) );
#else
	NL(nodes_)->push_back( newNode );
#endif
	return newNode;
}
