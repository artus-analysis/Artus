#!/bin/bash
mkdir -p `dirname $2` && hadd $@ && touch success
