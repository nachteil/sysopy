#!/bin/bash
for i in {1000..1500000..75000}
  do
     ./$1/main $i $2
 done

 echo "Done"