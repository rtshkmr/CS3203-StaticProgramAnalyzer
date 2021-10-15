echo "[NOTE] Testing SP Parsing"; 
RESULT="0";

for i in $(ls basic/*.txt);
do echo "[NOTE] Running Basic Test $i"; 
	./../../autotester $i blank.txt blank.xml > /dev/null;
	if [ "$?" != 0 ] 
	then
	    RESULT="1";
	else
	fi
done

for i in $(ls adv/*.txt);
do echo "[NOTE] Running Advanced Test $i"; 
	./../../autotester $i blank.txt blank.xml > /dev/null;
	if [ "$?" != 0 ] 
	then
	    RESULT="1";
	else
	fi
done

for i in $(ls throw/*.txt);
do echo "[NOTE] Running Exception Test $i"; 
	./../../autotester $i blank.txt blank.xml &> /dev/null;
	if [ "$?" == 0 ] 
	then
	    RESULT="1";
	else
	fi
done

exit $RESULT
