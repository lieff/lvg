// makeswf -v 7 -r 1 -o test-7.swf test.as

ASnew = ASnative (2, 0);

function Test () {
  trace (ASnew ());
}

Test ();
var tmp = new Test ();

loadMovie ("FSCommand:quit", "");
