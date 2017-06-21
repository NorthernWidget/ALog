#! /bin/sh

# Build documentation
sh buildDoxygenLocally.sh
# cd to root for git commands
cd ..
# Upload documentation to branch
sh doc/branchDoc.sh

