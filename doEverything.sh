#!/bin/bash

set -e
make diskTest
make writeTest
make readTest

echo $'Running Create Disk Test:'
apps/createDiskTest

echo $'\n\nRunning write Files test:'
apps/writeSomeFilesTest

echo $'\n\nRunning read Files test:'
apps/readFilesTest
