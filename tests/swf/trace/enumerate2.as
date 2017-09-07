// makeswf -v 7 -s 200x150 -r 1 -o enumerate2.swf enumerate2.as

#include "values.as"

function enum (x) {
  asm {
    push "x"
    getvariable
    dup
    trace
    enumerate2
    push undefined
    equals
    trace
  };
};

for (i = 0; i < values.length; i++) {
  trace (">>> " + names[i]);
  enum (values[i]);
};

loadMovie ("FSCommand:quit", "");
