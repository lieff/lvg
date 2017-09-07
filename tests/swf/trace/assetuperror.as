// makeswf -v 7 -r 1 -o test-7.swf test.as

trace (AsSetupError ("Test1,Test2"));
trace (Test1);
trace (Test2);

_global.Error = function () {
  trace ("new Error:");
  trace ("this: " + this);
  trace ("arguments: " + arguments);
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
};
_global.Error.toString = function () { return "Error"; };
AsSetupError.toString = function () { return "AsSetupError"; };

trace ("--");
trace (AsSetupError ("Test1,Test2"));
trace ("--");
trace (AsSetupError ());
trace ("--");
trace (AsSetupError ("Hello world!"));
trace ("--");
trace (AsSetupError (",,,,,,,,,,,,"));

loadMovie ("FSCommand:quit", "");
