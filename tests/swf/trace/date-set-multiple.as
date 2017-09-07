// makeswf -v 7 -s 200x150 -r 15 -o date-set-multiple-7.swf date-set-multiple.as

trace ("Setting multiple fields with single setWhatever call");

var methods = [
  "setYear",
  "setFullYear",
  "setUTCFullYear",
  "setMonth",
  "setUTCMonth",
  "setDate",
  "setUTCDate",
  "setHours",
  "setUTCHours",
  "setMinutes",
  "setUTCMinutes",
  "setSeconds",
  "setUTCSeconds",
  "setMilliseconds",
  "setUTCMilliseconds"
];

for (var i = 0; i < methods.length; i++) {
  trace ("Testing: " + methods[i]);
  var d = new Date (2020, 10, 15, 14, 13 ,12, 11);
  d[methods[i]] (1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
  trace (d);
  trace (d.valueOf ());
}

getURL ("fscommand:quit");
