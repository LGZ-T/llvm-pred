#/bin/bash 

if [ ! -d "../build" ]; then
    echo "build directory is not exist, Please build llvm-pred"
    exit 0
fi

if [ ! -f "../build/src/originhpccg.bc" ]; then
    echo "Please build originhpccg.bc, and then copy it to llvm-pred/build/src/"
    exit 0
fi

if [ ! -f "../build/src/insttime" ]; then
    echo "Please run llvm-prof/build/src/inst-timing > insttime"
    echo "and copy insttime to llvm-pred/build/src."
    echo "!!!Add '#' before load and store instruction"
    exit 0
fi

opt-3.5 -load ../build/src/libLLVMPred.so -insert-edge-profiling ../build/src/origin.bc -o ../build/src/hpccg.edge.profile.bc

clang++-3.5 ../build/src/hpccg.edge.profile.bc -lprofile_rt -o ../build/src/hpccg_edge_profile

for ((i=50;i<201;i=i+10)) 
do
    ../build/src/hpccg_edge_profile 100 100 $i 1>/dev/null
    mv llvmprof.out.* ../build/src/"blockfreq."$i
    llvm-prof -timing=irinst-max ../build/src/origin.bc ../build/src/"blockfreq."$i ../build/src/insttime | \
    grep "Block Timing:" >> computetime
    rm *.yaml
done


