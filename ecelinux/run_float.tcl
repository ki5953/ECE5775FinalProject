#=============================================================================
# run_float.tcl 
#=============================================================================
# @brief: A Tcl script for synthesizing the float baseline iris recognition design.

# Project name
set hls_prj iris_float_pipeline_predict_2.prj
# set hls_prj iris_float.prj

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
set_directive_pipeline gnb_predict/PREDICT_LOOP
set_directive_array_partition gnb_predict std_dev -dim 0
set_directive_array_partition gnb_predict prior -dim 0
set_directive_array_partition gnb_predict mean -dim 0
set_directive_array_partition gnb_predict X -dim 0


############################################

# Simulate the C++ design
csim_design
# Synthesize the design
csynth_design
# Co-simulate the design
#cosim_design
exit
