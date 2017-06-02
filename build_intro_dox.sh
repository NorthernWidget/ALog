#! /bin/sh

#python autogenerate_help.py
#cat doc/construct_html/top.dox doc/construct_html/new_users_guide.html > intro.dox
#cat doc/construct_html/top.dox README.md > intro.dox
echo "/*! \mainpage ALog Guide" > intro.dox
echo "" >> intro.dox
pandoc -f markdown_github -t markdown README.md >> intro.dox
#grip README.md --export README.html
#cat README.html >> intro.dox
echo "" >> intro.dox
echo " */" >> intro.dox

doxygen doxygen_Logger.cfg
