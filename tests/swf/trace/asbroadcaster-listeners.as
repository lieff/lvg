// makeswf -v 7 -s 200x150 -r 1 -o asbroadcaster-listeners.swf asbroadcaster-listeners.as

trace ("Check what happens if _listeners is not an array");

emitter = new Object ();
AsBroadcaster.initialize (emitter);
o1 = new Object ();
o1.foo = function () {
  trace ("o1");
};
emitter._listeners = new Object ();
emitter._listeners[10] = o1;
trace (emitter._listeners[10]);
trace (emitter.broadcastMessage ("foo"));

loadMovie ("FSCommand:quit", "");
