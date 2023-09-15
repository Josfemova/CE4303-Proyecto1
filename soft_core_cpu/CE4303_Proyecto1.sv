module CE4303_Proyecto1(
    input clk_50,  
    input reset, // sw9
    input [3:0] pio_btn,
    input [8:0] pio_sw,
    output [6:0] hex0, hex1, hex2, hex3, hex4, hex5,
    output [12:0] dram_addr,
    output [1:0] dram_ba,
    output dram_cas_n, dram_ras_n, dram_clk, dram_cke, dram_cs_n, dram_we_n,
           dram_udqm, dram_ldqm,
    inout [15:0] dram_dq,
	// DDR3 SDRAM
    output		[14: 0]	HPS_DDR3_ADDR,
    output		[ 2: 0] HPS_DDR3_BA,
    output				HPS_DDR3_CAS_N,
    output				HPS_DDR3_CKE,
    output				HPS_DDR3_CK_N,
    output				HPS_DDR3_CK_P,
    output				HPS_DDR3_CS_N,
    output		[ 3: 0]	HPS_DDR3_DM,
    inout		[31: 0]	HPS_DDR3_DQ,
    inout		[ 3: 0]	HPS_DDR3_DQS_N,
    inout		[ 3: 0]	HPS_DDR3_DQS_P,
    output				HPS_DDR3_ODT,
    output				HPS_DDR3_RAS_N,
    output				HPS_DDR3_RESET_N,
    input				HPS_DDR3_RZQ,
    output				HPS_DDR3_WE_N
);

logic [23:0] pio_7_seg;
//assign dram_clk = clk_50;
platform instancia_nios(
		.clk_clk(clk_50),          //        clk.clk
		.pio_7seg_export(pio_7_seg),  //   pio_7seg.export
		.pio_btn_export(pio_btn),   //    pio_btn.export
		.pio_sw_export(pio_sw),    //     pio_sw.export
		.reset_reset(reset),      //      reset.reset
		.sdram_clk_clk(dram_clk),    //  sdram_clk.clk
		.sdram_wire_addr(dram_addr),  // sdram_wire.addr
		.sdram_wire_ba(dram_ba),    //           .ba
		.sdram_wire_cas_n(dram_cas_n), //           .cas_n
		.sdram_wire_cke(dram_cke),   //           .cke
		.sdram_wire_cs_n(dram_cs_n),  //           .cs_n
		.sdram_wire_dq(dram_dq),    //           .dq
		.sdram_wire_dqm({dram_udqm, dram_ldqm}),   //           .dqm
		.sdram_wire_ras_n(dram_ras_n), //           .ras_n
		.sdram_wire_we_n(dram_we_n),   //           .we_n
		
	// DDR3 SDRAM
	.memory_mem_a			(HPS_DDR3_ADDR),
	.memory_mem_ba			(HPS_DDR3_BA),
	.memory_mem_ck			(HPS_DDR3_CK_P),
	.memory_mem_ck_n		(HPS_DDR3_CK_N),
	.memory_mem_cke		(HPS_DDR3_CKE),
	.memory_mem_cs_n		(HPS_DDR3_CS_N),
	.memory_mem_ras_n		(HPS_DDR3_RAS_N),
	.memory_mem_cas_n		(HPS_DDR3_CAS_N),
	.memory_mem_we_n		(HPS_DDR3_WE_N),
	.memory_mem_reset_n	(HPS_DDR3_RESET_N),
	.memory_mem_dq			(HPS_DDR3_DQ),
	.memory_mem_dqs		(HPS_DDR3_DQS_P),
	.memory_mem_dqs_n		(HPS_DDR3_DQS_N),
	.memory_mem_odt		(HPS_DDR3_ODT),
	.memory_mem_dm			(HPS_DDR3_DM),
	.memory_oct_rzqin		(HPS_DDR3_RZQ)
);
		
seven_segment_driver seg0(.data_in(pio_7_seg[3:0]), .data_out(hex0));
seven_segment_driver seg1(.data_in(pio_7_seg[7:4]), .data_out(hex1));
seven_segment_driver seg2(.data_in(pio_7_seg[11:8]), .data_out(hex2));
seven_segment_driver seg3(.data_in(pio_7_seg[15:12]), .data_out(hex3));
seven_segment_driver seg4(.data_in(pio_7_seg[19:16]), .data_out(hex4));
seven_segment_driver seg5(.data_in(pio_7_seg[23:20]), .data_out(hex5));
		
endmodule
