#! /bin/sh

git checkout master
doxygen doxygen_Logger.cfg

cp -r doc/figures doc/html/

git checkout --orphan gh-pages # first time only
git checkout gh-pages doc/html # all other times
#git checkout gh-pages doc/figures # all other times

git checkout gh-pages

git add doc/html # even needed at this point?
git commit -m "Documentation update (doxygen): gh-pages"
git subtree push --prefix doc/html origin gh-pages

#git add -f doc/figures
#git commit -m "source figures"
#git push origin master

git checkout master
