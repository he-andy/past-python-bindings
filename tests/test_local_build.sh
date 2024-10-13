#!/bin/bash

# if past-0.7.0 directory doesn't exist, download and extract it
if [ ! -d "past-0.7.0" ]; then
    wget https://master.dl.sourceforge.net/project/pocc/1.6/release/modules/past-0.7.0.tar.gz
    tar -xvf past-0.7.0.tar.gz
    cp ../setup.py ./past-0.7.0/
    cp ../bindings.cpp ./past-0.7.0/src/
    cp ../Makefile.am ./past-0.7.0/src/
fi

# build wheel
cd past-0.7.0
python3 setup.py bdist_wheel
cd dist
# find the wheel name
WHEEL_NAME=$(ls | grep .whl)
python3 -m pip install --force-reinstall ./$WHEEL_NAME
python3 -c "import past"