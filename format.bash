#!/bin/bash

for (( n=12; n>=10; n-- ))
do
	CLANGFORMAT=clang-format-$n
	if command -v $CLANGFORMAT &> /dev/null; then
		echo "$CLANGFORMAT was found."
		for f in $(find $@ -name '*.h' -or -name '*.c'); do
			echo "Formatting $f"
			$CLANGFORMAT -i $f
   		done
		echo "Done."
		exit
	fi
done

echo "No suitable clang-format (>=10) was found."