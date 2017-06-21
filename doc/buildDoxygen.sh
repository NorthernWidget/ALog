#! /bin/sh

# Build documentation
sh buildDoxygenLocally.sh
# Upload documentation to branch
sh branchDoc.sh

