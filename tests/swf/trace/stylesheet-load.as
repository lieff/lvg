// makeswf -v 7 -r 1 -o stylesheet-parse-7.swf stylesheet-parse.as

var style = new TextField.StyleSheet ();

style.onLoad = function () {
  trace ("onLoad: " + arguments);

  var names = style.getStyleNames ();
  names.sort ();
  for (var j = 0; j < names.length; j++) {
    trace ("* '" + names[j] + "'");

    var o = style.getStyle (names[j]);

    var props = [];
    for (var prop in o) {
      props.push (prop);
    }
    props.sort ();
    for (var k = 0; k < props.length; k++) {
      var prop = props[k];
      trace (prop + ": '" + o[prop] + "'");
    }
  }
};

style.onDataReal = style.onData;
style.onData = function () {
  trace ("onData: " + arguments);
  style.onDataReal.apply (this, arguments);
};

style.load ("stylesheet-load.css");
style.load ("404");

function quit () {
  loadMovie ("FSCommand:quit", "");
}

setTimeout (quit, 500);
