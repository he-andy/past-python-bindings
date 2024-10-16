#!/bin/bash

# if past-0.7.0 directory doesn't exist, download and extract it
if [ ! -d "past-0.7.2" ]; then
    wget https://master.dl.sourceforge.net/project/pocc/1.6/testing/modules/past-0.7.2.tar.gz
    tar -xvf past-0.7.2.tar.gz
    cp ../setup.py ./past-0.7.2/
    cp ../bindings.cpp ./past-0.7.2/src/
    cp ../Makefile.am ./past-0.7.2/src/
    # Replace 'restrict' with '__restrict__' and overwrite the file
    sed -i 's/\<restrict\>/__restrict__/g' ./past-0.7.2/past/include/past/abstract_interpretation_memory.h
fi

# build wheel
cd past-0.7.2
python3 setup.py bdist_wheel
cd dist
# find the wheel name
WHEEL_NAME=$(ls | grep .whl)
python3 -m pip install --force-reinstall ./$WHEEL_NAME
python3 -c "import past"