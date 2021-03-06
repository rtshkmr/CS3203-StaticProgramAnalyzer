# This shell executor will run all the test files in this folder. 
# How it works? 
# All test should have a "test_source_x" and "test_queries_x" where x is the same number, and in increasing order
# Do ensure the test case number are in increasing order. 
# At the end of the run, it will zip all the .xml file
# Written by: WeiJie (14/09/2021) 

TOTAL=$(ls *.txt | wc -l);
echo "[NOTE] Running a grand total of $TOTAL tests..."; 
RESULT="0";

for i in $(ls *.txt);
do echo "[NOTE] Running Test $i"; 

	./../../../autotester $i ../blank.txt output_$i.xml &> /dev/null;

	if [ "$?" != 1 ]
	then
	    RESULT="1";
		echo "[ERROR] $i did not throw any error; or has unhandled exceptions";
	fi
done

exit $RESULT
