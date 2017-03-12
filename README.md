# CSCC69---A2
Page Tables and Replacement Algorithms

Note:
To create trace files, do the following steps:
  1. Go to starter/traceprogs
  2. The following are trace programs: matmul.c, simpleloop.c, blocked.c and factorial.c
  3. For any x, where x is a trace program, run the following command: runit x
  4. This should give you a trace file with it's extension as .ref file. 


To test an alogithm on a given replacement algorithm w, run the following command:
  ./sim -f traceprogs/traceFile -m FRAME_SIZE -s MEMORY_SIZE -a w
