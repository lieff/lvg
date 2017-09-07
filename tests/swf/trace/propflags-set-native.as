// makeswf -v 7 -r 1 -o propflags-set-proto-7.swf propflags-set-proto.as

trace ("Test property with get/set functions being hidden with version propflags");

var a = {};

a.addProperty ("x", 
    function () { trace ("x_get"); return "x_get_value"; },
    function () { trace ("x_set: " + arguments); } );

trace (a.x);

ASSetPropFlags (a, "x", 1 << 12);

trace (a.x);

a.x = 1;

trace (a.x);

ASSetPropFlags (a, "x", 0, 1 << 12);

trace (a.x);

loadMovie ("FSCommand:quit", "");
