// makeswf -v 7 -s 200x150 -r 1 -o string-indexof.swf string-indexof.as

trace ("Check String.indexOf");

strings = [ "ABCDEFGHIabcdefghiABCDEFGHIabcdefghi",
  "ÄÖÜtütGHIabcdefghiABCDEFGHIabcdefghi"
];

for (i = 0; i < strings.length; i++) {
  foobar = strings[i];
  trace (">>> " + foobar);
  trace (foobar.length);
  trace (foobar.indexOf ("def"));
  trace (foobar.indexOf ("ghi", 3));
  trace (foobar.indexOf ("no-such-string"));
  trace (foobar.indexOf ("ABC", 30));
  trace (foobar.indexOf ("i", 36));
  trace (foobar.indexOf ("i", 35));
  trace (foobar.indexOf ("i", -15));
}

loadMovie("FSCommand:quit", "");

