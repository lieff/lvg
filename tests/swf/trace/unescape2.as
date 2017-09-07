// makeswf -v 7 -s 200x150 -r 1 -o unescape2.swf unescape2.as

trace ("Check unescaping of various interesting strings (add more if you find bugs)");

trace (unescape (""));

trace (unescape ("start%00end"));
trace (unescape ("start%c1end"));
trace (unescape ("start%c2end"));
trace (unescape ("start%c3end"));
trace (unescape ("start%ffend"));
trace (unescape ("start%feend"));
trace (unescape ("start%80end"));
trace (unescape ("start%Bfend"));
trace (unescape ("start%c0end"));

trace (unescape ("start%c0"));
trace (unescape ("start%6+1"));

trace (unescape ("start+end"));
trace (unescape ("start+end+++"));

trace (unescape ("start&end"));
trace (unescape ("start&end&"));

loadMovie ("FSCommand:quit", "");
