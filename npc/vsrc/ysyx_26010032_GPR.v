module ysyx_26010032_GPR #(ADDR_WIDTH = 5, DATA_WIDTH = 32) (
  input clk,
  input [DATA_WIDTH-1:0] wdata,
  input [ADDR_WIDTH-1:0] waddr,
  input wen,

  output [DATA_WIDTH-1:0] rdata_1,
  input [ADDR_WIDTH-1:0] raddr_1,
  output [DATA_WIDTH-1:0] rdata_2,
  input [ADDR_WIDTH-1:0] raddr_2,
  output [DATA_WIDTH-1:0] a0
);
  reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];
  always @(posedge clk) begin
    if (wen&&waddr!=0) rf[waddr] <= wdata;
  end


assign rdata_1 = (raddr_1==0?0:rf[raddr_1]);
assign rdata_2 = (raddr_2==0?0:rf[raddr_2]);
assign a0 = rf[10];

endmodule
