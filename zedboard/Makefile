#==========================================================================
# Makefile
#==========================================================================
# @brief: A makefile the compiles and runs the digitrec program
#
# @desc: 1. "make" or "make sw" runs software execution by default
#        2. "make fpga" invokes the HW accelerator
#        3. "make clean" cleans up the directory


INC_PATH=/usr/include/vivado_hls
CFLAGS = -I${INC_PATH} -DHLS_NO_XIL_FPO_LIB

all: sw

sw: iris.cpp iris_test.cpp
	@echo "Compiling & executing digitrec software program on ARM ..."
	g++ ${CFLAGS} iris.cpp iris_test.cpp -o iris-arm -lrt
	./iris-arm

fpga: host.cpp
	@echo "Compiling host program & running digitrec accelerator ..."
	g++ ${CFLAGS} host.cpp -o iris-fpga -lrt
	./iris-fpga

# Clean up the directory before submission
clean:
	@echo "Clean up output files"
	rm -rf iris-sw vivado_hls.log *.prj result out.dat *~
	rm -rf iris-fpga
