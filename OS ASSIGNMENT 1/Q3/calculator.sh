#!/bin/bash

# Creating the "Result" directory
mkdir -p Result

# Reading the pre-created text file input.txt line by line
while IFS= read -r content || [[ -n "$content" ]]; do
    arr=($content)

    num1="${arr[0]}"
    num2="${arr[1]}"
    operation="${arr[2]}"

    if [ "$operation" = "xor" ]; then
        result=$((num1 ^ num2))
        echo "Result of xor $num1 $num2: $result" >> Result/output.txt

    elif [ "$operation" = "product" ]; then
        result=$((num1 * num2))
        echo "Result of product $num1 $num2: $result" >> Result/output.txt

    elif [ "$operation" = "compare" ]; then
        if (( num1 == num2 )); then
            result="The numbers are equal"
            echo "$result" >> Result/output.txt
        elif (( num1 > num2 )); then
            result="$num1 is greater than $num2"
            echo "$result" >> Result/output.txt
        elif (( num2 > num1 )); then
            result="$num2 is greater than $num1"
            echo "$result" >> Result/output.txt
        fi
    fi
done < "input.txt"
