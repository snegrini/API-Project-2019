mkdir output
for file in tests/others/i/*
do
  filename="${file##*/}"
  echo $file
  cat $file | ./main > output/$filename.out
done 
