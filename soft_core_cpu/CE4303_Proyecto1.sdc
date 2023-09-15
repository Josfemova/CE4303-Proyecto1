#**************************************************************
# Altera DE1-SoC SDC settings
# Users are recommended to modify this file to match users logic.
#**************************************************************

#**************************************************************
# Create Clock
#**************************************************************
create_clock -period 20 [get_ports clk_50]

create_clock -period "100 MHz" -name clk_dram [get_ports dram_clk]


#**************************************************************
# Create Generated Clock
#**************************************************************
derive_pll_clocks



#**************************************************************
# Set Clock Latency
#**************************************************************



#**************************************************************
# Set Clock Uncertainty
#**************************************************************
derive_clock_uncertainty



#**************************************************************
# Set Input Delay
#**************************************************************
# Board Delay (Data) + Propagation Delay - Board Delay (Clock)
set_input_delay -max -clock clk_dram -0.048 [get_ports dram_dq*]
set_input_delay -min -clock clk_dram -0.057 [get_ports dram_dq*]


#**************************************************************
# Set Output Delay
#**************************************************************
# max : Board Delay (Data) - Board Delay (Clock) + tsu (External Device)
# min : Board Delay (Data) - Board Delay (Clock) - th (External Device)
set_output_delay -max -clock clk_dram 1.452  [get_ports dram_dq*]
set_output_delay -min -clock clk_dram -0.857 [get_ports dram_dq*]
set_output_delay -max -clock clk_dram 1.531 [get_ports dram_addr*]
set_output_delay -min -clock clk_dram -0.805 [get_ports dram_addr*]
set_output_delay -max -clock clk_dram 1.533  [get_ports dram_*dqm]
set_output_delay -min -clock clk_dram -0.805 [get_ports dram_*dqm]
set_output_delay -max -clock clk_dram 1.510  [get_ports dram_ba*]
set_output_delay -min -clock clk_dram -0.800 [get_ports dram_ba*]
set_output_delay -max -clock clk_dram 1.520  [get_ports dram_ras_n]
set_output_delay -min -clock clk_dram -0.780 [get_ports dram_ras_n]
set_output_delay -max -clock clk_dram 1.5000  [get_ports dram_cas_n]
set_output_delay -min -clock clk_dram -0.800 [get_ports dram_cas_n]
set_output_delay -max -clock clk_dram 1.545 [get_ports dram_we_n]
set_output_delay -min -clock clk_dram -0.755 [get_ports dram_we_n]
set_output_delay -max -clock clk_dram 1.496  [get_ports dram_cke]
set_output_delay -min -clock clk_dram -0.804 [get_ports dram_cke]
set_output_delay -max -clock clk_dram 1.508  [get_ports dram_cs_n]
set_output_delay -min -clock clk_dram -0.792 [get_ports dram_cs_n]

