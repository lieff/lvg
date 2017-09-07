// makeswf -v 7 -r 1 -o string-old-upperlowercase-7.swf string-old-upperlowercase.as

var x = {};
x.toString = function () {
  return "Hello world! Ää!";
};
x.u = ASnative (102, 0);
x.l = ASnative (102, 1);
trace (x);
trace (x.l ());
trace (x.l ());

loadMovie ("FSCommand:quit", "");

