// makeswf -v 7 -r 1 -o xml-id-map-7.swf xml-id-map.as

// Basic stuff
#include "trace_properties.as"

var x = new XML ("<a id='1'><b id='2'/><c/></a>");
trace_properties (x, "local", "x");


// See the state when set is called
trace ("");

function test_get () {
  trace ("test_get");
}

function test_set (n) {
  trace ("test_set: " + arguments);
  trace ("caller: " + arguments.caller + ", callee: " + arguments.callee);
  trace (x);
}

x = new XML ();
x.addProperty ("test", test_get, test_set);
x.idMap = {};
x.idMap.addProperty = Object.prototype.addProperty;
x.idMap.addProperty ("test", test_get, test_set);
delete x.idMap.addProperty;
x.parseXML ("<a><b id='test'/></a>");
trace ("done");
trace (x);


// Change ignoreWhite while parsing
trace ("");

function change_get () {
};

function change_set () {
  x.ignoreWhite = !x.ignoreWhite;
  trace ("changed ignoreWhite to: " + x.ignoreWhite);
  trace (x);
}

x = new XML ();
x.addProperty ("change", change_get, change_set);
x.idMap = {};
x.idMap.addProperty = Object.prototype.addProperty;
x.idMap.addProperty ("change", change_get, change_set);
delete x.idMap.addProperty;
x.parseXML (" <a> <b id='change'/> <c/> <d id='change'/> </a>");
trace ("done");
trace (x);


// idMap preset
trace ("");

x = new XML ();
x.idMap = 2;
x.parseXML ("<a id='1'/>");
trace (x.idMap);
trace (x.idMap[1]);

x = new XML ();
x.idMap = new Object ();
x.parseXML ("<a id='1'/>");
trace (x.idMap);
trace (x.idMap[1]);

x = new XML ();
x.idMap = {};
x.idMap[1] = "hello";
x.parseXML ("<a id='1'/>");
trace (x.idMap);
trace (x.idMap[1]);

x = new XML ();
x.idMap = {};
x.idMap[1] = "hello";
ASSetPropFlags (x.idMap, "1", 7);
x.parseXML ("<a id='1'/>");
trace (x.idMap);
trace (x.idMap[1]);


loadMovie ("FSCommand:quit", "");
