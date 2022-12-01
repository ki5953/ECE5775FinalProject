#==========================================================================
# Makefile
#==========================================================================
# @brief: A makefile the compiles and synthesizes the digitrec program
#
# @desc: 1. "make" runs csim by default
#        2. "make csim" compiles & executes the fixed-point implementation
#        3. "make clean" cleans up the directory


# Extract Vivado HLS include path
VHLS_PATH := $(dir $(shell which vivado_hls))/..
VHLS_INC ?= ${VHLS_PATH}/include

CFLAGS = -g -I${VHLS_INC} -std=c++11

all: csim

csim: iris.cpp iris_test.cpp
	@echo "Compiling & simulating iris on amdpool ..."
	g++ ${CFLAGS} $^ -o iris -lrt
	./iris

fixed_vivado:
	vivado_hls -f run_fixed.tcl

float_vivado:
	vivado_hls -f run_float.tcl

float_bitstream: 
	@echo "Synthesizing iris and creating bitstream on amdpool ..."
	vivado_hls -f run_float.tcl
	./run_bitstream.sh iris_float_nopragma

fixed_bitstream: 
	@echo "Synthesizing iris and creating bitstream on amdpool ..."
	vivado_hls -f run_fixed.tcl
	./run_bitstream.sh iris_fixed_2decbits

dummy_bitstream:
	@echo "Synthesizing dummy and creating bitstream on amdpool ..."
	vivado_hls -f run_dummy.tcl
	./run_bitstream.sh iris_dummy

clean:
	rm -rf iris *.dat *.prj *.log
	rm -rf zedboard_project* xillydemo.bit
