// makeswf -v 7 -s 200x150 -r 1 -o stack-segment.swf stack-segment.as

trace ("Make sure the stack doesn't fall over when a new segment is pushed");

function foo () {
  trace (arguments);
};

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
