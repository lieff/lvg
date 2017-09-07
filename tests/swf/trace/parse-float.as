// makeswf -v 7 -r 1 -o parse-float-7.swf parse-float.as

#include "values.as"

trace (parseFloat ());
trace (parseFloat ("1.5", "2.5"));

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  trace (parseFloat (values[i]));
}

tests = [
  "0x123",
  "1.234e10",
  "1,5",
  "12e3",
  "5.12e0"
  ];

for (var i = 0; i < tests.length; i++) {
  trace ("Testing: " + tests[i]);
  trace (parseFloat (tests[i]));
}

loadMovie ("FSCommand:quit", "");
