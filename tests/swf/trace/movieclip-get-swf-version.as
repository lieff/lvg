// makeswf -v 7 -r 1 -o movieclip-get-swf-version-7.swf movieclip-get-swf-version.as

trace (this + ": " + this.getSWFVersion ());

if (_root["noquit"] != true)
  loadMovie ("FSCommand:quit", "");
