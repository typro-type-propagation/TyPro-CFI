#!/bin/sh

export TG_GRAPH_OUTPUT=auto
export TG_CFI_OUTPUT=auto

export TG_ENFORCE=1
export TG_DYNLIB_SUPPORT=1
export TG_ENFORCE_DEBUG=0
# export TG_ENFORCE_DISPATCHER_LIMIT=1000000

exec "$@"
