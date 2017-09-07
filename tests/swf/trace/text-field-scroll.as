// makeswf -v 7 -r 1 -o test-7.swf test.as

function trace_scroll_properties () {
  trace ("bottomScroll: " + t.bottomScroll);
  trace ("hscroll: " + t.hscroll);
  trace ("maxhscroll: " + t.maxhscroll);
  trace ("maxscroll: " + t.maxscroll);
  trace ("scroll: " + t.scroll);
}

this.createTextField ("t", 1, 0, 0, 100, 25);

var fmt = new TextFormat ();
fmt.size = 14;
fmt.font = "Bitstream Vera Sans";
t.setNewTextFormat (fmt);

trace ("# EMPTY");
trace_scroll_properties ();

trace ("# WITH TEXT");

t.text = "asdfo jsofa jsapofjpsajf pjsadpfjsapf josapof jsafjposajf pjsapfj dposajf pojsap fjpasjpfjdsap jfpsajpfd jsapjfdpsajfjpsdajfp\r aposdjf jsapofj\r posajp posaj fsa jfdsapof jposaj fjsapofd paposdjf dsaj\r pfjsapd jpdsaj joasf jpoasjpfjdpsajfpsaj pfposa jfpsaj pfjdapsjfpajp\rasdfo jsofa jsapofjpsajf pjsadpfjsapf josapof jsafjposajf pjsapfj dposajf pojsap fjpasjpfjdsap jfpsajpfd jsapjfdpsajfjpsdajfp\r aposdjf jsapofj\r posajp posaj fsa jfdsapof jposaj fjsapofd paposdjf dsaj\r pfjsapd jpdsaj joasf jpoasjpfjdpsajfpsaj pfposa jfpsaj pfjdapsjfpajp\rasdfo jsofa jsapofjpsajf pjsadpfjsapf josapof jsafjposajf pjsapfj dposajf pojsap fjpasjpfjdsap jfpsajpfd jsapjfdpsajfjpsdajfp\r aposdjf jsapofj\r posajp posaj fsa jfdsapof jposaj fjsapofd paposdjf dsaj\r pfjsapd jpdsaj joasf jpoasjpfjdpsajfpsaj pfposa jfpsaj pfjdapsjfpajp\rasdfo jsofa jsapofjpsajf pjsadpfjsapf josapof jsafjposajf pjsapfj dposajf pojsap fjpasjpfjdsap jfpsajpfd jsapjfdpsajfjpsdajfp\r aposdjf jsapofj\r posajp posaj fsa jfdsapof jposaj fjsapofd paposdjf dsaj\r pfjsapd jpdsaj joasf jpoasjpfjdpsajfpsaj pfposa jfpsaj pfjdapsjfpajp";

t.scroll = 3;
t.hscroll = 200;

trace_scroll_properties ();

trace ("# WITH WORD WRAP");

t.wordWrap = true;

trace_scroll_properties ();

trace ("# WITHOUT WORD WRAP");

t.wordWrap = false;

trace_scroll_properties ();

trace ("# WITH WORD WRAP - SCROLL RESET");

t.wordWrap = true;
t.scroll = 3;

trace_scroll_properties ();

// FIXME: version 6 gives crazy value for maxscroll here
#if __SWF_VERSION__ != 6
trace ("# WITH WORD WRAP - FORMAT RESET");

var fmt = new TextFormat ();
fmt.font = "Bitstream Vera Sans";
t.setTextFormat (fmt);

trace_scroll_properties ();
#endif

trace ("# WITH WORD WRAP - TEXT RESET");

t.text = t.text;

trace_scroll_properties ();

t.scroll = 30;

trace ("# WITHOUT WORD WRAP - FORMAT RESET");

t.wordWrap = false;
var fmt = new TextFormat ();
fmt.font = "Bitstream Vera Sans";
t.setTextFormat (fmt);

trace_scroll_properties ();

trace ("# WITHOUT WORD WRAP - TEXT RESET");

t.text = t.text;

trace_scroll_properties ();

getURL ("FSCommand:quit", "");
