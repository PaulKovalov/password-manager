#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status.
set -e

if [[ ! -f testing/testing.sh ]]; then
    echo 'You have to run this script from "python-version" folder'
    exit 1
fi

docker-compose --file testing/docker-compose.yml up --build --renew-anon-volumes --exit-code-from pm_tests
