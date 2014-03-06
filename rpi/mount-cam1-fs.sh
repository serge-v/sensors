#!/bin/bash

[[ ! -d ${HOME}/cam1 ]] && mkdir ${HOME}/cam1
sshfs -p 443 pi@cam1:/ ${HOME}/cam1 -o allow_other,auto_cache,reconnect,workaround=rename
