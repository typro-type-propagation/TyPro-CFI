#!/bin/bash

export TG_INSTRUMENT_COLLECTCALLTARGETS=0
export TG_ENFORCE=0
export TG_ENABLED=0

exec "$@"
