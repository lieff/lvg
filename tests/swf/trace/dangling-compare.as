// makeswf -v 7 -s 200x150 -r 1 -o dangling-compare.swf dangling-compare.as

aa = createEmptyMovieClip ("a", 0);
ab = aa;
a.removeMovieClip ();

ba = createEmptyMovieClip ("b", 1);
bb = ba;
b.removeMovieClip ();
trace (bb == ba);

ca = createEmptyMovieClip ("c", 2);
c._name = "d";
cb = d;
d.removeMovieClip ();

ea = createEmptyMovieClip ("e", 3);
e._name = "f";
eb = f;

createEmptyMovieClip ("d", 4);
createEmptyMovieClip ("b", 5);

array = [ undefined, aa, ab, ba, bb, ca, cb, d, b, e, ea, eb ];

for (i = 0; i < array.length; i++) {
  for (j = 0; j < array.length; j++) {
    trace (i + " == " + j + ": " + (array[i] == array[j]));
#if __SWF_VERSION__ > 5
    trace (i + " === " + j + ": " + (array[i] === array[j]));
#endif
  }
}

loadMovie ("fscommand:quit", "");
