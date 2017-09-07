// makeswf -v 7 -s 200x150 -r 1 -o addProperty-set-prototypes.swf addProperty-set-prototypes.as

trace ("Check how addProperty affects prototypes");

o = {name: "a"};
o.__proto__ = {name: "b", x: 10 };
o.__proto__.__proto__ = {name: "c"};
o.__proto__.__proto__.__proto__ = {name: "d"};
o.__proto__.__proto__.addProperty ("x", function () { trace (this.name); return 42; }, null);
o.__proto__.__proto__.__proto__.addProperty ("x", function () { trace (this.name); return 642; }, function () { trace (this.name + " set"); });
trace (o.x);
o.x = 15;
trace (o.x);
o.__proto__.__proto__ = o.__proto__.__proto__.__proto__;
o.x = 15;
trace (o.x);

loadMovie ("FSCommand:quit", "");
