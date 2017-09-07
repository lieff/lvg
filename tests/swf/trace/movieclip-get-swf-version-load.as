// makeswf -v 7 -r 1 -o movieclip-get-swf-version-load-7.swf movieclip-get-swf-version-load.as

this.noquit = true;

for (var i = 5; i <= 8; i++) {
  this.createEmptyMovieClip ("m"+i, i);
  this["m"+i].loadMovie ("movieclip-get-swf-version-" + i + ".swf");
}

function quit () {
  trace ("After:");
  trace ("5: " + _root["m5"].getSWFVersion ());
  trace ("6: " + _root["m6"].getSWFVersion ());
  trace ("7: " + _root["m7"].getSWFVersion ());
  trace ("8: " + _root["m8"].getSWFVersion ());
  loadMovie ("FSCommand:quit", "");
}

setTimeout (quit, 1500);
