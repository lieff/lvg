// makeswf -v 7 -s 200x150 -r 1 -o constructor-relay.swf constructor-relay.as

extends = function (child, parent) {
  asm {
    push "child"
    getvariable
    push "parent"
    getvariable
    extends
  };
};

Foo = function () {
  before = this;
  trace (this.getTime ());
  this.__proto__.__constructor__ = Date;
  trace (this.getTime ());
  this.x = 42;
  trace (this.x);
  for (i = 0; i < 5; i++) {
    super ();
    trace (this.x);
    trace (this.getTime ());
  }
  this.__proto__.__constructor__ = Array;
  trace (this.getTime ());
  super ();
  trace (this.getTime ());
  after = this;
};

//extends (Foo, Date);
Foo.prototype.getTime = Date.prototype.getTime;
x = new Foo ();
trace (before == after);
trace (before == x);
trace (before.getTime ());
trace (after.getTime ());

x = new Object ();
x.getTime = Date.prototype.getTime;
x.construct = Date;
x.construct ();
trace (x.getTime ());

getURL ("fscommand:quit", "");
