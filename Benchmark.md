# Introduction #
This document contains the result of the benchmark for 3 xml parsers - libxml2, tinyxml and minidom.

# Benchmark method #
Read and parse a big xml file(4.4MB) 5 times per each parser and calculate the average time in milliseconds.<br>
(xml file was created by 'junkxml.cpp' in minidom unit test source; <a href='http://minidom.googlecode.com/svn/trunk/tests/junkxml.cpp'>http://minidom.googlecode.com/svn/trunk/tests/junkxml.cpp</a> )<br>
<br>
<h1>Environment</h1>
Apple Mac Mini / Intel Core Duo 1.66Ghz / Mac OS X Leopard 10.5.8<br>
i686-apple-darwin9-gcc-4.0.1 (GCC) 4.0.1 (Apple Inc. build 5493)<br>
<br>
<ul><li>libxml2 - 2.6.30, Mac port from <a href='http://www.explain.com.au/oss/libxml2xslt.html'>http://www.explain.com.au/oss/libxml2xslt.html</a>
</li><li>tinyxml - 2.5.3, from <a href='http://sourceforge.net/projects/tinyxml'>http://sourceforge.net/projects/tinyxml</a>
</li><li>minidom - <a href='https://code.google.com/p/minidom/source/detail?r=22'>revision 22</a></li></ul>

<h1>Result</h1>
<ul><li>libxml2 : 483.4 ms<br>
</li><li>tinyxml : 912.0 ms<br>
</li><li>minidom : 858.0 ms