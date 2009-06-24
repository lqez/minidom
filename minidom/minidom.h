/*

minidom - a minimized dom/path library

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

#ifndef _MINIDOM_H_
#define _MINIDOM_H_

/*
 *
 * If you don't use cmake, 
 *
 * define MINIDOM_SUPPORT_XML to parse XML
 * define MINIDOM_SUPPORT_INI to parse INI
 * define MINIDOM_SUPPORT_JSON to parse JSON
 * define MINIDOM_SUPPORT_NKV to parse NKV
 * define MINIDOM_SUPPORT_HTTP to parse HTTP Header
 *
 * define MINIDOM_ENABLE_DUMP to dump(save) dom into text or file
 * define MINIDOM_ENABLE_ICONV to use iconv library
 * define MINIDOM_ENABLE_MAP to use STL's map architecture
 * define MINIDOM_DEBUG to show stl types on debugging
 *
 *
 */

#if defined(_WIN32) || defined(_WIN64)
    #define MINIDOM_PLATFORM_WINDOWS
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define MINIDOM_PLATFORM_LINUX
#elif defined(__FreeBSD__)
    #define MINIDOM_PLATFORM_BSD
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define MINIDOM_PLATFORM_MACOS
#else
    // TODO: So, what is your platform?
#endif

#if defined( MINIDOM_PLATFORM_WINDOWS )
	#if defined( MINIDOM_DLL )
		#if defined( MINIDOM_EXPORT )
			#define MINIDOM_API __declspec(dllexport)
		#else
			#define MINIDOM_API __declspec(dllimport)
		#endif
	#else
		#define MINIDOM_API
	#endif /* MINIDOM_DLL */
#else /* other platforms */
	#define MINIDOM_API
#endif /* MINIDOM_PLATFORM_WINDOWS */

#include <iostream>
#include <string>
#ifdef MINIDOM_DEBUG
#include <list>
#include <vector>
#endif

#ifdef MINIDOM_ENABLE_ICONV
	#if defined(MINIDOM_PLATFORM_WINDOWS)
		#include "../requisite/libiconv/include/iconv.h"
	#else
		#include <iconv.h>
	#endif
#endif

namespace minidom
{
	class MINIDOM_API node;
	class MINIDOM_API selector;
	class MINIDOM_API doc;
	class MINIDOM_API algorithm;

	// hiding real-type (to export STL classes for dynamic libraries)
#ifdef MINIDOM_DEBUG
	typedef std::list<node*>* __NodeList;
	typedef std::vector<node*>* __NodeVector;
#else
	typedef void* __NodeList;
	typedef void* __NodeVector;
#endif

	class MINIDOM_API algorithm
	{
		friend class node;
		friend class selector;
	private:
		static bool compare(
				const std::string& dst,
				const std::string& src,
				size_t jump = 0 );
		static bool reverse_compare(
				const std::string& dst,
				const std::string& src,
				size_t jump = 0 );
	};

	class MINIDOM_API node
	{
		friend class doc;
		friend class selector;
	public:
		node* parent() { return parent_; }
		node* next() { return next_; }
		node* prev() { return prev_; }
		node* firstChild();
		node* lastChild();
		node* firstAttr();
		node* lastAttr();

		node* get( const std::string& path, size_t no = 0 );
		size_t count( const std::string& path );
		/* 'print' function is not safe on dynamic library.
			use it with your own risk. */
		void print( std::ostream& stream = std::cout, 
				bool useIndent = true, size_t indent = 0 );

		node* add( const char* k, const char* v, bool bAttribute = false );
		node* add( const char* k, int v, bool bAttribute = false );
		node* add( const char* k, double v, bool bAttribute = false );

		node* add( const std::string& k, const std::string& v, bool bAttribute = false );
		node* add( const std::string& k, int v, bool bAttribute = false );
		node* add( const std::string& k, double v, bool bAttribute = false );

		std::string& toString();
		const char* toChars();
		int toInt();
		double toDouble();

	protected:
		node();
		virtual ~node();
		void addChild( node* child );
		void addAttr( node* attr );
		virtual node* getNode( const std::string& path, size_t no = 0, bool getCount = false );

		std::string path_;
		std::string k_;
		std::string v_;

		doc* doc_;
		node* parent_;
		node* prev_;
		node* next_;
		__NodeList attrList_;
		__NodeList childList_;
	};

	class MINIDOM_API selector
	{
		friend class doc;
	public:
		selector();
		virtual ~selector();

	public:
		selector& query( const std::string& query );
		int query( const std::string& query, selector* s );
		node* at( size_t i );
		void printResult( std::ostream& stream = std::cout );
		size_t size();

	protected:
		__NodeVector nodeVec_;
	};

	class MINIDOM_API doc : public node, public selector
	{
		friend class node;
	public:
		enum DOCTYPE {
			XML,
			INI,
			JSON,
			NKV,
			HTTP,
		};

	public:
		doc();
		virtual ~doc();

	public:
//		void copy( doc &from );
//		bool equal( doc &with );
		void clear();
		size_t size();
		int loadString( DOCTYPE type, 
				const char* text, 
				const std::string targetEncoding = "" );
		int loadFile( DOCTYPE type, 
				const std::string& filename, 
				const std::string targetEncoding = "" );	
		int dumpString( DOCTYPE type, 
				char* target,
				size_t* size,
				const std::string targetEncoding = "" );
		int dumpFile( DOCTYPE type, 
				const std::string& filename, 
				const std::string targetEncoding = "" );

	protected:
		int parseXML( const char* text );
		int parseINI( const char* text );
		int parseJSON( const char* text );
		int parseNKV( const char* text );
		int parseHTTP( const char* text );

		int writeXML( char* buf, size_t* size, void* conv );
		int writeINI( char* buf, size_t* size, void* conv );
		int writeJSON( char* buf, size_t* size, void* conv );
		int writeNKV( char* buf, size_t* size, void* conv );
		int writeHTTP( char* buf, size_t* size, void* conv );

		node* createNode( std::string& key, node* parent, bool attribute = false );
		int initIconv();
		void* nodes_;
		void* iconv_;
		inline std::string& convertString( std::string& str );
		virtual node* getNode( std::string query, size_t no = 0, bool getCount = false );
		std::string srcEncoding_;
		std::string dstEncoding_;
	};
}

#endif /* _MINIDOM_H_ */
