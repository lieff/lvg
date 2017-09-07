// makeswf -v 7 -s 200x150 -r 1 -o this-localfunction.swf this-localfunction.as

trace ("Check the this object of local functions");

function test () {
  o = { x: 2 };
  var x = 3;
  var bla = function () {
    trace (this);
    trace (this.x);
  };
  bla ();
  with (o) {
    bla ();
  }
};
x = 1;
test ();

loadMovie ("FSCommand:quit", "");
