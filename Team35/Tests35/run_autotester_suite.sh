# This shell executor will run all the test files in this folder. 
# How it works? 
# All test should have a "test_source_x" and "test_queries_x" where x is the same number, and in increasing order
# Do ensure the test case number are in increasing order. 
# At the end of the run, it will zip all the .xml file
# Written by: WeiJie (14/09/2021) 


END=$(ls test_source_*.txt | wc -l);
echo "[NOTE] Running a grand total of $END tests..."; 
RESULT="0";

for i in $(seq 1 $END);
do echo "[NOTE] Running Test $i"; 
	./../autotester test_source_$i.txt test_queries_$i.txt output_$i.xml > log_$i.txt;
	if [ "$?" != 0 ] 
	then
	    RESULT="1";
	else
		head -1 log_$i.txt;
		FILENAME=output_$i.xml;
		QUERIES=$(grep -o '</query>' $FILENAME | wc -l);
		PASSES=$(grep -o '<passed/>' $FILENAME | wc -l);
		FAILS=$(grep -o '</failed>' $FILENAME | wc -l);
		echo "[NOTE] #Queries = $QUERIES; #Passes = $PASSES; #Fails = $FAILS" 
		mv Team35_SPA.log Team35_SPA_$i.log # to rename Logger file for each run.
	
		if [ "$QUERIES" != "$PASSES" ]
		then
			RESULT="2";
		fi
	fi
done

exit $RESULT
