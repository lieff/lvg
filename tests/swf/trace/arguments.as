// makeswf -v 7 -r 1 -o test-7.swf test.as

function child () {
  trace ("arguments: " + arguments);
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
}
child.valueOf = child.toString = function () { return "child"; };

function func_and_child () {
  trace ("arguments: " + arguments);
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
  trace ("Child:");
  child ();
}
func_and_child.valueOf = func_and_child.toString =
  function () { return "func_and_child"; };

function prop_get () {
  trace ("");
  trace ("Prop get:");
  trace ("arguments: " + arguments);
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
  trace ("Child:");
  child ();
}
prop_get.valueOf = prop_get.toString = function () { return "prop_get"; };

function prop_set () {
  trace ("");
  trace ("Prop set:");
  trace ("arguments: " + arguments);
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
  trace ("Child:");
  child ();
}

function watcher () {
  trace ("");
  trace ("Watcher:");
  trace ("arguments: " + arguments);
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
  trace ("Child:");
  child ();
}
watcher.valueOf = watcher.toString = function () { return "watcher"; };

function timeout () {
  trace ("");
  trace ("Timeout:");
  trace ("arguments: " + arguments);
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
  trace ("Child:");
  child ();

  loadMovie ("FSCommand:quit", "");
}
timeout.valueOf = timeout.toString = function () { return "timeout"; };

function run () {
  var o = {};
  var tmp = {};


  trace ("Run:");
  trace ("arguments: " + arguments);
  trace ("caller: " + arguments.caller);
  trace ("callee: " + arguments.callee);
  trace ("Child CallFunction:");
  child ();
  trace ("Child CallMethod:");
  o.child = child;
  o.child ();


  o.toString = func_and_child;
  trace ("");
  trace ("toString:");
  tmp = "" + o;


  o.valueOf = func_and_child;
  trace ("");
  trace ("valueOf:");
  parseInt.valueOf = parseInt.toString = function () { return "parseInt"; };
  parseInt (tmp, o);


  trace ("");
  trace ("Method:");
  o.func_and_child = func_and_child;
  o.func_and_child ();


  trace ("");
  trace ("Call:");
  func_and_child.call ();


  trace ("");
  trace ("Apply:");
  func_and_child.apply ();


  trace ("");
  trace ("Sort:");
  tmp = new Array (2);
  tmp[0] = "a";
  tmp.sort (func_and_child);


  prop_set.valueOf = prop_set.toString = function () { return "prop_set"; };

  o.addProperty ("prop", prop_get, prop_set);

  o.prop = tmp;
  tmp = o.prop;


  o.watched = true;
  o.watch ("watched", watcher);
  o.watched = false;


  var emitter = new Object ();
  AsBroadcaster.initialize (emitter);
  emitter._listeners.push (new Object ());
  emitter._listeners[0].broadcast = function () {
    trace ("");
    trace ("Broadcast:");
    trace ("arguments: " + arguments);
    trace ("caller: " + arguments.caller);
    trace ("callee: " + arguments.callee);
    trace ("Child:");
    func ();
  };
  emitter._listeners[0].broadcast.valueOf = function () { return "broadcast"; };
  emitter._listeners[0].broadcast.toString = function () { return "broadcast"; };

  emitter.broadcastMessage ("broadcast");


  setTimeout (timeout, 0);
}
run.valueOf = run.toString = function () { return "run"; };

trace ("Global:");
trace ("arguments: " + arguments);
trace ("caller: " + arguments.caller);
trace ("callee: " + arguments.callee);
trace ("Child:");
child ();

run ();
