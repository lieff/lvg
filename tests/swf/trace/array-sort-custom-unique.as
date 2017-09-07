// makeswf -v 7 -r 1 -o array-sort-custom-unique-7.swf array-sort-custom-unique.as

function custom (a, b)
{
  trace ("custom (" + a + ", " + b + ")");
  if (a < b) {
    trace (-1);
    return -1;
  } else if (b < a) {
    trace (1);
    return 1;
  } else {
    trace (0);
    return 0;
  }
}

function custom_not_equal (a, b)
{
  trace ("custom (" + a + ", " + b + ")");
  if (a < b) {
    trace (-1);
    return -1;
  } else if (b < a) {
    trace (1);
    return 1;
  } else {
    trace ("1 instead of 0");
    return 1;
  }
}

var a = ["a", "c", "b", "d"];
trace (a.sort (custom, Array.UNIQUESORT));

var a = ["a", "c", "b", "d", "c"];
trace (a.sort (custom, Array.UNIQUESORT));

var a = ["a", "c", "b", "d"];
trace (a.sort (custom_not_equal, Array.UNIQUESORT));

var a = ["a", "c", "b", "d", "c"];
trace (a.sort (custom_not_equal, Array.UNIQUESORT));

var a = ["a", "c", "b", "d", "C"];
trace (a.sort (custom, Array.UNIQUESORT));

var a = ["a", "c", "b", "d", "C"];
trace (a.sort (custom_not_equal, Array.UNIQUESORT));

var a = ["a", "c", "b", "d", "C"];
trace (a.sort (custom, Array.UNIQUESORT | Array.CASEINSENSITIVE));

var a = ["a", "c", "b", "d", "C"];
trace (a.sort (custom_not_equal, Array.UNIQUESORT | Array.CASEINSENSITIVE));

var a = ["a", "01", "b", "d", "1"];
trace (a.sort (custom, Array.UNIQUESORT));

var a = ["a", "01", "b", "d", "1"];
trace (a.sort (custom_not_equal, Array.UNIQUESORT));

var a = ["a", "01", "b", "d", "1"];
trace (a.sort (custom, Array.UNIQUESORT | Array.NUMERIC));

var a = ["a", "01", "b", "d", "1"];
trace (a.sort (custom_not_equal, Array.UNIQUESORT | Array.NUMERIC));

function custom_zero (a, b)
{
  trace ("custom (" + a + ", " + b + ")");
  return 0;
}

var a = ["a", "b", "a"];
trace (a.sort (custom, Array.UNIQUESORT));

var a = ["a", "b", "a"];
trace (a.sort (custom_zero, Array.UNIQUESORT));

getURL ("FSCommand:quit", "");
