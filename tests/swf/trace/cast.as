// makeswf -v 7 -r 1 -o cast-7.swf cast.as

var o = new Object ();

asm {
  push "o"
  getvariable
  push "Object"
  getvariable
  cast
  trace
};

asm {
  push "Object"
  getvariable
  push "o"
  getvariable
  cast
  trace
};

loadMovie ("FSCommand:quit", "");
