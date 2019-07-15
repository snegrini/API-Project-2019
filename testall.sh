for file in /home/snegrini/Downloads/test/i/*
do
  filename="${file##*/}"
  echo $file
  cat $file | ./main > /home/snegrini/Projects/API_Project-2019/output/$filename.out
done 
