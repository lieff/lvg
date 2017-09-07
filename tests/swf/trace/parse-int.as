// makeswf -v 7 -r 1 -o parse-int-7.swf parse-int.as

#include "values.as"

for (var i = 0; i < values.length; i++) {
  trace ("Testing: " + names[i]);
  trace (parseInt (values[i]));
  trace (parseInt (values[i], 1));
  trace (parseInt (values[i], 37));
}

tests = [
  "0x123",
  "  0x123",
  "  0x",
  "  0xA",
  "  0xX",
  " a 0x123",
  "00x123",
  "+0x123",
  "-0x123",
  " +0x123",
  " -0x123",
  "+ 0x123",
  "- 0x123",
  "+0y123",
  "-0 x123",
  "0x+123",
  "0x-123",
  "  123",
  " 1 2 3",
  "1.23",
  "23.",
  "23asdf",
  "23.asdf",
  "asdf234",
  "+92",
  "-92",
  "+-92",
  "+-0",
  "A09C",
  "4294967296",
  "8589934592",
  "0123",
  " 0123",
  "+0123",
  "-0123",
  " +0123",
  " -0123",
  "+ 0123",
  "- 0123",
  "Infinity",
  "-Infinity"
  ];

for (var i = 0; i < tests.length; i++) {
  trace ("Testing: '" + tests[i] + "'");
  trace (parseInt (tests[i]));
  trace (parseInt (tests[i], 5));
  trace (parseInt (tests[i], 8));
  trace (parseInt (tests[i], 10));
  trace (parseInt (tests[i], 16));
  trace (int (tests[i]));
}

getURL ("FSCommand:quit", "");
