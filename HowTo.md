minidom is quick-and-dirty, not elegant dom/path c++ library.

---

## Parse a document ##
### From a file ###
```
minidom::doc dom;
dom.loadFile( minidom::doc::XML, "sample.xml" );
dom.print( std::cout );
```
### From a string ###
```
minidom::doc dom;
dom.loadString( minidom::doc::XML, "<google>code</google>" );
dom.print( std::cout );
```

## Get a node ##
```
dom.get( "author/first-name" )->toChars();
dom.get( "/files/count" )->toInt();
```

## XPath-like query ##
```
minidom::selector s = dom.query( "book" );
if( s.size() >= 2 )
{
	s.at(1)->get( "@style" )->toChars();
	s.printResult();
}
```

## Create a node ##
```
// Simple
dom.add( "string", "1234" );
dom.add( "an integer", 1234 ).add( "child integer", 5678 );

// Complex
minidom::selector s = dom.query( "book" );
size_t books = s.size();
int totalpages = 0;

while( books-- )
	totalpages += s.at(books)->get( "pagecount" )->toInt();

dom.add( "totalpages", totalpages );
```

## Dump ##
### Dump into a file ###
```
if( dom.dumpFile( minidom::doc::XML, "dumpped.xml" ) == MINIDOM_SUCCESS )
	std::cout << "ok" << endl;
```

### Dump into string ###
```
char buf[4096];
size_t size = 4096;
if( dom.dumpString( minidom::doc::XML, buf, &size ) == MINIDOM_SUCCESS )
	std::cout << "ok" << endl;
```