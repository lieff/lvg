// makeswf -v 6 -r 1 -o array2-6.swf array2.as
// makeswf -v 7 -r 1 -o array2-7.swf array2.as

// TODO: version 5 compatible version, inserting and reading, sorton

// define our own function to print arrays, that shows nested arrays nicely
function pretty (arr, lvl) {
  var i;
  var str = "[";
  for (i = 0; i < arr.length; i++) {
    if (arr[i] instanceOf Array) {
      str = str + pretty (arr[i], lvl + 1);
    } else {
      str = str + arr[i];
    }
    if (i != arr.length - 1)
      str += ", ";
  }
  str += "]";
  return str;
}

function mytrace (arr) {
  if (arr instanceOf Array) {
    trace (pretty (arr, 0));
  } else {
    trace (arr);
  }
}


mytrace ("## Static properties");

mytrace (Array.CASEINSENSITIVE);
mytrace (Array.DESCENDING);
mytrace (Array.UNIQUESORT);
mytrace (Array.RETURNINDEXEDARRAY);
mytrace (Array.NUMERIC);
old = Array.NUMERIC;
Array.NUMERIC = 3;
mytrace (Array.NUMERIC);
Array.NUMERIC = old;
mytrace (Array.NUMERIC);


mytrace ("## Contructor");

mytrace ("# Normal usage");

mytrace (new Array ());				// empty array
mytrace (new Array (5));			// array with 5 empty spots
mytrace (new Array (1, "b", new Object ()));	// array with 3 pre-determined elements
mytrace (Array ());
mytrace (Array (5));
mytrace (Array (1, "b", new Object ()));
mytrace ([]);
mytrace ([1, "b", new Object ()]);

mytrace ("# Special cases");

mytrace (new Array (1000));			// big number
mytrace (new Array (-4));				// negative number as argument
mytrace (new Array ("3"));			// one string preseting number as argument
mytrace (new Array (new String (3)));		// one string preseting number as argument
mytrace (new Array (new Number (3)));		// number, but not a primitive type
mytrace (new Array (3.5));			// floating point numbers
mytrace (new Array (3.4));
mytrace (new Array (3.6));
mytrace (new Array ([], ["a", "b", [1, 2]]));	// nested arrays


mytrace ("## length");

mytrace ("# Normal usage");

a = new Array (1, "b", "c", 4);
mytrace (a.length);
a = new Array (1, new Object (), "c");
mytrace (a.length);
a = new Array (1337);
mytrace (a.length);
a = new Array ();
mytrace (a.length);
mytrace (a);
a.length = 10;
mytrace (a.length);
mytrace (a);

mytrace ("# Special cases");
a = new Array ();			// making it smaller than largest value
a.length = 10;
a[9] = 1;
mytrace (a);
a.length = 5;
mytrace (a);
mytrace (a[9]);
a.length = 10;
mytrace (a);
a = new Array (1, "b", "c", 4);		// negative value
a.length = -4;
mytrace (a);
mytrace (a.length);
a = new Array ();			// setting negative variable while negative length
a.length = -4;
a[-2] = "x";
mytrace (a);
mytrace (a.length);
a = new Array (1, "b", "c", 4);		// floating point values
a.length = 3.4;
mytrace (a);
mytrace (a.length);
a = new Array (1, "b", "c", 4);		// floating point values
a.length = 3.5;
mytrace (a);
mytrace (a.length);
a = new Array (1, "b", "c", 4);		// floating point values
a.length = 3.6;
mytrace (a);
mytrace (a.length);
a = new Array (1, "b", "c", 4);		// non-numeric values
a.length = new Object ();
mytrace (a);
mytrace (a.length);
a = new Array (1, "b", "c", 4);
a.length = "har";
mytrace (a);
mytrace (a.length);
a = new Array (1, "b", "c", 4);
a.length = "3";
mytrace (a);
mytrace (a.length);
a = new Array (1, "b", "c", 4);
a.length = null;
mytrace (a);
mytrace (a.length);
a = new Array (1, "b", "c", 4);
a.length = undefined;
mytrace (a);
mytrace (a.length);


mytrace ("## toString");

mytrace ("# Normal usage");
a = new Array ("a", "b", "c");
mytrace (a.toString ());

mytrace ("# Special cases");
a = new Array ();			// empty array
mytrace (a.toString ());
a = new Array (20);			// only undefined values
mytrace (a.toString ());


mytrace ("## join");

mytrace ("# Normal usage");

a = new Array ("a", "b", "c");
mytrace (a.join ());			// default delimeter: ,
mytrace (a.join (":"));			// custom delimeter

mytrace ("# Special cases");

a = new Array ();			// empty array
mytrace (a.join ());
mytrace (a.join (":"));
a = new Array (1, "b", 3);
mytrace (a.join (31));			// non-string parameters
mytrace (a.join (new Object ()));
mytrace (a.join (null));
mytrace (a.join (undefined));
mytrace (a.join ("lal", 31));		// many parameters
mytrace (a.join (new Object (), 3, 3));
a = new Array (20);			// only undefined values
mytrace (a.join ());
mytrace (a.join (":"));


mytrace ("## Push");

mytrace ("# Normal usage");

a = new Array (1, 2, 3);
mytrace (a.push (4));
mytrace (a);
mytrace (a.push ("a"));
mytrace (a);

mytrace ("# Special cases");

a = new Array (1, 2, 3);
mytrace (a.push (4, 5));		// more than one element
mytrace (a);
mytrace (a.push ());			// pushing nothing
mytrace (a);
mytrace (a.push (["a", "b", "c"]));	// pushing another array
mytrace (a);
a = new Array ();			// pushing when weird length
a.length = "weird";
mytrace (a.push ("weirder"));
mytrace (a["weird"]);
mytrace (a);
a = new Array ();			// pushing nothing, weird length
a.length = "weird";
mytrace (a.push ());
mytrace (a["weird"]);
mytrace (a);
a = new Array ();			// pushing on empty array
mytrace (a.push (6));
mytrace (a);
a = new Array (5);			// pushing on array with only undefined values
mytrace (a.push (6));
mytrace (a);
a = new Array ();			// pushing on array with negative length
a.length = -3;
mytrace (a.push ("x"));
mytrace (a[-3]);
mytrace (a.length);


mytrace ("## Pop");

mytrace ("# Normal usage");

a = new Array (1, "b", "c", 4);
mytrace (a.pop ());
mytrace (a);
mytrace (a.pop ());
mytrace (a);

mytrace ("# Special cases");

a = new Array ();			// empty
mytrace (a.pop ());
mytrace (a);
a = new Array ();			// empty, weird length
a.length = "weird";
mytrace (a.pop ());
mytrace (a);
a = new Array (1, "b", "c", 4);		// use parameters
mytrace (a.pop (5));
mytrace (a);
mytrace (a.pop ("c", "b"));
mytrace (a);


mytrace ("## Unshift");

mytrace ("# Normal usage");

a = new Array (1, 2, 3);
mytrace (a.unshift (4));
mytrace (a);
mytrace (a.unshift ("a"));
mytrace (a);

mytrace ("# Special cases");

a = new Array (1, 2, 3);
mytrace (a.unshift (4, 5));		// more than one element
mytrace (a);
mytrace (a.unshift ());			// unshifting nothing
mytrace (a);
a1 = new Array ("a", "b", "c");		// unshifting another array
mytrace (a.unshift (a1));
mytrace (a);
a = new Array ();			// unshifting on empty array
mytrace (a.unshift (6));
mytrace (a);
a = new Array (5);			// unshifting on array with only undefined values
mytrace (a.unshift (6));
mytrace (a);


mytrace ("## Shift");

mytrace ("# Normal usage");

a = new Array (1, "b", "c", 4);
mytrace (a.shift ());
mytrace (a);
mytrace (a.shift ());
mytrace (a);

mytrace ("# Special cases");

a = new Array ();			// empty
mytrace (a.shift ());
mytrace (a);
a = new Array (1, "b", "c", 4);	// use parameters
mytrace (a.shift (5));
mytrace (a);
mytrace (a.shift ("b", "c"));
mytrace (a);


mytrace ("## Reverse");

mytrace ("# Normal usage");

a = new Array (1, "b", "c", 4);
mytrace (a.reverse ());
mytrace (a);
a = new Array (1, new Object (), "c");
mytrace (a.reverse ());
mytrace (a);

mytrace ("# Special cases");

a = new Array ();			// empty
mytrace (a.reverse ());
mytrace (a);
a = new Array ("a");			// one element
mytrace (a.reverse ());
mytrace (a);
a1 = new Array ([a, "b"]);		// array inside array
mytrace (a1.reverse ());
mytrace (a1);
a = new Array (1, "b", "c", 4);		// use parameters
mytrace (a.reverse (4));
mytrace (a);
a = new Array (1, "b", "c", 4);
mytrace (a.reverse ("b", "c"));
mytrace (a);


mytrace ("## Concat");

mytrace ("# Normal usage");

a = new Array (1, "b", "c", 4);
a1 = new Array (1, new Object (), "c");
mytrace (a.concat (a1));
mytrace (a);
mytrace (a1);
a = new Array (1, "b", "c", 4);
mytrace (a.concat (1, new Object (), "c"));
mytrace (a);

mytrace ("# Special cases");

a = new Array ();			// empty arrays
a1 = new Array ();
mytrace (a.concat (a1));
mytrace (a);
mytrace (a1);
a = new Array (1, "b", "c", 4);		// self
mytrace (a.concat (a));
mytrace (a);
a = new Array (1, ["b", "c"], 4);	// nested arrays
a1 = new Array ([1, new Object ()], "c");
mytrace (a.concat (a1));
mytrace (a);
mytrace (a1);


mytrace ("## Slice");

mytrace ("# Normal usage");

a = new Array (1, "b", "c", 4);
mytrace (a.slice (1, 3));
mytrace (a);
mytrace (a.slice (0, -2));
mytrace (a);
mytrace (a.slice (-2));
mytrace (a);

mytrace ("# Special cases");

a = new Array (1, "b", "c", 4);		// empty array
mytrace (a.slice ());
mytrace (a);
a = new Array (1, "b", "c", 4);
mytrace (a.slice ());			// without parameters
mytrace (a);
mytrace (a.slice (0, -2, 4, 3, "d"));	// with extra parameters
mytrace (a);
mytrace (a.slice (3, 20));		// with too much plus
mytrace (a);
mytrace (a.slice (20));
mytrace (a);
mytrace (a.slice (0, -5));		// with too much minus
mytrace (a);
mytrace (a.slice (-20));
mytrace (a);
mytrace (a.slice (1, 2, 3, 4, 5));	// extra parameters
mytrace (a);
mytrace (a.slice (1, 2, "hah", new Object ()));
mytrace (a);


mytrace ("## Splice");

mytrace ("# Normal usage");

a = new Array (1, "b", "c", 4);
mytrace (a.splice (1));
mytrace (a);
a = new Array (1, "b", "c", 4);
mytrace (a.splice (0, 1));
mytrace (a);
a = new Array (1, "b", "c", 4);
mytrace (a.splice (-3, 2, new Object (), 3));
mytrace (a);

mytrace ("# Special cases");

a = new Array (1, "b", "c", 4);		// no arguments
mytrace (a.splice ());
a = new Array ();			// empty array
mytrace (a.splice (0));
mytrace (a);
mytrace (a.splice (0, 0, "a", "b"));
mytrace (a);
// TODO


mytrace ("## Sort");

// not all things can be tested by tracing, because it's possible to have many valid
// results that look the same, for example for: ["a", "A"].sort (Array.CASEINSENSITIVE)

function sortall (arr, func, must, mustnot) {
  for (i = 0; i < Array.NUMERIC * 2; i++) {
    if ((must == 0 || (i & must) == must) && (mustnot == 0 || (i & mustnot) == 0)) {
      var arr_copy = arr.concat ();
      if (func != null) {
	mytrace ("sort customfunc " + i + " " + pretty (arr, 0));
	mytrace (arr_copy.sort (func, i));
      } else {
	mytrace ("sort " + i + " " + pretty (arr, 0));
	mytrace (arr_copy.sort (i));
      }
      mytrace (arr_copy);
    }
  }
}

function compare_array_length (a, b) {
  if (a.length > b.length) {
    return 1;
  } else if (a.length < b.length) {
    return -1;
  } else {
    return 0;
  }
}

mytrace ("# Normal usage");

a = new Array ("a", "B", "c", "v", 1, 9, 10);
sortall (a, null, 0, 0);

a = new Array ();
a[1] = 1;
a[3] = "a";
a[4] = 10;
a[5] = "B";
a[10] = "c";
a[12] = 9;
a[15] = "v";
sortall (a, null, 0, Array.RETURNINDEXEDARRAY);

a = new Array ("a", "B", "c", 1, 9, 10, "c");
sortall (a, null, Array.UNIQUESORT, 0);

a = new Array ("a", "B", "c", 1, 9, 10, "C");
sortall (a, null, Array.UNIQUESORT | Array.CASEINSENSITIVE, 0);
sortall (a, null, 0, Array.UNIQUESORT | Array.CASEINSENSITIVE);

a = new Array ([1,2,3], [1,2,3,4], [], [1,2], [1]);
sortall (a, compare_array_length, 0, 0);

// indexed array with undefined values
a = new Array ();
a[0] = "a";
a[2] = "b";
a[3] = undefined;
a[4] = "c";
a.length = 10;
var ret = c.sort (Array.RETURNINDEXEDARRAY);
#if __SWF_VERSION__ <= 6
mytrace (ret.slice (-3));
#else
mytrace (ret.slice (0, 3));
#endif

// array with undefined values
a = new Array ();
a[0] = "a";
a[2] = "b";
a[3] = undefined;
a[4] = "c";
a.length = 10;
mytrace (a.sort ());

// unique sort with one undefined values
a = new Array ("a", "b");
a.length = 3;
mytrace (a.sort (Array.UNIQUESORT));

a = new Array ("a", undefined, "b");
mytrace (a.sort (Array.UNIQUESORT));

a = new Array ("a", undefined, "b");
a.length = 4;
mytrace (a.sort (Array.UNIQUESORT));

// sort with non-integer and non-object argument
a = new Array ("b", "a", "c");
mytrace (a.sort ("moi"));


mytrace ("## Fake Array (Object with Array's methods)");

fake = new Object();

fake.join = Array.prototype.join;
fake.toString = Array.prototype.toString;
fake.push = Array.prototype.push;
fake.pop = Array.prototype.pop;
fake.shift = Array.prototype.shift;
fake.unshift = Array.prototype.unshift;
fake.reverse = Array.prototype.reverse;
fake.concat = Array.prototype.concat;
fake.slice = Array.prototype.slice;
fake.splice = Array.prototype.splice;
fake.sort = Array.prototype.sort;

trace (fake.push);
trace (fake.push ("a", "b", "c", "x", "z"));
trace (fake.length + ": " + fake.join (":"));
trace (fake.pop ());
trace (fake.length + ": " + fake.join (":"));

trace (fake.unshift ("y"));
trace (fake.length + ": " + fake.join (":"));
trace (fake.unshift ("z"));
trace (fake.length + ": " + fake.join (":"));
fake.length++;
trace (fake.length + ": " + fake.join (":"));
trace (fake.shift ());
trace (fake.length + ": " + fake.join (":"));
// shift on the last element
fake.length = 1;
trace (fake.shift ());
trace (fake.length + ": " + fake.join (":"));
fake.length = 5;

trace (fake.reverse ());
trace (fake.length + ": " + fake.join (":"));

trace (fake.concat ("a", fake).join (":"));
trace (fake.length + ": " + fake.join (":"));

trace (fake.slice (1, 3));
trace (fake.length + ": " + fake.join (":"));

trace (fake.splice (2, 3, "i", "j"));
trace (fake.length + ": " + fake.join (":"));

trace (fake.sort ());
trace (fake.length + ": " + fake.join (":"));

fake = new Object();
fake.sort = Array.prototype.sort;
fake[0] = 4;
fake[1] = 3;
trace (fake.sort ());
trace (fake[0] + ", " + fake[1]);


mytrace ("## Done");
loadMovie ("FSCommand:quit", "");
