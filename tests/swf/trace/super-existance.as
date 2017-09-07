// makeswf -v 7 -s 200x150 -r 1 -o movie23.swf movie23.as

trace (super);
fuz = function () {
  trace ("fuz");
};
foo = function () extends fuz {
  trace ("foo");
  trace (super);
  trace (super.toString);
  super ();
};
bar = function () extends foo {
  trace ("bar");
  trace (super);
  if (huh) {
    asm {
      push 0, "huh"
      getvariable
      push undefined
      callmethod
    };
  }
  huh = super;
  asm {
    push 0, "super"
    callfunction
    pop
    push 0, "super"
    getvariable
    push undefined
    callmethod
    pop
  };
};
function call_huh () {
  trace ("calling last function's super object: " + huh);
  huh ();
};

trace (">>> CallFunction bar ()");
bar ();
call_huh ();
trace (">>> CallMethod bar ()");
asm {
  push 0, "super"
  getvariable
  push undefined
  callmethod
  pop
};
call_huh ();
trace (">>> this.bar ()");
this.bar ();
call_huh ();
trace (">>> new bar ()");
x = new bar ();
call_huh ();
trace (">>> new this.bar ()");
x = new this.bar ();
call_huh ();

loadMovie ("fscommand:quit", "");
