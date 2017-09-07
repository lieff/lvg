// makeswf -v 7 -s 200x150 -r 1 -o asbroadcaster-override.swf asbroadcaster-override.as

trace ("override AsBroadcaster methods");

AsBroadcaster.addListener = function () {
  trace ("add");
};
AsBroadcaster.removeListener = function () {
  trace ("remove");
};
AsBroadcaster.broadcastMessage = function () {
  trace ("broadcast");
};
function test () {
  emitter = new Object ();
  AsBroadcaster.initialize (emitter);
  emitter._listeners[0] = "hi";
  emitter.addListener (null);
  trace (emitter.broadcastMessage ("foo"));
  emitter.removeListener (null);
};

test ();

trace (_global.ASnative);
_global.ASnative = 22;
trace (_global.ASnative);

test ();

loadMovie ("FSCommand:quit", "");
