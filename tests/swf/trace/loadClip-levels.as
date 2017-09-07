// makeswf -v 7 -r 1 -o loadClip-levels.swf loadClip-levels.as

if (this != _level0) {
  trace ("hi, quitting from " + this);
  getURL ("FSCommand:quit", "");
  return;
}

trace ("testing target arguments for MovieClipLoader");
l = new MovieClipLoader ();
l.onLoadError = function (m, e, s) { trace ("onLoadError " + e + "/" + s + ": " + m); };
l.onLoadStart = function (m) { trace ("onLoadStart: " + m); };
l.onLoadProgress = function (m, l, t) { trace ("onLoadProgress " + l + "/" + t + ": " + m); };
l.onLoadComplete = function (m) { trace ("onLoadComplete: " + m); };
l.onLoadInit = function (m) { trace ("onLoadInit: " + m); };

l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", true);
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", 2);
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", "_level3");
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", _level4);
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", "5");
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", 6.54321);
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", { valueOf: function () { trace ("valueOf"); return 7; }, toString: function () { trace ("toString"); return "7"; } });
createEmptyMovieClip ("_level8", 8);
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", "_level8");
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", -10);
l.loadClip ("loadClip-levels-" + __SWF_VERSION__ + ".swf", 0xFFFFFFFF + 12);

x = { valueOf: function () { trace ("valueOf"); return 9; }, toString: function () { trace ("toString"); return "_level9"; } };
loadMovie ("loadClip-levels-" + __SWF_VERSION__ + ".swf", x);

onEnterFrame = function () {
  trace (_level1);
  trace (_level2);
  trace (_level3);
  trace (_level4);
  trace (_level5);
  trace (_level6);
  trace (_level7);
  trace (_level8);
  trace (_level9);
  trace (_level10);
  trace (_level11);
};
onEnterFrame ();

