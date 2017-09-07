// makeswf -v 7 -r 1 -o array-sort-compare-modify-7.swf array-sort-compare-modify.as

function compare (a, b)
{
  if (a < b) {
    return -1;
  } else if (b < a) {
    return 1;
  } else {
    return 0;
  }
}

var x = ["a", "d", "b", "c", "e", "g", "f"];

function remove (a, b)
{
  x.length = 0;
  return compare (a, b);
}

trace (x.sort (remove));


var x = ["a", "d", "b", "c", "e", "g", "f"];

function append (a, b)
{
  x[7] = "h";
  x[8] = "i";
  return compare (a, b);
}

trace (x.sort (append));

loadMovie ("FSCommand:quit", "");
