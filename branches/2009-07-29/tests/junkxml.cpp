/*

	junkxml - create a junk xml file
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

*/
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

typedef vector<string> STRVEC;

const size_t max_node = 50000;
const size_t max_element_depth = 16;
const size_t max_element_name = 16;
const size_t max_attribute = 3+1; /*(0 to 3)*/
const size_t max_attribute_name = 8;
const size_t max_attribute_value = 12;
const size_t max_value = 100;
const string tab = "  ";
const bool   indent_and_linefeed = true;

void randomizedString( string& str, size_t min, size_t max )
{
	min = rand()%(max-min)+min;

	str.clear();
	str.reserve(min);
	do
	{
		str += rand()%26+((rand()%2)?65:97);
	} while( --min );
}

inline void print_indent( size_t i )
{
	if( !indent_and_linefeed )
		return;

	do
	{
		cout << tab;
	} while( --i );
}

int main()
{
	srand( time(NULL) );

	size_t nodes = 0;
	size_t indent = 0;
	STRVEC stack;

	string name;
	string CRLF;

	if( indent_and_linefeed )
#ifdef WIN32
		CRLF = "\r\n";
#else
		CRLF = "\n";
#endif

	// top
	randomizedString( name, 1, max_element_name );
	stack.push_back( name );
	cout << "<" << name << ">" << CRLF;

	while( nodes < max_node )
	{
		print_indent( stack.size() );

		// element
		randomizedString( name, 1, max_element_name );
		cout << "<" << name;
		stack.push_back( name );

		// attribute
		size_t attrs = rand()%max_attribute;
		while( attrs )
		{
			randomizedString( name, 1, max_attribute_name );
			cout << " " << name << "=\"";
			randomizedString( name, 1, max_attribute_value );
			cout << name << "\"";
			--attrs;
		}
		cout << ">";

		// value and closing
		if( ( rand()%2 == 0 ) || ( stack.size() == max_element_depth ) )
		{
			randomizedString( name, 1, max_value );
			cout << name << "</" << *stack.rbegin() << ">" << CRLF;
			stack.pop_back();

			while( ( rand()%2 == 0 ) && ( stack.size() >= 2 ) )
			{
				print_indent( stack.size()-1 );
				cout << "</" << *stack.rbegin() << ">" << CRLF;
				stack.pop_back();
			}
		}
		else
			cout << CRLF;
		++nodes;
	}

	// close all
	while( ( indent = stack.size() ) )
	{
		print_indent( --indent );
		cout << "</" << *stack.rbegin() << ">" << CRLF;
		stack.pop_back();
	}

 	return 0;
}
