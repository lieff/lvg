// makeswf -v 7 -s 200x150 -r 1 -o construct-properties.swf construct-properties.as

trace ("Check initial properties of script-constructed objects");

function check (o, desc)
{
  trace (">>> " + desc);
  trace (o.constructor);
  trace (o.hasOwnProperty ("constructor"));
  trace (o.__constructor__);
  trace (o.hasOwnProperty ("__constructor__"));
  trace (o.__proto__);
  trace (o.hasOwnProperty ("__proto__"));
};

check (new Object (), "new Object ()");
check ({}, "{}");
check ([], "[]");
Foo = function () {
  check (this, "");
};
check (new Foo (), "new Foo ()");
check (new this.Foo (), "new this.Foo ()");
ASSetPropFlags (Foo, "prototype", 0, 7);
delete Foo.prototype;
trace (Foo.prototype);
x = new Foo ();
check (x, "new Foo () - without prototype");
Foo.__proto__ = null;
x = new Foo ();
x.hasOwnProperty = Object.prototype.hasOwnProperty;
check (x, "new this.Foo () - without prototype");
check (42, "42");
check (true, "true");

loadMovie ("FSCommand:quit", "");
