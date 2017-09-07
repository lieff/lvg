// makeswf -n -v 7 -r 1 -o crash-0.6.0-moviecliploader.swf crash-0.6.0-moviecliploader.as

// NOTE: Has to be compiled with -n

trace ("Start");

this.createEmptyMovieClip ("a", 0);
var l = new MovieClipLoader ();
l.loadClip ("getvariable-exhaustive-8.swf", a);

function quit () {
  trace ("Done");
  loadMovie ("FSCommand:quit", "");
}

setInterval (quit, 2000);
