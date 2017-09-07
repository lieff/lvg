// makeswf -v 7 -s 200x150 -r 1 -o lots-of-arguments.swf lots-of-arguments.as

trace ("Calling a function with 10000 arguments just works");

function foo () {
  trace (arguments);
};

for (i = 0; i < 10000; i++) {
  asm {
    push "i"
    getvariable
  };
};
asm {
  push 20000
  push "foo"
  callfunction
};

loadMovie ("FSCommand:quit", "");
