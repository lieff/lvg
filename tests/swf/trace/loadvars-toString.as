// makeswf -v 7 -s 200x150 -r 1 -o loadvars-toString.swf loadvars-toString.as

// can't test the joining here, as the joining is dependant on hash table order

trace (LoadVars.prototype);

Array.prototype.push = function (x) {
  trace ("pushed " + x);
  ASnative (252,1).call (this, x);
};
_global.escape = function (s) {
  trace ("escaping " + s);
  var fun = ASnative (100, 0);
  return fun (s);
};
l = new LoadVars ();
l.x = 42;
trace (l.toString ());

getURL ("fscommand:quit", "");
