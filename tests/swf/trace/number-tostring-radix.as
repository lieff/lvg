// makeswf -v 7 -r 1 -o test-7.swf test.as


var numbers = [
  0,
  -0.38,
  0.94,
  123.123
  -12303,
  1230312412
  /* FIXME: Flash player doesn't like these...
  12391283981421234812
  NaN,
  Infinity,
  Infinity,
  -Infinity*/
];

for (var i = 0; i < numbers.length; i++) {
  for (var radix = 0; radix <= 40; radix++) {
    trace (numbers[i].toString (radix));
  }
}

trace (n.toString (2));

loadMovie ("FSCommand:quit", "");
