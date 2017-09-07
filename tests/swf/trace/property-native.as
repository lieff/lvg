// makeswf -v 7 -s 200x150 -r 1 -o property-native.swf property-native.as

trace ("Check how addProperty overwrites native properties");
this.__proto__._x = 42;
trace (this._x);
this.addProperty ("_x", function () { return 42; }, function () { trace ("set"); });
trace (this._x);
this._x = 10;
trace (this._x);

loadMovie ("FSCommand:quit", "");
