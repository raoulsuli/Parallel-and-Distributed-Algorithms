#!/bin/bash

echo "Start time: $(date)"

FOLDERS=../tests/*
HW_PATH=com/apd/tema2/Main
ROOT=./src
ERR=./err
OUT=./out

echo -e "Show CPU info (lscpu)\n\n"
lscpu

echo -e "\n\nShow memory info (free -m)\n\n"
free -m
echo -e "\n\n"

echo -e "Unzip tests and student solution\n\n"

unzip artifact.zip
unzip archive.zip

echo -e "\n\nRunning the checker\n\n"

if [ -d "$ROOT" ]; then
	rm -rf out err src/bin
	mkdir out err
	cd src

	javac -g $HW_PATH.java -d bin

	for d in $FOLDERS
	do	
		for f in $d/*
		do
    			echo "Processing $f file..."
    		
			fullpath=`echo "${f%.*}"`
			filename="${fullpath##*/}"

			timeout 120 java -cp bin/ $HW_PATH $f > ../out/$filename.out
			
			if [ $? != 0 ]
			then
				echo "Timer exceeded"
			fi
		done
	done

	cd ..
	timeout 120 java -jar ./Tema2Checker_J8.jar

	for f in $ERR/*
        do
               	if [[ -s $f ]]
		then
			echo "Contents of the err file $f"
			cat $f

			fullpath=`echo "${f%.*}"`
                        filename="${fullpath##*/}"

			echo "Contents of the out file $filename.out"
			head --lines=10 $OUT/$filename.out
		fi
	done

	echo "complex_1"
	cat "out/complex_maintenance_1.out"
	echo -e "\n\n"

	echo "complex_2"
	cat "out/complex_maintenance_2.out"
	echo -e "\n\n"

	echo "complex_3"
	cat "out/complex_maintenance_3.out"
	echo -e "\n\n"

	echo "complex_4"
	cat "out/complex_maintenance_4.out"
else
	echo "src not found"
fi

date
