// makeswf -v 7 -r 1 -o test-7.swf test.as

trace ("Start");

_root.onLoad = function () {
  trace ("onLoad");
};

function quit () {
  loadMovie ("FSCommand:quit", "");
}

setInterval (quit, 1000);
