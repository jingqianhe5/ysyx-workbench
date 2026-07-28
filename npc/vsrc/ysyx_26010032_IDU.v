module ysyx_26010032_IDU(
    input           clk,
    input   [31:0]  a0,
    input   [31:0]  ins,

    output  reg [31:0]  imm,
    output  reg [3:0]   alu_op,
    output  reg [4:0]   rs1,
    output  reg [4:0]   rs2,
    output  reg [4:0]   rd,

    output  reg              reg_wen,
    output  reg [3:0]        src1_sel,
    output  reg [3:0]        src2_sel,
    output  reg [3:0]        pc_sel
    //output          is_ebreak
);

//ALU操作类型
    localparam ALU_ADD = 4'd0;
    localparam ALU_SLL = 4'd1;
    localparam ALU_SLT = 4'd2;
    localparam ALU_XOR = 4'd3;
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

    localparam PC_NEXT  = 4'd0;
    localparam PC_JAL   = 4'd1;
    localparam PC_JALR  = 4'd2;

    assign rs1 = ins[19:15];
    assign rs2 = ins[24:20];
    assign rd  = ins[11:7];

    always @(*)begin
            imm          = 32'b0;
            alu_op       = ALU_ADD;
            reg_wen      = 1'b0;
            src1_sel     = SRC1_RS1;
            src2_sel     = SRC2_RS2;
            pc_sel       = PC_NEXT;
            case (ins[6:0])
                7'b0010011: begin //I-type addi
                    imm  =  {{20{ins[31]}}, ins[31:20]};
                    alu_op       = ALU_ADD;
                    reg_wen      = 1'b1;
                    src1_sel     = SRC1_RS1;
                    src2_sel     = SRC2_IMM;
                    pc_sel       = PC_NEXT;
                end
                7'b0110111: begin //U-type lui
                    imm  =  {ins[31:12],12'b0};
                    alu_op       = ALU_ADD;
                    reg_wen      = 1'b1;
                    src1_sel     = SRC1_NONE;
                    src2_sel     = SRC2_IMM;
                    pc_sel       = PC_NEXT; 
                end
                7'b1101111: begin //J-type jal
                    imm  =  {{11{ins[31]}}, ins[31], ins[19:12],ins[20], ins[30:21], 1'b0};
                    alu_op       = ALU_ADD;
                    reg_wen      = 1'b1;
                    src1_sel     = SRC1_PC;
                    src2_sel     = SRC2_4; 
                    pc_sel       = PC_JAL;
                end
                7'b1100111: begin //I-type jalr
                    imm  =  {{20{ins[31]}}, ins[31:20]};
                    alu_op       = ALU_ADD;
                    reg_wen      = 1'b1;
                    src1_sel     = SRC1_PC;
                    src2_sel     = SRC2_4; 
                    pc_sel       = PC_JALR;
                end
                7'b0010111: begin
                    imm      = {ins[31:12], 12'b0};
                    alu_op   = ALU_ADD;
                    reg_wen  = 1'b1;
                    src1_sel = SRC1_PC;
                    src2_sel = SRC2_IMM;
                    pc_sel   = PC_NEXT;
                end 
            endcase
    end

endmodule
