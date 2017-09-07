// makeswf -v 7 -r 1 -o array-sort-tostring-7.swf array-sort-tostring.as

function trace_toString () {
  trace ("toString called for " + this.value + " @ " + this.position);
  return "" + this.value;
}

function trace_valueOf () {
  trace ("valueOf called for " + this.value + " @ " + this.position);
  return this.value;
}

var values = [ 3, 3, 8, 7, 5, 5, 9, 8, 8, 8, 9, 4, 2, 3, 8, 1, 6, 6, 3, 3 ];
var a = [];
for (var i = 0; i < values.length; i++) {
  a[i] = new Object ();
  a[i].position = i;
  a[i].value = values[i];
  a[i].toString = trace_toString;
  a[i].valueOf = trace_valueOf;
}

trace (a);
trace (a.concat ().sort ());
trace (a.concat ().sort (Array.NUMERIC));
trace (a.concat ().sort (Array.DESCENDING));

loadMovie ("FSCommand:quit", "");
