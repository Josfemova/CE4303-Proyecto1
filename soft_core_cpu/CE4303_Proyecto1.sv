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
    inout [15:0] dram_dq
);

logic [23:0] pio_7_seg;

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
		.sdram_wire_we_n(dram_we_n)   //           .we_n
);
		
seven_segment_driver seg0(.data_in(pio_7_seg[3:0]), .data_out(hex0));
seven_segment_driver seg1(.data_in(pio_7_seg[7:4]), .data_out(hex1));
seven_segment_driver seg2(.data_in(pio_7_seg[11:8]), .data_out(hex2));
seven_segment_driver seg3(.data_in(pio_7_seg[15:12]), .data_out(hex3));
seven_segment_driver seg4(.data_in(pio_7_seg[19:16]), .data_out(hex4));
seven_segment_driver seg5(.data_in(pio_7_seg[23:20]), .data_out(hex5));
		
endmodule
