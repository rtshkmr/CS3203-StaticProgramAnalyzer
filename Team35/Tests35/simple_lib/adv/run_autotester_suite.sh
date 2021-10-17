# This shell executor will run all the test files in this folder. 
# How it works? 
# All test should have a "test_source_x" and "test_queries_x" where x is the same number, and in increasing order
# Do ensure the test case number are in increasing order. 
# At the end of the run, it will zip all the .xml file
# Written by: WeiJie (14/09/2021) 

#Perform cleanup
rm output_*.xml; 
rm log_*.txt;
rm Team35_SPA*.log;

TOTAL=$(ls *.txt | wc -l);
echo "[NOTE] Running a grand total of $TOTAL tests..."; 
RESULT="0";

for i in $(ls *.txt);
do echo "[NOTE] Running Test $i"; 

	if [ -f "queries/$i" ]
	then
		./../../../autotester $i queries/$i output_$i.xml > log_$i.txt;
	else
		./../../../autotester $i ../blank.txt output_$i.xml > log_$i.txt;
	fi

	if [ "$?" != 0 ]
	then
	    RESULT="1";
		echo "[ERROR] $i has exception errors."; 
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
			echo "[ERROR] $i has failing cases."; 
		fi
	fi
done

exit $RESULT
