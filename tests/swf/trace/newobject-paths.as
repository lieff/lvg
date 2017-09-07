// makeswf -v 7 -s 200x150 -r 1 -o newobject-paths.swf newobject-paths.as

function construct (s) {
  var ret;
  asm {
    push "ret", 0, "s"
    getvariable
    dup
    trace
    new
    setvariable
  };
  return ret;
};

function Foo () {
  this.toString = function () {
    return "hi";
  };
};

createEmptyMovieClip ("a", 0);
a.Bar = Foo;

values = [
  "Foo",
  "/:Foo",
  "/:::Foo",
  "a.Bar",
  "a/Bar",
  "/a/..:a.Bar"
];

for (i = 0; i < values.length; i++) {
  trace (construct (values[i]));
}

loadMovie ("FSCommand:quit", "");
