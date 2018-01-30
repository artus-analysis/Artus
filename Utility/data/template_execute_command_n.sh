#!/bin/bash

COMMANDS=(
	$commands
)

echo "Execute command $1:"
echo ${COMMANDS[$1]}
eval ${COMMANDS[$1]}

