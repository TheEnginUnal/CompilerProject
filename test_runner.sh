#!/bin/bash

# Define colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Building B-Minor Scanner ===${NC}"
make

echo -e "\n${BLUE}=== Running Automated Tests ===${NC}"

# Check if tests directory exists
if [ ! -d "tests" ]; then
  echo "Creating 'tests' directory..."
  mkdir tests
  echo "Please put your .bminor test files inside the 'tests' directory!"
  exit 1
fi

# Loop through all .bminor files in the tests folder
for test_file in tests/*.bminor; do
    # Skip if no files are found
    [ -e "$test_file" ] || continue 
    
    echo -e "${GREEN}--> Testing: $test_file${NC}"
    ./scanner "$test_file"
    echo "------------------------------------------------"
done

echo "All tests finished!"