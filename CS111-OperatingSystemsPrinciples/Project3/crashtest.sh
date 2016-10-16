echo This is our working file > ./test/file.txt
echo This is our working file > ./test/file.txt

./crashtest 2 2

yes | head -n 10000 > ./test/yes.txt

yes | head -n 10000 >> ./test/yes.txt

echo Now that we are out of writes, we will try doing some file operations

echo Attempting to create new.txt in ./test
echo This is our new file > ./test/new.txt

echo Attmepting to delete pokercats.gif and world.txt

rm ./test/pokercats.gif
rm ./test/world.txt

cd test

echo The current content of ./test is:
ls -l

echo Neither of the deletes or the new file we tried to create appear
echo We are resetting the disk to write properly

cd ..
./crashtest -1 -1
cd test
echo and writing another file, world.txt, identical to file.txt
cat file.txt > world.txt
echo and running a diff command
diff -u file.txt world.txt
