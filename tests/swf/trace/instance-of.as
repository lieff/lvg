// makeswf -v 7 -r 1 -o instance-of-7.swf instance-of.as

var empty_object_properties_added = {};
var new_object_properties_removed = new Object ();
ASSetPropFlags (new_object_properties_removed, null, 0, 7);
for (var prop in new_object_properties_removed) {
  empty_object_properties_added[prop] = new_object_properties_removed[prop];
  delete new_object_properties_removed[prop];
}

var new_array_proto_changed_and_restored = new Array ();
new_array_proto_changed_and_restored.__proto__ = XML.prototype;
new_array_proto_changed_and_restored.__proto__ = Array.prototype;

this.createTextField ("t", 0, 0, 0, 100, 100);

var objects = [];
objects.push ({ o: Object, n: "Object" });
objects.push ({ o: Object.prototype, n: "Object.prototype" });
objects.push ({ o: new Object (), n: "new Object ()" });
objects.push ({ o: Object (), n: "Object ()" });
objects.push ({ o: {}, n: "{}" });
objects.push ({ o: new_object_properties_removed, n: "new Object (), properties removed" });
objects.push ({ o: empty_object_properties_added, n: "{}, properties added" });
objects.push ({ o: String, n: "String" });
objects.push ({ o: String.prototype, n: "String.prototype" });
objects.push ({ o: new String (), n: "new String ()" });
objects.push ({ o: String (), n: "String ()" });
objects.push ({ o: "test", n: "\"test\"" });
objects.push ({ o: Number, n: "Number" });
objects.push ({ o: Number.prototype, n: "Number.prototype" });
objects.push ({ o: new Number (), n: "new Number ()" });
objects.push ({ o: Number (), n: "Number ()" });
objects.push ({ o: 1, n: "1" });
objects.push ({ o: Array, n: "Array" });
objects.push ({ o: Array.prototype, n: "Array.prototype" });
objects.push ({ o: new Array (), n: "new Array ()" });
objects.push ({ o: Array (), n: "Array ()" });
objects.push ({ o: [], n: "[]" });
objects.push ({ o: new_array_proto_changed_and_restored, n: "new Array (), __proto__ changed and restored" });
objects.push ({ o: XML, n: "XML" });
objects.push ({ o: MovieClip, n: "MovieClip" });
objects.push ({ o: this, n: "this" });
objects.push ({ o: TextField, n: "TextField" });
objects.push ({ o: t, n: "createTextField (...)" });

for (var i = 0; i < objects.length; i++) {
  trace (i + ": " + objects[i].n);
}
trace ("");

var str = "  ";
for (var i = 0; i < objects.length; i++) {
  if (i < 10)
    str += " ";
  str += " " + i;
}
trace (str);

for (var i = 0; i < objects.length; i++) {
  if (i < 10) {
    str = " ";
  } else {
    str = "";
  }
  str += i;
  for (var j = 0; j < objects.length; j++) {
    if (objects[i].o instanceOf objects[j].o) {
      if (j < 10)
	str += " ";
      str += " "+j;
    } else {
      str += "   ";
    }
  }
  trace (str);
}

getURL ("FSCommand:quit", "");
