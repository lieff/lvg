// makeswf -v 7 -s 200x150 -r 1 -o stage-fullscreen.swf stage-fullscreen.as

function print (stuff) {
  trace (">>> " + stuff);
  trace (Stage.displayState);
  trace (Stage.scaleMode);
  trace (Stage.width);
  trace (Stage.height);
};

Stage.addListener ({ onResize: function () { print ("onResize"); } });
Stage.addListener ({ onFullScreen: function (is_fs) { print ("fullscreen: " + is_fs); } });

onEnterFrame = function () {
  if (!work_around_stupid_size_bug) {
    /* wine or adobe's player don't set the correct window size when
       fullscreening before the window is shown */
    print ("before setting fullscreen");
    Stage.displayState = "fullscreen";
    print ("after setting fullscreen");
    work_around_stupid_size_bug = 1;
  } else if (Stage.displayState == "fullScreen") {
    print ("before Stage.displayState = \"normal\"");
    Stage.displayState = "normal";
    print ("after Stage.displayState = \"normal\"");
  } else if (Stage.scaleMode != "noScale") {
    print ("before Stage.scaleMode = \"noScale\"");
    Stage.scaleMode = "noScale";
    print ("before Stage.displayState = \"fullScreen\"");
    Stage.displayState = "fullScreen";
    print ("after Stage.displayState = \"fullScreen\"");
  } else {
    print ("before Stage.scaleMode = \"showAll\"");
    Stage.scaleMode = "showAll";
    print ("when quitting");
    getURL ("fscommand:quit", "");
  }
};

