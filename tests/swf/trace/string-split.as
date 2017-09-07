// makeswf -v 7 -s 200x150 -r 1 -o string-split.swf string-split.as

trace ("Check String.split()");

#include "values.as"

s = new String ("aaaa");
trace (s.split ());
trace (typeof (s.split ()));
trace (s.split ("a"));
trace (s.split ("a", 3));
trace (s.split ("a", -2));

s = names.toString ();
for (i = 0; i < values.length; i++) {
  trace (">>> split (\"" + names[i] + "'\")");
  a = s.split (names[i]);
  trace (a.length);
  for (j = 0; j < a.length; j++)
    trace (a[j]);
  l = a.length + 1;
  if (l > 8)
    l = 10;
  for (j = -1; j < l; j++) {
    trace (">>> split (\"" + names[i] + "\", " + j + ")");
    trace (s.split (names[i], j));
  };
  trace (">>> split (" + names[i] + ")");
  a = s.split (String (values[i]));
  for (j = 0; j < a.length; j++)
    trace (a[j]);
  trace (a.length);
  l = a.length + 1;
  if (l > 8)
    l = 10;
  for (j = -1; j < l; j++) {
    trace (">>> split (" + names[i] + ", " + j + ")");
    trace (s.split (values[i], j));
  };
};

loadMovie ("FSCommand:quit", "");
