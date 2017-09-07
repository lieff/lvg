// makeswf -v 7 -r 1 -o array-sort-unique-abort-7.swf array-sort-unique-abort.as

trace ("Test whether the sorting is aborted with UNIQUESORT, when the first equal element is found");


trace ("Custom function: ");

var times = 0;
function compare (a, b)
{
  times++;
  return 0;
}

var a = [];
a.length = 5;
trace (a.sort (compare, Array.UNIQUESORT));
trace (times <= 1);


trace ("Builtin function: ");

var o = new Object ();
times = 0;
o.toString = function () {
  times++;
  return "o";
};

var a = [];
a.push (o, o, o, o, o);
trace (a.sort (Array.UNIQUESORT));
trace (times <= 2);

loadMovie ("FSCommand:quit", "");
