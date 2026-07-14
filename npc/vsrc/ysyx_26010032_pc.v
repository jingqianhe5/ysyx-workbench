module ysyx_26010032_pc(
    input clk,
    input rst,
    input change_pc,
    input [31:0]npc,
    output [31:0] pc
);
    wire [31:0] pc_next;

    Reg #(32, 32'h80000000) 
    pc_reg (
        .clk(clk), 
        .rst(rst), 
        .din(pc_next), 
        .dout(pc), 
        .wen(1'b1)
    );//触发器

    assign pc_next = change_pc ? npc:pc + 32'd4;

endmodule
