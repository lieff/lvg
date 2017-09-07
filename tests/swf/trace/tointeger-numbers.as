// makeswf -v 7 -s 200x150 -r 1 -o tointeger-numbers.swf tointeger-numbers.as

trace ("check ToInteger for various numbers");

numbers = [ Infinity, -Infinity, NaN,
  0.0, -0.0, 1, -1, -2, 2, 3, 42, 
  0.5, -0.5, 1.5, -1.5, 3.1415926535, -1.234567,
  0.000000001, 0.99999999,
  2147483647, 2147483648, 4294967295, 4294967296,
  -2147483647, -2147483648, -4294967295, -4294967296
];

for (i = 0; i < numbers.length; i++) {
  trace (numbers[i] + " ==int=> " + int (numbers[i]));
};

loadMovie ("FSCommand:quit", "");
