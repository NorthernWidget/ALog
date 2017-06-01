#! /bin/sh

git checkout master
doxygen doxygen_Logger.cfg

git checkout --orphan gh-pages # first time only
git checkout gh-pages doc/html # all other times

git add doc/html # even needed at this point?
git commit -m "Documentation update (doxygen): gh-pages"
git subtree push --prefix doc/html origin gh-pages

git checkout master
