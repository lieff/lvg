// /usr/bin/makeswf -s 200x150 -o empty-stack.swf empty-stack.as

trace ("Check that the empty stack is treated as unlimited undefined values");
asm {
  pop
  trace
};
