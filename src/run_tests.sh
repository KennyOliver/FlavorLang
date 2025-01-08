#!/bin/bash

# Directory containing test files
TEST_DIR="tests"

# Loop through all .flv files in the tests directory
for test_file in "$TEST_DIR"/*.flv; do
    echo "Running test: $test_file"
    ./flavor "$test_file"

    # Optionally check the exit code of each test
    if [ $? -ne 0 ]; then
        echo "Test failed: $test_file"
        exit 1 # Exit the script on failure
    fi
done

echo "All tests completed successfully!"
