// makeswf -v 7 -s 200x150 -r 1 -o movie2.swf movie2.as

array = [ 0, undefined, null, 1, -1, -0, new Object (), this, getDepth, "1", "0", "", "foo" ];

for (i = 0; i < array.length; i++) {
  for (j = 0; j < array.length; j++) {
    trace (array[i] + " (" + typeof (array[i]) + ") / " + array[j] + " (" + typeof (array[j]) + ") = " + (array[i] / array[j]));
  };
};
