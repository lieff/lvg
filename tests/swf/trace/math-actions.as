// makeswf -v 7 -s 200x150 -r 1 -o math-actions.swf math-actions.as

numbers = [ Infinity, -Infinity, NaN,
  0.0, -0.0, 1, -1, -2, 2, 11, -11,
  0.5, -0.5, 1.5, -1.5, 3.1415926535, -1.234567,
  0.000000001, 0.99999999,
  2147483647, 2147483648, 4294967295, 4294967296,
  -2147483647, -2147483648, -4294967295, -4294967296
];

add_object = function (x) {
  o = new Object ();
  o.x = x;
  o.valueOf = function () {
    trace (this.x + " valueOf!");
    return this.x;
  };
  o.toString = function () {
    trace (this.x + " toString!");
    return this.x;
  };
  numbers.push (o);
};
add_object (42);
add_object (-42);

for (i = 0; i < numbers.length; i++) {
  for (j = 0; j < numbers.length; j++) {
    trace (numbers[i] + " + " + numbers[j] + " = " + (numbers[i] + numbers[j]));
    trace (numbers[i] + " - " + numbers[j] + " = " + (numbers[i] - numbers[j]));
    trace (numbers[i] + " * " + numbers[j] + " = " + (numbers[i] * numbers[j]));
    trace (numbers[i] + " / " + numbers[j] + " = " + (numbers[i] / numbers[j]));
    trace (numbers[i] + " % " + numbers[j] + " = " + (numbers[i] % numbers[j]));
  };
};

loadMovie ("FSCommand:quit", "");
