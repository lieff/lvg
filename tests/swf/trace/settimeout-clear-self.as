// makeswf -v 7 -r 1 -o settimeout-clear-self-7.swf settimeout-clear-self.as

trace ("Test clearing timeout in it's own callback function");

function clear_self () {
  trace ("clearing timeout");
  clearTimeout (timeout);
  loadMovie ("FSCommand:quit", "");
}

timeout = setTimeout (clear_self, 50);
