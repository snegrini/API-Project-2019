for file in tests/others/o/*
do
  filename="${file##*/}"
  echo $filename.out
  diff $file output/$filename.out
done 
