// makeswf -v 7 -r 1 -o loadvariables-target-7.swf loadvariables-target.as

// see what loadVariables does when target is a TextField or an object
this.createTextField ("a", 0, 0, 0, 100, 100);
this.b = new Object ();

a.onData = function () {
  trace ("onData a: " + this.test);
};

b.onData = function () {
  trace ("onData b: " + this.test);
};

loadVariables ("loadvars.txt", "a");
loadVariables ("loadvars.txt", "b");

function quit () {
  trace ("quit a: " + a.test);
  trace ("quit b: " + b.test);
  loadMovie ("FSCommand:quit", "");
}

setTimeout (quit, 1500);
