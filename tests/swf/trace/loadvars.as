// makeswf -v 7 -r 1 -o loadvars-7.swf loadvars.as

var lv = new LoadVars ();

trace (lv.onLoad);
trace (lv.loaded);

lv.onDataReal = lv.onData;
lv.onData = function (src) {
  trace ("onData: " + src);
  trace (this.loaded);
  trace (this._bytesLoaded);
  trace (this._bytesTotal);
  lv.onDataReal (src);
};

lv.onLoad = function (success) {
  trace ("onLoad: " + success);
  trace (this.loaded);
  trace (this._bytesLoaded);
  trace (this._bytesTotal);
  var props = new Array ();
  for (var prop in this) {
    props.push (prop);
  }
  props.sort ();
  for (var i = 0; i < props.length; i++) {
    trace (props[i] + " = " + this[props[i]]);
  }
};

lv.load ("loadvars.txt");
lv.load ("404");

function quit () {
  loadMovie ("FSCommand:quit", "");
};

setInterval (quit, 1000);
