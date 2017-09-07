// makeswf -v 7 -s 200x150 -r 1 -o bitwise.swf bitwise.as

trace ("check Bitwise and shift operations");

numbers = [ Infinity, -Infinity, NaN,
  0.0, -0.0, 1, -1, -2, 2, 3, 42, 
  0.5, -0.5, 1.5, -1.5, 3.1415926535, -1.234567,
  0.000000001, 0.99999999,
  2147483647, 2147483648, 4294967295, 4294967296
];

for (i = 0; i < numbers.length; i++) {
  for (j = 0; j < numbers.length; j++) {
    trace (numbers[i] + " and " + numbers[j] + " = " + (numbers[i] & numbers[j]));
    trace (numbers[i] + " or " + numbers[j] + " = " + (numbers[i] | numbers[j]));
    trace (numbers[i] + " xor " + numbers[j] + " = " + (numbers[i] ^ numbers[j]));
  };
};

loadMovie ("FSCommand:quit", "");
