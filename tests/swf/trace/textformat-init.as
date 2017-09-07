// makeswf -v 7 -r 1 -o textformat-init-7.swf textformat-init.as

function check_and_reset () {
  _global.TextFormat.prototype.hasOwnProperty = ASnative (101, 5);
  trace (_global.TextFormat.prototype.hasOwnProperty ("size"));
  _global.TextFormat.prototype = new Object ();
}

trace ("Check when TextFormat's native properties are initialized");

// FIXME: Not tested yet: The properties should only be created if the
// prototype Object has not been initialized by any object's constructor with
// native properties (TextField, TextFormat, XML, XMLNode at least)

check_and_reset ();
var tmp = new TextFormat ();
check_and_reset ();

var tmp = new TextField.StyleSheet ();
tmp.parse (".a { text-size: 4; }");
check_and_reset ();

this.createTextField ("t", 0, 0, 0, 100, 100);
t.text = "hello";
check_and_reset ();
t.getNewTextFormat ();
check_and_reset ();
t.setNewTextFormat (new Object ());
check_and_reset ();
t.getTextFormat ();
check_and_reset ();
t.getTextFormat (20, 21);
check_and_reset ();
t.getTextFormat (1, 2);
check_and_reset ();
t.setTextFormat (new Object ());
check_and_reset ();
tmp = t.textColor;
check_and_reset ();
t.textColor = 200;
check_and_reset ();

loadMovie ("FSCommand:quit", "");
