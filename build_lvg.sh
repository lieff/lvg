. ~/Public/emsdk_portable/emsdk_set_env.sh

if [ -z "$1" ]
  then
    echo "No argument supplied"
    exit 1
fi

if [ -f $1 ];
then
   echo "Processing $1"
else
   echo "File $1 does not exist."
   exit 1
fi
unzip $1 main.c
mv main.c main.pc
cat lvg_header.h main.pc >> main.c
emcc -Os main.c -o main.js -s EXPORTED_FUNCTIONS="['onFrame', 'onInit']" -s SIDE_MODULE=1
rm main.c
rm main.pc
zip -9 $1 main.js
rm main.js
