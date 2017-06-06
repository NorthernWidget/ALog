#! /bin/sh

# Set up the Markdown
pandoc -f markdown_github -t markdown ../README.md > README_doxygen.md
perl -CSDA -plE 's/\s/ /g' README_doxygen.md # replace Unicode whitespace with ASCII
sed -i -e 's/\ {.c/{.c/g' README_doxygen.md # rm space before {.cpp} for syntax highlighting

# Create doxygen files: HTML and PDF
doxygen doxygen_Logger.cfg
cd latex
./Make.sh

# Move PDF into HTML folder so it appears in branch but doesn't have
# tracked history (would be too much overhead)
cp refman.pdf ../html/refman.pdf

