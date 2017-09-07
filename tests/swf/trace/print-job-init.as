// makeswf -v 7 -r 1 -o xml-node-init-7.swf xml-node-init.as

function check_and_reset () {
  _global.PrintJob.prototype.hasOwnProperty = ASnative (101, 5);
  trace (_global.PrintJob.prototype.hasOwnProperty ("orientation"));
  _global.PrintJob.prototype = new Object ();
}

trace ("Check when PrintJob's native properties are initialized");

// FIXME: Not tested yet: The properties should only be created if the
// prototype Object has not been initialized by any object's constructor with
// native properties (TextField, TextFormat, PrintJob, PrintJobNode at least)

check_and_reset ();
var tmp = new PrintJob ();
check_and_reset ();

loadMovie ("FSCommand:quit", "");
