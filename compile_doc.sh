#! /bin/sh

doxygen doxygen_Logger.cfg
moxygen doc/xml -o README_components/api.md

cat README_components/base.md README_components/api.md > README.md
