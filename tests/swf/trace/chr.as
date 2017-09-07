// makeswf -v 7 -s 200x150 -r 1 -o chr.swf chr.as

for (i = 0; i < 100000; i += 100) {
  trace (i);
  trace (chr (i));
  asm {
    push "i"
    getvariable
    mbchr
    trace
  };
};

loadMovie ("FSCommand:quit", "");
