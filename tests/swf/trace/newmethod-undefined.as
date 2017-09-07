// makeswf -v 7 -s 200x150 -r 1 -o newmethod-undefined.swf newmethod-undefined.as

foo = function () {
  trace ("w00t");
};

test = function (v) {
  trace (">>> " + v + " (" + typeof (v) + "}");
  asm {
    push 0, "foo"
    getvariable
    push "v"
    getvariable
    newmethod
    pop
  };
};

test (undefined);
test ("");
test (null);
test ({ valueOf: function () { trace ("valueOf"); return ""; }, toString: function () { trace ("toString"); return ""; } });

loadMovie ("FSCommand:quit", "");
