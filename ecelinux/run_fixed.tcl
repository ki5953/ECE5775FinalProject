#=============================================================================
# run_fixed.tcl 
#=============================================================================
# @brief: A Tcl script for synthesizing the float baseline iris recognition design.

set num_dec_bits {1,2,3,4,5}

foreach { NUM_BIT } $num_dec_bits {
# Define the bitwidth macros from CFLAGs
set CFLAGS "-DNFIXED -DDEC_BITS=${NUM_BIT}"

# Project name
set hls_prj "iris_fixed_${NUM_BIT}decbits.prj"

# Open/reset the project
open_project ${hls_prj} -reset

# Top function of the design is "dut"
set_top dut

# Add design and testbench files
add_files iris.cpp
add_files -tb iris_test.cpp
add_files -tb data_iris

open_solution "solution1"
# Use Zynq device
set_part {xc7z020clg484-1}

# Target clock period is 10ns
create_clock -period 10

### You can insert your own directives here ###

############################################

# Simulate the C++ design
csim_design
# Synthesize the design
csynth_design
# Co-simulate the design
#cosim_design
}
exit
