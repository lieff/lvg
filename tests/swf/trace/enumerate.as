// makeswf -v 7 -s 200x150 -r 1 -o enumerate.swf enumerate.as

x = 0;
createEmptyMovieClip ("a", 0);
a.y = 1;
a.createEmptyMovieClip ("a", 0);
a.a.z = { w: 42 };

function do_test (x) {
  asm {
    push "x"
    getvariable
    enumerate
    push undefined
    equals
    trace
  };
};

tests = [ "/a/a", "a/../a::::a", "", "a/..:a", "a.a", "a.a.z" ];

for (i = 0; i < tests.length; i++) {
  do_test (tests[i]);
};


loadMovie ("FSCommand:quit", "");
