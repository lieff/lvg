// makeswf -v 7 -s 200x150 -r 1 -o crash-0.7.5-duplicate.swf crash-0.7.5-duplicate.as

createEmptyMovieClip ("a", 0);

onEnterFrame = function () {
  a.duplicateMovieClip ("b", 1);

  getURL ("fscommand:quit", "");
};
