module ysyx_26010032_ALU(
    input [19:0] imm,
    input [31:0] src1,
    /* verilator lint_off UNUSEDSIGNAL */
    input [31:0] src2,
    input [2:0] f3,
    input [6:0] f7,
    input [2:0] inst_type,
    input [31:0] pc,

    input is_alu_i,
    input is_jalr ,
    input is_jal  ,
    input is_lui  ,
    input is_auipc,
    /* verilator lint_on UNUSEDSIGNAL */

    output [31:0] result,
    output [31:0] new_pc,
    output wen,
    output change_pc
);
    //符号拓展
    wire [31:0] imm_32 =
        (inst_type == 3'b001)?{{20{imm[19]}},imm[19:8]}://I
        (inst_type == 3'b010)?{imm[19:0],12'b0}:        //U
        (inst_type == 3'b011)?{{11{imm[19]}},imm[19:0],1'b0}://J
        32'b0;

    
    assign result = 
        (is_alu_i)  ?   //I
            ((f3 == 3'b000) ? (imm_32+src1): 
            32'b0):
        (is_jalr)   ?   //I
            ((f3 == 3'b000) ? (pc + 32'd4):
            32'b0):
        (is_jal)    ?   (pc + 32'd4)://J
        (is_lui)    ?   (imm_32):
        (is_auipc)  ?   (imm_32+pc):
        32'b0;

    assign new_pc = 
        (is_jalr) ? ((src1 + imm_32) & ~32'b1) : // jalr 的跳转目标
        (is_jal)  ? (pc + imm_32)              : // jal 的跳转目标
        32'b0;

    assign change_pc = is_jal | is_jalr;

    assign wen = is_alu_i | is_jalr | is_jal | is_lui | is_auipc;

endmodule
