#!/bin/bash
rsync -a --progress --delete -e ssh --filter "- Makefile" ./src_de_base/ ikhaloo@pcserveur.ensimag.fr:/user/8/ikhaloo/Documents/PS/src_de_base/
