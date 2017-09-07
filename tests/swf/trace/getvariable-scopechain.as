// makeswf -v 7 -s 200x150 -r 1 -o getvariable-scopechain.swf getvariable-scopechain.as

trace ("Check behavior of scope chain in getvariable lookups");

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
a.a.createEmptyMovieClip ("a", 0);
x = 0;
a.x = 1;
a.a.x = 2;
a.a.a.x = 3;
a.a.a = { x: 4 };
a.a = { x: 5 };
//a = { x: 6 };
with (a) {
  asm {
    push "a:x"
    getvariable
    trace
  };
  asm {
    push "a/a:x"
    getvariable
    trace
  };
  asm {
    push "a/a"
    getvariable
    trace
  };
};
asm {
  push "Number:MAX_VALUE"
  getvariable
  trace
};

loadMovie ("FSCommand:quit", "");
