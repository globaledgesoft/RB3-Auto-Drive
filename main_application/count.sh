#!/bin/bash

[ $# -lt 1 ] && echo "Error! Invalid input!" && exit 1

FILE="$1"
shift

if [ $# -eq 0 ] ; then

	mv ${FILE} _${FILE}
	sort -u _${FILE} > ${FILE}
	rm -f _${FILE}

else

	for f in $@
	do
		echo $f >> ${FILE}
	done

fi
