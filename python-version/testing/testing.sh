#!/bin/sh

# Exit immediately if a command exits with a non-zero status.
set -e
# First step is running unit tests defined in the python test file.
python3 -m pytest -v

# Integration testing
test_password="Qwerty123"
expected_storage_path="/home/$USER/.pmp/81d1d68e2230674aa504d7fa2efaa8b893a128d3cc477385f26fa106380fb004.pst"
echo $'-i\n'$test_password | python3 pm.py
if ! [ -f $expected_storage_path ]
then
  echo "File $expected_storage_path doesn't exist"
  # 1-255 exit code means failure.
  exit 1
fi
expected_file_content="4f42378d53b50a1c73d9a57f1550540ea0c3c0d5ceaf671394f2d200d4f95eafdcd6ff816c08e4c4364dc3ccc8bcf5a29cb003f05ff3089df5c852dc30d8fd8c"
if [ $expected_file_content != $(cat $expected_storage_path) ]
then
  echo "Expected $expected_file_content but got $(cat $expected_storage_path)"
  exit 1
fi

# User friendly
# ID: 10
output=$(echo "abracadabra" | python3 pm.py)
want_substr="Unexpected command:"
if [[ "$output" != *"$want_substr"* ]];
then
  echo "Expected substring $want_substr to be in $output, but it wasn't"
  exit 1
fi

# 0 exit code indicates all tests have passed successfully
exit 0
