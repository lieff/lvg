// makeswf -v 7 -s 200x150 -r 1 -o asbroadcaster-broadcastMessage-retval.swf asbroadcaster-broadcastMessage-retval.as

trace ("check return value of broadcastMessage");

emitter = new Object ();
AsBroadcaster.initialize (emitter);
trace (emitter.broadcastMessage ("foo"));
emitter._listeners[0] = "hi";
trace (emitter.broadcastMessage ("foo"));
emitter._listeners.length = 0;
trace (emitter.broadcastMessage ("foo"));
emitter._listeners.length = -1;
trace (emitter.broadcastMessage ("foo"));
emitter._listeners.length = 1;
trace (emitter.broadcastMessage ("foo"));
emitter._listeners[1] = null;
trace (emitter.broadcastMessage ("foo"));

loadMovie ("FSCommand:quit", "");
