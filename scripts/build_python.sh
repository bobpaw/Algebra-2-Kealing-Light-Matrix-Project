#!/bin/bash

for q in python/source/*
do
  echo "Building ${q}"
  python -m py_compile ${q}
done
