#!/bin/bash

VERSION="v1.0.10"
echo "Current version is $VERSION"
git tag -d $VERSION
git push origin --delete $VERSION
git tag -a $VERSION -m "build version $VERSION"
git push origin $VERSION
