import "DPI-C" function void itrace(input int unsigned pc, input int unsigned ins);
import "DPI-C" function void npc_trap(int code);
import "DPI-C" function int unsigned pmem_read(input int unsigned raddr);
module ysyx_26010032_npc(
    input           clk,
    input           rst
    //input  [31:0]   ins,

    //output [31:0]   inst_addr//指令地址
);


wire [31:0] new_pc;
wire        new_pc_en;//跳转指令有效信号
wire [31:0] pc;
reg [31:0] ins;

//assign inst_addr = pc;

always @(posedge clk) begin
    if (!rst) begin
        itrace(pc, ins);
    end
    if(ins==32'h00100073)begin
        npc_trap(a0);
    end
    
end
always @(*) begin
  if (rst) begin
    ins = 32'b0;
  end else begin
    ins = pmem_read(pc - 32'h80000000);
  end
end

ysyx_26010032_pc u_ysyx_26010032_pc(//计算指令地址
    .clk        (clk),
    .rst        (rst),
    .new_pc_en  (new_pc_en),
    .new_pc     (new_pc),
    .pc         (pc)


);
localparam [3:0] PC_NEXT = 4'd0;
localparam [3:0] PC_JAL  = 4'd1;
localparam [3:0] PC_JALR = 4'd2;
assign new_pc_en = (pc_sel != PC_NEXT);

assign new_pc =
    pc_sel == PC_JAL  ? pc + imm :
    pc_sel == PC_JALR ? (src1_reg + imm) & 32'hfffffffe :
                        32'b0;
wire [31:0] imm;
wire [4:0] rs1;
wire [4:0] rs2;
wire [4:0] rd;
wire     reg_wen;
wire [3:0] src1_sel;
wire [3:0] src2_sel;
wire [3:0] pc_sel;
wire [3:0] alu_op;
ysyx_26010032_IDU u_ysyx_26010032_IDU(
    .clk    (clk),
    .ins    (ins),

    .imm    (imm),
    .alu_op (alu_op),
    .rs1    (rs1),
    .rs2    (rs2),
    .rd     (rd) ,

    .reg_wen    (reg_wen),
    .src1_sel   (src1_sel),
    .src2_sel   (src2_sel),
    .pc_sel     (pc_sel),


    .a0         (a0)
);
//src1数据来源
    localparam SRC1_RS1 = 4'd0;
    localparam SRC1_IMM = 4'd1;
    localparam SRC1_NONE= 4'd2;
    localparam SRC1_PC  = 4'd3;
    localparam SRC1_4   = 4'd4;
//src2数据来源
    localparam SRC2_RS2 = 4'd0;
    localparam SRC2_IMM = 4'd1;
    localparam SRC2_NONE= 4'd2;
    localparam SRC2_PC  = 4'd3;
    localparam SRC2_4   = 4'd4;

wire [31:0] src1;
wire [31:0] src2;
wire [31:0] src1_reg;
wire [31:0] src2_reg;
assign src1 =   (src1_sel==SRC1_RS1 ? src1_reg :
                (src1_sel==SRC1_IMM ? imm:
                (src1_sel==SRC1_NONE ? 32'b0 :
                (src1_sel==SRC1_PC ? pc :
                (src1_sel==SRC1_4 ? 32'd4:
                32'b0
                )))));
assign src2 =   (src2_sel==SRC2_RS2 ? src2_reg :
                (src2_sel==SRC2_IMM ? imm:
                (src2_sel==SRC2_NONE ? 32'b0 :
                (src2_sel==SRC2_PC ? pc :
                (src2_sel==SRC2_4 ? 32'd4:
                32'b0
                )))));

wire [31:0] a0;
ysyx_26010032_GPR u_ysyx_26010032_GPR_rs1(//取出src1 存放结果
    .clk    (clk),
    .wdata  (result),
    .waddr  (rd),
    .wen    (reg_wen),
    .rdata_1  (src1_reg),
    .raddr_1  (rs1),
    .rdata_2  (src2_reg),
    .raddr_2  (rs2),
    .a0         (a0)
    
);

wire [31:0] result;

ysyx_26010032_ALU u_ysyx_26010032_ALU(//计算
.src1       (src1),
.src2       (src2),
.alu_op     (alu_op),
.result     (result)
);


endmodule
