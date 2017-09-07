// makeswf -v 7 -s 200x150 -r 1 -o stack-segements2.swf stack-segements2.as

trace ("another test for SEGV in stack handling");

for (i = 0; i < 8200; i++) {
  asm {
    push "i"
    getvariable
  };
};
while (i != undefined) {
  trace (i);
  asm {
    push "i"
    swap
    setvariable
  };
};

loadMovie ("FSCommand:quit", "");
