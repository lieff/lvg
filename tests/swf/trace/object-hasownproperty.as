// makeswf -v 7 -r 1 -o object-hasownproperty-7.swf object-hasownproperty.as

var o = new Object ();
o.hasOwnProperty = ASnative (101, 5);

o[0] = 0;
for (var i = 0; i <= 13; i++) {
  o[1 << i] = 1 << i;
  ASSetPropFlags (o, 1 << i, 1 << i, 0);
}

trace ("(no param): " + o.hasOwnProperty ());
trace ("not_found: " + o.hasOwnProperty ("not_found"));
trace ("0: " + o.hasOwnProperty (0));
for (var i = 0; i <= 13; i++) {
  trace (1 << i + ": " + o.hasOwnProperty (1 << i));
}

loadMovie ("FSCommand:quit", "");
