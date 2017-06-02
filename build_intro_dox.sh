#! /bin/sh

python autogenerate_help.py
cat doc/construct_html/top.dox doc/construct_html/new_users_guide.html > intro.dox

