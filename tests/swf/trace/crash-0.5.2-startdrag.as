// makeswf -v 7 -s 200x150 -r 1 -o crash-0.5.2-startdrag.swf crash-0.5.2-startdrag.as

asm {
  push 1, "this"
  getvariable
  startdrag
};

loadMovie ("FSCommand:quit", "");
