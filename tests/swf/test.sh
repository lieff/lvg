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
for i in trace/*.swf; do
$APP $i >$i.failed
cmp -s $i.failed $i.trace > /dev/null
if [ $? -eq 1 ]; then
    echo test $i failed
    num_fail=$((num_fail + 1))
else
    echo test $i passed
    rm $i.failed
    num_pass=$((num_pass + 1))
fi
done

echo passed tests: ${num_pass}
echo failed tests: ${num_fail}
