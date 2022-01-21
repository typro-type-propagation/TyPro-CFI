#!/bin/sh

export TG_INSTRUMENT_COLLECTCALLTARGETS=1
export TG_GRAPH_OUTPUT=auto
#export TG_ICFI_OUTPUT=auto
export TG_IFCC_OUTPUT=auto
export TG_CFI_OUTPUT=auto
export TG_ENFORCE=0

exec "$@"
