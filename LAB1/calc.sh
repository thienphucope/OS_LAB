#!/bin/bash

ans=0
history=()
while true; do
	echo -n ">> "
	read input
	if [[ "$input" == "EXIT" ]]; then
		break
	fi
	if [[ "$input" == "HIST" ]]; then
		for calc in "${history[@]}"; do
			echo "$calc"
		done
		continue
	fi
	IFS=' ' read -r number1 operator number2 <<< "$input"
	if [[ "$number1" == "ANS" ]]; then
		number1=$ans
	fi
	if [[ "$number2" == "ANS" ]]; then
		number2=$ans
	fi
	case $operator in
	+)
            ans=$(echo "$number1 + $number2" | bc)
            ;;
        -)
            ans=$(echo "$number1 - $number2" | bc)
            ;;
        \*)
            ans=$(echo "$number1 * $number2" | bc)
            ;;
        /)
            if [ "$number2" -ne 0 ]; then
                ans=$(echo "scale=4; $number1 / $number2" | bc)
		ans=$(echo "scale=2; ($ans + 0.005)" | bc)
		ans=$(printf "%.2f" "$ans")
            else
                echo "MATH ERROR"
                continue
            fi
            ;;
	%)
	   if [ "$number2" -ne 0 ]; then
		ans=$(echo "$number1 % $number2" | bc)
	   else
		echo "MATH ERROR"
		continue
	   fi
	   ;;
        *)
            echo "SYNTAX ERROR"
            continue
            ;;
    esac
    history+=("$input = $ans")
    if ((${#history[@]} > 5)); then
    	history=("${history[@]:1}") 
    fi

    echo $ans
done
