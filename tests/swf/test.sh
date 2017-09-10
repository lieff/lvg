APP=../../build/lvg_test
if [ ! -f "$APP" ]; then
    mkdir ../../build
    pushd ../../build
    meson -Dbuildtype=debug -Db_sanitize=address ..
    ninja
    popd
fi

num_fail=0
num_pass=0
num_error=0
for i in trace/*.swf; do
tput ed
echo -ne "(${num_pass} passed, ${num_fail} failed) testing $i\r"
$($APP $i >$i.failed 2>&1)
retval=$?
if [ ! $retval -eq 0 ]; then
    tput ed
    echo $i exited with code=$retval
    num_error=$((num_error + 1))
    if [ $retval -eq 1 ]; then
        cat $i.failed
    fi
fi
cmp -s $i.failed $i.trace > /dev/null
if [ $? -eq 1 ]; then
    num_fail=$((num_fail + 1))
else
    rm $i.failed
    num_pass=$((num_pass + 1))
fi
done

echo -ne "\nresults: passed tests: ${num_pass}, failed tests: ${num_fail}, exited with error: ${num_error}\n\n"
