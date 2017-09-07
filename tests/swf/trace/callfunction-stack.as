// /usr/bin/makeswf -s 200x150 -o callfunction-stack.swf callfunction-stack.as

trace ("Check that function calls using empty stack work as expected");
function foo () {
  trace (arguments.length);
  for (i = 0; i < arguments.length; i++) {
    trace (arguments[i]);
  };
}
asm {
  push 5, "foo"
  callfunction
  pop
};
asm {
  push 1, 2, 3, 5, "foo"
  callfunction
  pop
};
asm {
  push 1, 2, 5, "foo"
  callfunction
  pop
};
asm {
  push "foo"
  callfunction
  pop
};
asm {
  push "hi", "foo"
  callfunction
  pop
};
asm {
  push 42, "+1", "foo"
  callfunction
  pop
};
asm {
  push 268435455, "foo"
  callfunction
  pop
};
