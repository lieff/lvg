// makeswf -v 7 -s 200x150 -r 1 -o shift.swf shift.as

function Test (v) {
  this.v = v;
  this.valueOf = function () { 
    trace (this.v + ": valueOf!");
    return this.v; 
  };
};

numbers = [ Infinity, -Infinity, NaN,
  0.0, -0.0, 1, -1, -2, 2, 3, 42, 
  31, 32, 33, -31, -32, -33,
  2147483647, 2147483648, 4294967295, 4294967296,
  -2147483647, -2147483648, -4294967295, -4294967296,
  new Test(1), new Test (2)
];

for (i = 0; i < numbers.length; i++) {
  for (j = 0; j < numbers.length; j++) {
    trace (numbers[i] + " << " + numbers[j] + " = " + (numbers[i] << numbers[j]));
    trace (numbers[i] + " >> " + numbers[j] + " = " + (numbers[i] >> numbers[j]));
    trace (numbers[i] + " >>> " + numbers[j] + " = " + (numbers[i] >>> numbers[j]));
  };
}


loadMovie ("FSCommand:quit", "");
