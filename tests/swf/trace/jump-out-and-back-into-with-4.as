// makeswf -v 7 -s 200x150 -r 1 -o movie68.swf movie68.as

// note that the real checks are binary-modified to produce the desired tests.
// The branch offsets have been adjusted.
trace ("Check how jumping out of a With and back in works");
x = 0;
o = new Object ();
o.x = -10;
with (o) {
  trace (x);
  x++;
  if (x > 10) {
    asm {
      branchalways "hi"
    };
  };
};
asm {
  stop
  branchalways "hi"
};

loadMovie ("FSCommand:quit", "");
