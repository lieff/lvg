echo '#include <math.h>' >> allh.h
echo '#define NANOSVG_ALL_COLOR_KEYWORDS' >> allh.h
cat include/*.h nanovg/nanosvg.h nanovg/nanovg.h lvg_header.h >> allh.h
dd if=/dev/zero bs=1 count=1 >> allh.h
xxd -i allh.h >all.h
rm allh.h