// makeswf -v 7 -s 200x150 -r 1 -o function-scope.swf function-scope.as

trace ("Check the function scope chain works");
Foo = function () {
  var bla = "inner scope";
  this.bla = "this";
  this.func = function () {
    trace (bla);
    var bla = "innermost scope";
    this.func = function () {
      trace (bla);
    };
  };
};
bla = "outermost scope";
x = new Foo ();
x.func ();
x.func ();

loadMovie ("FSCommand:quit", "");
