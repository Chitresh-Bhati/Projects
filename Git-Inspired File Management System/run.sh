#!/bin/bash

#compile the code.cpp file into an executable called "program"
g++ code.cpp -o program

# if successful, run it!
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running program..."
    ./program
else
    echo "Compilation failed."
fi
