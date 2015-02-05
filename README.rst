=========
llvm-pred
=========

A set of Pass and functions to help analysis program's performance model.

build
------

if gcc doesn't support c++11 ,can use clang

.. code:: bash

    $ export CC=clang
    $ export CXX=clang++
    $ mkdir build;cd build
    $ cmake .. -DLLVM_RECOMMAND_VERSION="3.4"
    $ make

use ``LLVM_RECOMMAND_VERSION`` to change llvm version directly

script
-------

drawline.py : used to draw lines from value profiling
dirdiff.sh  : compare two dir's llvmprof.out file and report whether they are
same
gettime.sh  : Used to build test files. Put this file in the build directory

performance analysis based dynamic information
-----------------------------------------------

1. insert edge profiling into bitcode.
2. compile program and run.
3. merge multi process's log into one: ``llvm-prof -merge -algo=avg merged.out input.out*``
4. convert edge profiling to block format: ``llvm-prof -to-block merged.out output.out``
5. curve fit and predicted: ``src/curve-fit bitcode 24:24.out 36:36.out ... +120:+120.out +240:+240.out``
   put 24.out, 36.out ... as input with x axis 24, 36 ... 
   and predict x=120, 240 write out to file +120.out and +240.out. 
   the '+' flag is used to switch mode
=======
*  drawline.py : used to draw lines from value profiling
*  dirdiff.sh  : compare two dir's llvmprof.out file and report whether they are
   same

unit test
---------

we provide some unit test in ``unit`` dir. to compile them, you need `google
test`_ and `google mock`_

.. _google test: https://code.google.com/p/googletest
.. _google mock: https://code.google.com/p/googlemock

.. code:: bash

   $ cmake .. -DUNIT_TEST=On
   $ make
   $ cd unit
   $ ./unit_test

documents
----------

we provide documents on wiki pages, you can use git submodule to sync them into
doc folder.

.. code:: bash

   $ git submodule init
   $ git submodule update
