module ysyx_26010032_pc(
    input           clk,
    input           rst,
    input           new_pc_en,
    input   [31:0]  new_pc,
    output  [31:0]  pc
);
    wire [31:0] pc_next;

    Reg #(32, 32'h80000000) 
    pc_reg (
        .clk    (clk), 
        .rst    (rst), 
        .din    (pc_next), 
        .dout   (pc), 
        .wen    (1'b1)
    );//触发器

    assign pc_next = new_pc_en ? new_pc : pc + 32'd4;

endmodule
