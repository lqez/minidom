'minidom' is a minimized quick & dirty dom/path c++ library.

minidom supports XML, INI, JSON, NKV, HTTP header parsers and dumpers.

  * Platform independent. Supports Windows, Mac, Linux.
  * Supports ICONV (optional).
  * Various compile settings with CMake. ( http://cmake.org/ )
  * And CMake-based unit testings are also available.

```
minidom::doc dom;
dom.loadFile( minidom::doc::XML, "inventory.xml" );
std::cout << dom.get( "author/first-name" )->toChars(); /* Joe */
...
```