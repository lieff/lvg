// This ActionScript file defines a list of values that are considered 
// important for checking various ActionScript operations.
// It defines 2 variables:
// - "values": The array of values to be checked
// - "names": The array of corresponding string representations for values.
//            It's suggested to use these instead of using values directly to
//            avoid spurious toString and valueOf calls.

printall = new Object ();
printall.valueOf = function () {
  trace ("valueOf called");
  return this;
};
printall.toString = function () {
  trace ("toString called");
  return this;
};

printtostring = new Object ();
printtostring.toString = function () {
  trace ("toString called with " + arguments);
  return this;
};

printvalueof = new Object ();
printvalueof.valueOf = function () {
  trace ("valueOf called with " + arguments);
  return this;
};

nothing = new Object ();
nothing.__proto__ = undefined;
values = [ 
  undefined, 
  null, 
  true, false, 
  0, 1, 0.5, -1, -0.5, Infinity, -Infinity, NaN,
  "", "0", "-0", "0.0", "1", "Hello World!", "true", "_level0", "äöü",
  this, new Object (), new Date (1239752134235.94), new String (),
  Function, printall, printtostring, printvalueof, nothing ];

var l = values.length;
var v = function () {
  trace (this.nr + ": valueOf!");
  return this.v;
};
var s = function () {
  trace (this.nr + ": toString!");
  return this.v;
};
for (i = 0; i < l; i++) {
  var o = new Object ();
  o.nr = i;
  o.valueOf = v;
  o.toString = s;
  o.v = values[i];
  values.push (o);
};


names = [];
for (i = 0; i < values.length; i++) {
  names[i] = "(" + i + ") " + values[i] + " (" + typeof (values[i]) + ")";
};
