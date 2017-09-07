// vivi-compile -v 7 -s 200x150 -r 15 -o try-flags.swf try-flags.as

try {
  asm {
    try has_catch has_finally "e" catch_start_1 finally_start_1 end_1
    push "Try"
    throw
catch_start_1:
    push "Catch"
    throw
finally_start_1:
    push "Finally"
    throw
end_1:
  }
} catch (e) {
  trace (e);
}

try {
  asm {
    try has_catch "e" catch_start_2 finally_start_2 end_2
    push "Try"
    throw
catch_start_2:
    push "Catch"
    throw
finally_start_2:
    push "Finally"
    throw
end_2:
  }
} catch (e) {
  trace (e);
}

try {
  asm {
    try has_finally "e" catch_start_3 finally_start_3 end_3
    push "Try"
    throw
catch_start_3:
    push "Catch"
    throw
finally_start_3:
    push "Finally"
    throw
end_3:
  }
} catch (e) {
  trace (e);
}

try {
  asm {
    try "e" catch_start_4 finally_start_4 end_4
    push "Try"
    throw
catch_start_4:
    push "Catch"
    throw
finally_start_4:
    push "Finally"
    throw
end_4:
  }
} catch (e) {
  trace (e);
}

getURL ("fscommand:quit", "");
