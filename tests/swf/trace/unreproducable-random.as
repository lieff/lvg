// makeswf -v 7 -s 200x150 -r 1 -o unreproducable-random.swf unreproducable-random.as

trace ("This is an internal Swfdec test to ensure tests are internally predictable.");
trace ("You cannot reproduce this test with any other Flash player.");

for (i = 0; i < 100; i++) {
  asm {
    push 100
    random
    trace
  };
  trace (Math.random ());
}

trace (new Date ());

loadMovie ("fscommand:QUIT", "");
