module ysyx_26010032_npc(
    input clk,
    input rst,
    input  [31:0] ins,

    output [31:0] out_pc
);


wire [31:0] npc;
wire change_pc;
wire [31:0]pc;
assign out_pc = pc;

ysyx_26010032_pc u_ysyx_26010032_pc(
    .clk    (clk),
    .rst    (rst),
    .change_pc(change_pc),
    .npc    (npc),
    .pc     (pc)


);

wire [19:0] imm;
wire [4:0]  rs1;
wire [4:0]  rs2;
wire [4:0]  rd;
wire [2:0]  f3;
/* verilator lint_off UNUSEDSIGNAL */
wire [6:0]  f7;
wire [2:0]  inst_type;

wire is_alu_i;
wire is_jalr ;
wire is_jal  ;
wire is_lui  ;
wire is_auipc;
ysyx_26010032_IDU u_ysyx_26010032_IDU(
    .clk    (clk),
    .ins    (ins),

    .imm    (imm),
    .inst_type (inst_type),
    .rs1    (rs1),
    .rs2    (rs2),
    .rd     (rd) ,
    .f3     (f3) ,
    .f7     (f7) ,

    .is_alu_i   (is_alu_i),
    .is_jalr    (is_jalr ),
    .is_jal     (is_jal  ),
    .is_lui     (is_lui  ),
    .is_auipc   (is_auipc),

    .a0         (a0)
);

wire [31:0] src1;
wire [31:0] src2;
wire [31:0] result;
wire wen;
wire [31:0] a0;
ysyx_26010032_GPR u_ysyx_26010032_GPR_rs1(//取出src1 存放结果
    .clk    (clk),
    .wdata  (result),
    .waddr  (rd),
    .wen    (wen),
    .rdata_1  (src1),
    .raddr_1  (rs1),
    .rdata_2  (src2),
    .raddr_2  (rs2),
    .a0         (a0)
    
);



ysyx_26010032_ALU u_ysyx_26010032_ALU(//计算
    .imm    (imm),
    .src1   (src1),
    .src2   (src2),
    .f3     (f3),
    .f7     (f7),
    .inst_type(inst_type),
    .pc     (pc),

    .is_alu_i(is_alu_i),
    .is_jalr (is_jalr ),
    .is_jal  (is_jal  ),
    .is_lui  (is_lui  ),
    .is_auipc(is_auipc),

    .result (result),
    .npc    (npc),
    .wen    (wen),
    .change_pc(change_pc)
);


endmodule
