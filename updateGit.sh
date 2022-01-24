#! /usr/bin/bash

# update github repo

pushd .

git add .
git rm --cached ./bin/data/arc/* -r
git rm --cached ./bin/data* -r
git commit 
git push
popd 
