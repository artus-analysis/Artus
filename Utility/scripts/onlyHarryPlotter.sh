#!/bin/bash
git config core.sparsecheckout true
rm .git/info/sparse-checkout
echo /HarryPlotter/ >> .git/info/sparse-checkout
echo /Utility/python/ >> .git/info/sparse-checkout
echo /Utility/scripts/ >> .git/info/sparse-checkout
git checkout -f master
