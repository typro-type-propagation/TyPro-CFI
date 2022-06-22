#!/bin/sh

export TG_ENFORCE=1
export TG_DYNLIB_SUPPORT=1
export TG_GRAPH_OUTPUT=graph
#export TG_ICFI_OUTPUT=icfi_icall.json
#export TG_IFCC_OUTPUT=ifcc.json
export TG_CFI_OUTPUT=tgcfi.json

exec "$@"
