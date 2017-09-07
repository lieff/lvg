// makeswf -v 7 -s 200x150 -r 1 -o prototype-addProperty.swf prototype-addProperty.as

trace ("Check addProperty (\"__proto__\", ...) setting doesn't affect prototypes");
o = new Object ();
o.x = 42;
o.addProperty ("__proto__", function () { trace("get"); }, function () { trace("set"); });
p = new Object ();
p.__proto__ = o;
trace ("hi");
o.__proto__ = new Object ();
ASSetPropFlags (p, "__proto__", 0, 7);
delete p.__proto__;
trace (p.x);
p.__proto__ = new Object ();
ASSetPropFlags (p, "__proto__", 7);
p.__proto__ = o;
trace (p.x);

loadMovie ("FSCommand:quit", "");
