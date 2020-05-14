cd build

make

if [ $? -eq 0 ];
then
  cd ..
  cd Release
  ./longshot
fi

cd ..
