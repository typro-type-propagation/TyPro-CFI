#!/usr/bin/env bash

cd /spec
. ./shrc
ulimit -s unlimited
exec runspec "$@"
