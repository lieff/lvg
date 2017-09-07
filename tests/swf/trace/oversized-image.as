// makeswf -v 7 -s 200x150 -r 1 -o oversized-image.swf oversized-image.as

createEmptyMovieClip ("a", 0);
a.loadMovie ("cookiemon.jpg");

createEmptyMovieClip ("b", 1);
b.loadMovie ("wookiemon.jpg");

count = 3;
onEnterFrame = function () {
  if (count-- == 0) {
    getURL ("fscommand:quit", "");
  };
};
