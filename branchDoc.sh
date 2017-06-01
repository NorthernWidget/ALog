# Run once -- unless we want to do delete and remake the branch each time
#git checkout --orphan gh-pages
#git add doc/html && git commit -m "Initial doc subtree commit -- gh-pages"
git subtree push --prefix doc/html origin gh-pages
git checkout master -f
