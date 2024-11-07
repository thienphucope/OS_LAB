#!/bin/bash

ans=0
history=()
max_history=5

# Function to perform calculation
calculate() {
    local number1=$1
    local operator=$2
    local number2=$3

    # Error handling for division or modulus by zero
    if [[ "$operator" == "/" || "$operator" == "%" ]] && [[ "$number2" -eq 0 ]]; then
        echo "MATH ERROR"
        return 1
    fi

    case $operator in
        +) ans=$((number1 + number2)) ;;
        -) ans=$((number1 - number2)) ;;
        '*') ans=$((number1 * number2)) ;;
        /) ans=$(printf "%.2f" "$(echo "scale=2; $number1 / $number2" | bc)") ;;
        %) ans=$((number1 % number2)) ;;
        *) echo "SYNTAX ERROR"; return 1 ;;
    esac
    return 0
}

while true; do
    echo -n ">> "
    read input

    case $input in
        EXIT) break ;;   # Exit the calculator
        HIST)            # Display history
            for calc in "${history[@]}"; do
                echo "$calc"
            done
            continue
            ;;
    esac

    # Parse input with spaces as delimiters
    IFS=' ' read -r number1 operator number2 <<< "$input"
    
    # Substitute ANS keyword with the last result
    [[ "$number1" == "ANS" ]] && number1=$ans
    [[ "$number2" == "ANS" ]] && number2=$ans

    # Call calculate function and handle success
    if calculate "$number1" "$operator" "$number2"; then
        history+=("$input = $ans")
        # Maintain a maximum history length
        (( ${#history[@]} > max_history )) && history=("${history[@]:1}")
        echo $ans
    fi
done
