// makeswf -v 7 -s 200x150 -r 1 -o addProperty-flags.swf addProperty-flags.as

trace ("Check if flags of variables get reset upon addProperty");

o = {};
o.addProperty ("foo", function () { trace ("get"); return 42; }, function () { trace ("set called"); });
trace ("set:");
o.foo = 20;
trace ("mark constant:");
ASSetPropFlags (o, "foo", 4);
o.foo = 20;
trace ("overwrite:");
o.addProperty ("foo", function () { trace ("get"); return 42; }, function () { trace ("set called"); });
o.foo = 20;

o.bar = 10;
ASSetPropFlags (o, "bar", 4);
trace (o.bar);
o.addProperty ("bar", function () { trace ("get"); return 42; }, function () { trace ("set called"); });

trace (o.bar);

loadMovie ("FSCommand:quit", "");
