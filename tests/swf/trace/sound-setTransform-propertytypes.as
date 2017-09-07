// makeswf -v 7 -s 200x150 -r 1 -o sound-setTransform-propertytypes.swf sound-setTransform-propertytypes.as

Sound.prototype.toString = function () {
  var tf = this.getTransform ();
  return tf + ": { " + tf.ll + ", " + tf.rl + ", " + tf.lr + ", " + tf.rr + "} (" + this.getVolume () + ", " + this.getPan () + ")";
};

N = 5;
vals = { ll: 44, lr: 42, rl: 24, rr: 22 };
function modify (o, name, num) {
  //trace (num);
  switch (num) {
    case 0:
      break;
    case 1:
      o[name] = vals[name];
      break;
    case 2:
      o.addProperty (name, function () { trace (name); return vals[name] * 2; }, null);
      break;
    case 3:
      o.__proto__[name] = vals[name] / 2;
      break;
    case 4:
      o[name] = { valueOf: function () { trace ("valueOf " + name); return vals[name]; } };
      break;
    default:
      trace ("ERROR");
      break;
  }
};

for (i = 0; i < N * N * N * N; i++) {
  o = { __proto__: {}, toString: function () { return "(" + (o.ll ? o.ll : "  ") + (o.lr ? o.lr : "  ") + (o.rl ? o.rl : "  ") + (o.rr ? o.rr : "  ") + ")"; } };
  modify (o, "ll", int (i) % N);
  modify (o, "lr", int (i / N) % N);
  modify (o, "rl", int (i / N / N) % N);
  modify (o, "rr", int (i / N / N / N) % N);
  createEmptyMovieClip ("a", 0);
  x = new Sound (a);
  x.setTransform (o);
  trace (i + " " + o + " " + x);
};

getURL ("fscommand:quit", "");
