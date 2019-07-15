for file in /home/snegrini/Downloads/test/o/*
do
  filename="${file##*/}"
  echo $filename.out
  #echo $file
  diff $file /home/snegrini/Projects/API_Project-2019/output/$filename.out
done 
