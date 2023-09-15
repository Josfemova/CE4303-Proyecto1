module seven_segment_driver(
	input [3:0] data_in,
	output reg [6:0] data_out
);

always @(*) begin
	case (data_in)
		4'h0: data_out[6:0] = ~7'b0111111;
		4'h1: data_out[6:0] = ~7'b0000110;
		4'h2: data_out[6:0] = ~7'b1011011;
		4'h3: data_out[6:0] = ~7'b1001111;
		4'h4: data_out[6:0] = ~7'b1100110;
		4'h5: data_out[6:0] = ~7'b1101101;
		4'h6: data_out[6:0] = ~7'b1111101;
		4'h7: data_out[6:0] = ~7'b0000111;
		4'h8: data_out[6:0] = ~7'b1111111;
		4'h9: data_out[6:0] = ~7'b1101111;
		4'hA: data_out[6:0] = ~7'b1110111;
		4'hB: data_out[6:0] = ~7'b1111100;
		4'hC: data_out[6:0] = ~7'b0111001;
		4'hD: data_out[6:0] = ~7'b1011110;
		4'hE: data_out[6:0] = ~7'b1111001;
		4'hF: data_out[6:0] = ~7'b1110001;
	endcase
end

endmodule
