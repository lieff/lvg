// makeswf -v 7 -s 200x150 -r 1 -o call-arguments.swf call-arguents.as

trace ("Check arguments handling in Function.call");

#include "values.as"

function foo (a) {
  trace (this);
  trace (a);
  this.x = 42;
  trace (this.x);
};

foo.call ();
for (i = 0; i < values.length; i++) {
  foo.call (values[i], "hi");
};

loadMovie ("FSCommand:quit", "");
