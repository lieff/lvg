// makeswf -v 7 -r 1 -o xml-node-init-7.swf xml-node-init.as

function check_and_reset () {
  _global.XML.prototype.hasOwnProperty = ASnative (101, 5);
  trace (_global.XML.prototype.hasOwnProperty ("ignoreWhite"));
  _global.XML.prototype = new Object ();
}

trace ("Check when XML's native properties are initialized");

// FIXME: Not tested yet: The properties should only be created if the
// prototype Object has not been initialized by any object's constructor with
// native properties (TextField, TextFormat, XML, XMLNode at least)

check_and_reset ();
var tmp = new XML ();
check_and_reset ();
var tmp = _global.XML;
trace (tmp ());
check_and_reset ();

loadMovie ("FSCommand:quit", "");
