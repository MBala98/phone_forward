#!/bin/bash

	if  (($# < 3))
		then echo "Za mała liczba argumentów."
	else
		temp_file=$(mktemp)	
		temp_file2=$(mktemp)
		cat $2 > ${temp_file}
		if grep -q -e 'NEW' -e 'DEL' -e '?' ${temp_file}
			then echo "Bledny plik wejsciowy"
		else
			sed -i '1i NEW a' ${temp_file}
			echo ?$3 >> ${temp_file}
			<${temp_file} ./$1 > ${temp_file2}
			sed -i -e "/$3/d" $temp_file2
			cat ${temp_file2}
			rm ${temp_file}
			rm ${temp_file2}
		fi
	fi
