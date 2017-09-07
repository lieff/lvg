// makeswf -v 7 -s 200x150 -r 1 -o construct-constructors.swf construct-constructors.as

trace ("Check that [] and {} don't call their constructors");

_global.Array = function () {
  trace ("  Array");
};
_global.Object = function () {
  trace ("  Object");
};
trace ("new Array ()");
x = new Array ();
trace ("[]");
x = [];
trace ("new Object ()");
x = new Object ();
trace ("{}");
x = {};

loadMovie ("FSCommand:quit", "");
