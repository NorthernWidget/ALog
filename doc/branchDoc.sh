#! /bin/sh

git checkout master

git push origin --delete gh-pages
#read
git branch -D gh-pages

doxygen doxygen_Logger.cfg

rm -rf doctmp
cp -r doc doctmp # Store in temporary folder

git checkout --orphan gh-pages # first time only
#git checkout gh-pages

mkdir doc
#cp -r doctmp/figures doc/
cp -r doctmp/html doc/

git reset # maybe not needed?

git add -f doc/html
git commit -m "Documentation update (doxygen): gh-pages"
git subtree push --prefix doc/html origin gh-pages
#read

#git add -f doc/figures
#git commit -m "source figures"
#git push origin master

git clean -df
git clean -df
git checkout master
