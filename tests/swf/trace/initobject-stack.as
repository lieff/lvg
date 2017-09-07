// makeswf -v 7 -s 200x150 -r 1 -o initobject-stack.swf initobject-stack.as

trace ("Check that InitObject consumes the right amount of stack");

// Don't eat too much stack
asm {
  push 3, 2, 1
};
o = { one:1, two:2 };
asm {
  trace
  trace
  trace
};

// Don't try to eat nonexisting stack
asm {
  push "hi", undefined, "undefined", 1, "one", 2, "two", 5
  initobject
  push "hi"
  swap
  setvariable
  trace
};
trace (hi);
trace (hi.hasOwnProperty ("hi"));
trace (hi.hasOwnProperty ("undefined"));

loadMovie ("FSCommand:quit", "");
