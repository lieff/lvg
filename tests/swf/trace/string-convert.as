// makeswf -v 7 -s 200x150 -r 1 -o string-convert.swf string-convert.as

trace ("Check how automatic conversion to a string works");
asm {
  push "hello", "length"
  getmember
  trace
};
_global.String = function (foo) {
  trace (foo);
  this.length = -1;
};
String = function (foo) {
  trace (foo);
  this.length = 42;
};
asm {
  push "hello", "length"
  getmember
  trace
};

_global.String = 13;
asm {
  push "hello", "length"
  getmember
  trace
};

_global.String = new Object ();
asm {
  push "hello", "length"
  getmember
  trace
};

loadMovie ("FSCommand:quit", "");
