// makeswf -v 7 -s 200x150 -r 1 -o movie68.swf movie68.as

// note that the real checks are binary-modified to produce the desired tests.
// The branch offsets have been adjusted.
trace ("Check various things when jumping into with commands");
x = 0;
o = new Object ();
o.x = -1;
with (o) {
  trace (x);
}
trace (x);
x++;
if (x < 10) {
  asm {
    branchalways "hi"
  };
}

loadMovie ("FSCommand:quit", "");
