#! /bin/sh

pandoc -f markdown_github -t markdown README.md > README_doxygen.md
perl -CSDA -plE 's/\s/ /g' README_doxygen.md # replace Unicode whitespace with ASCII
sed -i -e 's/\ {.c/{.c/g' README_doxygen.md # rm space before {.cpp} for syntax highlighting
doxygen doxygen_Logger.cfg
sh branchDoc.sh
