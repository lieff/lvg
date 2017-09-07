// makeswf -v 7 -s 200x150 -r 15 -o crash-0.6.6-create-object.swf crash-0.6.6-create-object.as

function Test () {
  this.__proto__.__constructor__ = Date;
  super ();
}
var t = new Test ();

trace ("Qapla!");

getURL ("fscommand:quit", "");
