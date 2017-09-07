// makeswf -v 7 -r 1 -o xml-node-init-7.swf xml-node-init.as

function check_and_reset () {
  _global.XMLNode.prototype.hasOwnProperty = ASnative (101, 5);
  trace (_global.XMLNode.prototype.hasOwnProperty ("nodeType"));
  _global.XMLNode.prototype = new Object ();
}

trace ("Check when XMLNode's native properties are initialized");

// FIXME: Not tested yet: The properties should only be created if the
// prototype Object has not been initialized by any object's constructor with
// native properties (TextField, TextFormat, XML, XMLNode at least)

check_and_reset ();
var tmp = new XMLNode (1, "test");
check_and_reset ();

tmp = new XMLNode ();
check_and_reset ();
tmp = new XMLNode (1);
check_and_reset ();
tmp = new XMLNode (1, undefined);
check_and_reset ();
tmp = new XMLNode (undefined, "test");
check_and_reset ();

XMLNode (1, "test");
check_and_reset ();

var x = new XML ("<test/>test");
check_and_reset ();
x.createTextNode ("test");
check_and_reset ();
x.createElement ("test");
check_and_reset ();

loadMovie ("FSCommand:quit", "");
