// makeswf -v 7 -r 1 -o loadobject-bom-7.swf loadobject-bom.as

var files = [
  "utf-8-bom.txt",
  "utf-16-be-bom.txt",
  "utf-16-le-bom.txt",
  "utf-32-be-bom.txt",
  "utf-32-le-bom.txt"
];
var file_num = 0;

var obj = new Object ();

obj.load = ASnative (301, 0);

obj.onData = function (str) {
  trace ("Got: " + typeof (str) + " '" + str + "'");
  trace ("Loaded: " + this._bytesLoaded);
  trace ("Total: " + this._bytesTotal);
  trace ("Is loaded: " + obj.loaded);

  file_num++;
  if (file_num < files.length) {
    trace ("Loading: " + files[file_num]);
    obj.load (files[file_num]);
  } else {
    loadMovie ("FSCommand:quit", "");
  }
};

trace ("Loading: " + files[file_num]);
obj.load (files[file_num]);
