// makeswf -v 7 -r 1 -o text-field-html-new-format-7.swf text-field-html-new-format.as

// open tags in HTML input change the new text format of the text field

var fmt = new TextFormat ();
fmt.size = 50;


this.createTextField ("t", 0, 0, 0, 150, 150);
t.html = true;

t.htmlText = "hello<font size='100'>world";
trace (t.htmlText);

t.text = "terve";
trace (t.htmlText);


this.createTextField ("t", 0, 0, 0, 150, 150);
t.html = true;

t.htmlText = "hello<font size='100'>world";
trace (t.htmlText);

t.setNewTextFormat (fmt);
t.text = "terve";
trace (t.htmlText);


this.createTextField ("t", 0, 0, 0, 150, 150);
t.html = true;

t.htmlText = "hello<font size='100'>world";
trace (t.htmlText);

t.htmlText = "hello<font size='100'>world";
trace (t.htmlText);


loadMovie ("FSCommand:quit", "");

