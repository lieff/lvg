// makeswf -v 7 -s 200x150 -r 1 -o callmethod-this.swf callmethod-this.as

trace ("Check which objects work as this property for a CallMethod action");

#include "values.as"

function bar () {
  trace (this);
  trace (super);
  this.x = 42;
  trace (this.x);
};
for (i = 0; i < values.length; i++) {
  trace ("Calling with " + names[i]);
  x = values[i];
  asm {
    push 0, "bar"
    getvariable
    push "x"
    getvariable
    callmethod
    pop
  };
};

loadMovie ("fscommand:quit", "");
