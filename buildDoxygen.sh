#! /bin/sh

pandoc -f markdown_github -t markdown README.md > README_doxygen.md
doxygen doxygen_Logger.cfg
