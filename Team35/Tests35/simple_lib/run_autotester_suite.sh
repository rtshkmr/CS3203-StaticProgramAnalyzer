# This shell executor will run all the test files in this folder. 
# How it works? 
# All test should have a "test_source_x" and "test_queries_x" where x is the same number, and in increasing order
# Do ensure the test case number are in increasing order. 
# At the end of the run, it will zip all the .xml file
# Written by: WeiJie (14/09/2021) 


echo "[NOTE] Running parser"; 
RESULT="0";

for i in $(ls basic/basic_*.txt);
do echo "[NOTE] Running Test $i"; 
	./../autotester $i.txt blank.txt blank.xml > res_$i.txt;
	if [ "$?" != 0 ] 
	then
	    RESULT="1";
		echo "[NOTE] $i failed."; 
	else
		echo "[NOTE] $i passed."; 
	fi
done

exit $RESULT
