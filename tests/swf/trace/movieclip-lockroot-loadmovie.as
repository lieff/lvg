// makeswf -v 7 -s 200x150 -r 1 -o movieclip-lockroot-loadmovie.swf movieclip-lockroot-loadmovie.as

if (this._name == "") {
  _level0.value = "parent";
  this._name = "m";
}

trace (this._name);
trace (this._lockroot);
trace (_root.value);

if (this._name.length <= 3) {
  for (i = 6; i <= 8; i++) {
    var c = this.createEmptyMovieClip (this._name + "_" + i, i);
    c._lockroot = true;
    loadMovie ("movieclip-lockroot-loadmovie-" + i + ".swf", c);
  }
} else {
  getURL ("FSCommand:quit", "");
}
