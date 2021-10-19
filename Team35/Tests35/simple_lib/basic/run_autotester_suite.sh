# This shell executor will run all the test files in this folder. 
# How it works? 
# Any files within this directory will be treated as the source file and will be matched with the identical-naming file in queries/
# If the query file is not found, it will be executed as a blank file. 
# At the end of the run, it will zip all the log and xml file. 
# Written by: WeiJie (14/09/2021); 
# Last Updateed: 19/10/2021

#Perform cleanup
rm output_*.xml; 
rm log_*.txt;
rm Team35_SPA*.log;

# Colors
end="\033[0m"
red="\033[0;31m"
green="\033[0;32m"
yellow="\033[0;33m"
purple="\033[0;35m"
lightblue="\033[0;36m"

TOTAL=$(ls *.txt | wc -l);
echo -e "${purple}Running a grand total of $TOTAL tests...${end}"; 
RESULT="0";

for i in $(ls *.txt);
do echo -e "${lightblue}Running Test $i{$end}"; 

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
		
		mv Team35_SPA.log Team35_SPA_$i.log # to rename Logger file for each run.
		
		if [ "$QUERIES" == "0" ]
		then
			echo -e "${yellow}#Queries = $QUERIES; #Passes = $PASSES; #Fails = $FAILS${end}";
		else
			if [ "$QUERIES" != "$PASSES" ]
			then
				RESULT="2";
				echo -e "${red}#Queries = $QUERIES; #Passes = $PASSES; #Fails = $FAILS${end}";
			else
				echo -e "${green}#Queries = $QUERIES; #Passes = $PASSES; #Fails = $FAILS${end}";
		fi

	fi
done

exit $RESULT
