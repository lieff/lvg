.flash bbox=150x200 filename="createEmptyMovieClip-events.swf" background=white version=8 fps=15

.frame 1
  .action:
    MovieClip.prototype.onLoad = function () {
      trace ("onLoad: " + this);
      trace ("arguments: " + arguments);
      trace ("caller: " + arguments.caller);
      trace ("callee: " + arguments.callee);
    };

    MovieClip.prototype.onConstruct = function () {
      trace ("onConstruct: " + this);
      trace ("arguments: " + arguments);
      trace ("caller: " + arguments.caller);
      trace ("callee: " + arguments.callee);
    };

    TextField.prototype.onLoad = function () {
      trace ("TextField onLoad");
    };

    TextField.prototype.onConstruct = function () {
      trace ("TextField onConstruct");
    };

    trace ("Creating: a");
    createEmptyMovieClip ("a", 0);

    trace ("Creating duplicate: b");
    a.duplicateMovieClip ("b", 1);

    trace ("Creating child: c");
    a.createEmptyMovieClip ("c", 0);

    trace ("Creating text field: d");
    createTextField ("d", 2, 0, 0, 100, 100);
  .end

.frame 15
  .action:
    getURL("fscommand:quit", "");
  .end

.end
