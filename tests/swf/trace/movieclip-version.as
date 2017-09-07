// makeswf -v 7 -s 200x150 -r 1 -o movieclip-version.swf movieclip-version.as

trace (this);
trace ($version.substr (0, 3));
trace (this.$version.substr (0, 3));
if (this == _root) {
  for (i = 5; i <= 8; i++) {
    createEmptyMovieClip ("m" + i, i);
    loadMovie ("movieclip-version-" + i + ".swf", "m" + i);
    loadMovie ("movieclip-version-" + i + ".swf", "_level" + i);
  }
} else {
  delete this.$version;
  trace (this.$version);
  loadMovie ("fscommand:quit", "");
}
