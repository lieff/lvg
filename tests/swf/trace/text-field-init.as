// makeswf -v 7 -r 1 -o textfield-init-7.swf textfield-init.as

function check_and_reset () {
  _global.TextField.prototype.hasOwnProperty = ASnative (101, 5);
  trace (_global.TextField.prototype.hasOwnProperty ("text"));
  _global.TextField.prototype = new Object ();
}

trace ("Check when TextField's native properties are initialized");

// FIXME: Not tested yet: The properties should only be created if the
// prototype Object has not been initialized by any object's constructor with
// native properties (TextField, TextFormat, XML, XMLNode at least)

check_and_reset ();

var tmp = new TextField ();
check_and_reset ();

this.createTextField ("t", 0, 0, 0, 100, 100);
check_and_reset ();

this.createTextField ();
check_and_reset ();

this.createTextField ("t");
check_and_reset ();

this.createTextField ("t", 0);
check_and_reset ();

this.createTextField ("t", 0, 0);
check_and_reset ();

this.createTextField ("t", 0, 0, 0);
check_and_reset ();

this.createTextField ("t", 0, 0, 0, 100);
check_and_reset ();

this.createTextField ("t", 0, 0, 0, 100, undefined);
check_and_reset ();

this.createTextField ("t", 0, 0, 0, -5, -5);
check_and_reset ();

loadMovie ("FSCommand:quit", "");
