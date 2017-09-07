// makeswf -v 7 -r 1 -o text-field-replace-text-7.swf text-field-replace-text.as

function check() {
  var fmt = t.getNewTextFormat();
  var str = fmt.size + ": ";
  var size = -1;
  for (var i = 0; i < t.length; i++) {
    fmt = t.getTextFormat (i);
    if (fmt.size != size) {
      size = fmt.size;
      str = str + size;
    }
    str = str + t.text.charAt (i);
  }
  trace (str);
}

this.createTextField ("t", 1, 0, 0, 100, 100);

t.html = true;
t.text = "Hello world!";

var fmt = new TextFormat ();
for (var i = 0; i < t.length; i++) {
  fmt.size = 6 + i;
  t.setTextFormat (i, i + 1, fmt);
}
fmt.size = 5;
fmt.color = 0xFF0000;
t.setNewTextFormat (fmt);

check ();

// won't work:
t.replaceText (1, "Hi");
check ();
t.replaceText (1, 0, "Hi");
check ();
t.replaceText (-1, 5, "Hi");
check ();
t.replaceText (50, 45, "Hi");
check ();
t.replaceText (10, 10, "");
check ();

// will work:
t.replaceText (3, 6, "Hi");
check ();
t.replaceText (1, 2, "");
check ();
t.replaceText (7, 8, "Hi");
check ();
t.replaceText (50, 55, "Hi");
check ();
t.replaceText (10, 20, "Hi");
check ();
t.replaceText (0, 3, "Yo!");
check ();

getURL ("FSCommand:quit", "");
