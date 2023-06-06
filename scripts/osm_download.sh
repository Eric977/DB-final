#!/bin/bash
mkdir ../workloads
curl -O --location https://github.com/openstreetmap/osmosis/releases/download/0.48.3/osmosis-0.48.3.tgz
tar xfvz osmosis-0.48.3.tgz
rm osmosis-0.48.3.tgz
mv osmosis-0.48.3.tgz OSM
