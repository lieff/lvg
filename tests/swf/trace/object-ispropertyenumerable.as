// makeswf -v 7 -r 1 -o object-ispropertyenumerable-7.swf object-ispropertyenumerable.as

var o = new Object ();
o.hasOwnProperty = ASnative (101, 5);

o[0] = 0;
for (var i = 0; i <= 13; i++) {
  o[1 << i] = 1 << i;
  ASSetPropFlags (o, 1 << i, 1 << i, 0);
  o[(1 << i) | 1] = (1 << i) | 1;
  ASSetPropFlags (o, (1 << i) | 1, (1 << i) | 1, 0);
}

trace ("(no param): " + o.isPropertyEnumerable ());
trace ("not_found: " + o.isPropertyEnumerable ("not_found"));
trace ("0: " + o.isPropertyEnumerable (0));
for (var i = 0; i <= 13; i++) {
  trace ((1 << i) + ": " + o.isPropertyEnumerable (1 << i));
  trace (((1 << i) | 1) + ": " + o.isPropertyEnumerable ((1 << i) | 1));
}

loadMovie ("FSCommand:quit", "");
