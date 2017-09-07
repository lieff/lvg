// makeswf -v 7 -s 200x150 -r 1 -o with-function-delete.swf with-function-delete.as

trace ("Check how delete works inside functions defined in a With statement");

x = 0;
_global.x = -1;
o = { x: 1 };
p = { x: 2 };
with (o) {
  with (p) {
    foo = function (x) {
      trace (x);
      delete x;
      trace (x);
      delete x;
    };
  };
};

foo (3);
foo (4);
foo (5);
foo (6);
foo (7);

loadMovie ("FSCommand:quit", "");
