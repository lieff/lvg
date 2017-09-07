// makeswf -v 7 -s 200x150 -r 1 -o array-new-override.swf array-new-override.as

MyArray = Array;
_global.Array = function () {
  trace ("hi");
};
trace (MyArray (1, 2, 3));
trace (new MyArray (1, 2, 3));
foo = MyArray (1, 2, 3);
trace (foo[1]);
trace (foo.length);
trace (["hello", "world"]);

loadMovie ("FSCommand:quit", "");
