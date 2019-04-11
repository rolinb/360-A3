#!/bin/bash

set -e
make diskTest
make writeTest

echo $'Running Create Disk Test:'
apps/createDiskTest

echo $'\n\nRunning write Files test:'
apps/writeSomeFilesTest
