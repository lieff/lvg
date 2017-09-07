// makeswf -v 7 -r 1 -o rectangle-contains-7.swf rectangle-contains.as

var tests = [
  "2",
  " 2",
  "\t\n \r2",
  "0x2",
  "0X2",
  " 0x2",
  " 0X2",
  "\t0x2",
  "1x2",
  " 1x2",
  "\t1x2",
  "030",
  " 030",
  "030.12",
  "030,12",
  "030e12",
  "030x12",
  "030A12",
  "030912",
  "030812",
  "030+12",
  "030-12",
  "030 12",
  ".2",
  " .2",
  "1.",
  " 1.",
  "-2",
  "--2",
  " -2",
  "- 2",
  "++2",
  "+ 2",
  " +2",
  "-+2",
  "\t\n \r-2",
  "-\t\n \r2",
  "-0x2",
  "0x-2",
  " -0x2",
  "- 0x2",
  "\t-0x2",
  "-1x2",
  " -1x2",
  "\t-1x2",
  "-.2",
  " -.2",
  "- .2",
  "-1.",
  " -1.",
  "- 1.",
  "AB",
  "0x2.1",
  " 0x2.1",
  "Inf",
  "Infinity",
  "-Infinity",
  "+Infinity",
  "iNFinity",
  "NaN",
  "11Infinity",
  "110x2",
  "a1",
  "1a",
  "",
  " ",
  "\t"];

for (var i = 0; i < tests.length; i++) {
  trace ("Testing: " + tests[i]);
  trace (int (tests[i]));
  trace (1 * tests[i]);
  trace (parseInt (tests[i]));
  trace (parseInt (tests[i], 10));
  trace (parseInt (tests[i], 16));
  trace (parseInt (tests[i], 8));
  trace (parseFloat (tests[i]));
}

loadMovie ("FSCommand:quit", "");
