#! /bin/sh

# cd to root
cd ..
# Build documentation
sh doc/buildDoxygenLocally.sh
# Upload documentation to branch
sh doc/branchDoc.sh

