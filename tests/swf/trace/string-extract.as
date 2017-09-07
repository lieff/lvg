// makeswf -v 7 -s 200x150 -r 1 -o string-extract.swf string-extract.as

#include "values.as"

trace ("Test actions and functions to extract strings");

function subst (val, name, start, len) {
  asm {
    push "y", "val"
    getvariable
    push "start"
    getvariable
    push "len"
    getvariable
    substring
    setvariable
  };
  trace ("substring (" + name + ", " + start + ", " + len + ") = " + y);
  asm {
    push "z", "val"
    getvariable
    push "start"
    getvariable
    push "len"
    getvariable
    mbsubstring
    setvariable
  };
  trace ("mbsubstring (" + name + ", " + start + ", " + len + ") = " + z);
  trace (name + ".substr (" + start + ", " + len + ") = " + name.substr (start, len));
  trace (name + ".substring (" + start + ", " + len + ") = " + name.substring (start, len));
};

values.length /= 2;

extended = [ 
  undefined, 
  null, 
  -5, -2, -1, 0, 1, 0.5, 2, 5, 10, Infinity, NaN,
  "", "1", "äöü",
  this, new Object (), Function ];

for (i = 0; i < values.length; i++) {
  subst (values[i], names[i]);
  subst (values[i], names[i], undefined, 2);
  for (start = 0; start < extended.length; start ++) {
    for (len = 0; len < extended.length; len ++) {
      subst (values[i], names[i], extended[start], extended[len]);
    }
  }
}

loadMovie ("FSCommand:quit", "");
