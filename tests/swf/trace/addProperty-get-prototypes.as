// makeswf -v 7 -s 200x150 -r 1 -o addProperty-get-prototypes.swf addProperty-get-prototypes.as

trace ("Check getting of variables when addProperty has been used on prototypes.");

o = {name: "o"};
o.__proto__ = {name: "proto"};
o.x = 10;
trace (o.x);
o.__proto__.addProperty ("x", function () { trace (this.name); return 42; }, function () { trace (this.name + " set"); });
trace (o.x);
o.x = 15;
trace (o.x);
delete o.x;
trace (o.x);
o.x = 20;
trace (o.x);

loadMovie ("FSCommand:quit", "");
