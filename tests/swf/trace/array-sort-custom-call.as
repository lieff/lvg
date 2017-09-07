// makeswf -v 7 -r 1 -o array-sort-custom-call-7.swf array-sort-custom-call.as

function custom ()
{
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
  trace ("arguments: " + arguments);
}
custom.toString = function () { return "custom"; };

var a = [];

trace ("empty");
a.sort (custom);

trace ("one item");
a.push ("a");
a.sort (custom);

trace ("two items");
a.push ("b");
a.sort (custom);

loadMovie ("FSCommand:quit", "");
