#!/bin/sh

PROJECT_NAME=$1
export PROJECT_NAME
PROJECT_VERSION=$2
export PROJECT_VERSION
cd "${MESON_SOURCE_ROOT}"/doc
envsubst < Doxyfile.in >Doxyfile
doxygen
