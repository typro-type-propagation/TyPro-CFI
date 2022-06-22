#!/bin/sh

export TG_ICFI_OUTPUT=auto
export TG_IFCC_OUTPUT=auto
export TG_ENFORCE=0
export TG_ENABLED=0
export TG_CLANG_EXPORT_TYPES=1

exec "$@"
