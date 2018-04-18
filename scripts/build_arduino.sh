#!/bin/bash

for r in arduino/refs/*; do
  refs="${refs} --lib=${r}"
done

for s in arduino/source/*; do
  sources="${sources} --lib=${s}"
done

for f in arduino/main/*; do
  echo "Building ${f}"
  platformio ci ${refs} ${sources} --board=uno ${f}
done

for q in main/Final_Plain_Rain/*; do
  platformio ci ${refs} --board=uno ${q}
done
