// makeswf -v 7 -r 1 -o implements-7.swf implements.as

trace ("Test Implements ActionScript action");


trace ("A implements Array, XML");

var A = function () {
  return {
    constructor: A,
    __proto__: A.prototype
  };
};

trace ("Before");
var o = new A ();
trace (o instanceOf A);
trace (o instanceOf Array);
trace (o instanceOf XML);
trace (o instanceOf String);

asm {
  push "test"
  push "Array"
  getvariable
  push "XML"
  getvariable
  push 2
  push "A"
  getvariable
  implements
  trace
};

trace ("After");
var o = new A ();
trace (o instanceOf A);
trace (o instanceOf Array);
trace (o instanceOf XML);
trace (o instanceOf String);


trace ("B changes prototype to A.prototype");
var B = function () {
  return {
    constructor: B,
    __proto__: B.prototype
  };
};

trace ("Before");
o = new B ();
trace (o instanceOf A);
trace (o instanceOf Array);
trace (o instanceOf XML);
trace (o instanceOf String);

B.prototype = A.prototype;

trace ("After");
o = new B ();
trace (o instanceOf A);
trace (o instanceOf Array);
trace (o instanceOf XML);
trace (o instanceOf String);


trace ("C has A as parent");
var C = function () {
  return {
    constructor: C,
    __proto__: C.prototype
  };
};
C.prototype.__proto__ = A.prototype;

o = new C ();
trace (o instanceOf A);
trace (o instanceOf Array);
trace (o instanceOf XML);
trace (o instanceOf String);


trace ("A.prototype is reset");

A.prototype = new Object ();

var o = new A ();
trace (o instanceOf A);
trace (o instanceOf Array);
trace (o instanceOf XML);
trace (o instanceOf String);


loadMovie ("FSCommand:quit", "");
