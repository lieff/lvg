// makeswf -v 7 -s 200x150 -r 1 -o getsetproperty-target.swf getsetproperty-target.as

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);

function do_test (x) {
  // equivalent code:
  // trace (eval (x)._target);
  // eval (x)._x = 42;
  // trace (eval (x)._x);
  asm {
    push "x"
    getvariable
    dup
    push 11
    getproperty
    trace
    dup
    push 0, 42
    setproperty
    push 0
    getproperty
    trace
  };
};

tests = [ "/a/a", "a/../a::::a", "" ];

for (i = 0; i < tests.length; i++) {
  do_test (tests[i]);
};


loadMovie ("FSCommand:quit", "");
