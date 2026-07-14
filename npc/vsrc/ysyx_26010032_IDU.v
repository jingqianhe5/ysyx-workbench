import "DPI-C" function void npc_trap(int code);

module ysyx_26010032_IDU(
    input clk,
    input [31:0] ins,

    output [19:0] imm,
    output [2:0] inst_type,
    output [4:0] rs1,
    output [4:0] rs2,
    output [4:0] rd,
    output [2:0] f3,
    output [6:0] f7,

    output is_alu_i,
    output is_jalr ,
    output is_jal  ,
    output is_lui  ,
    output is_auipc,

    input [31:0] a0
);
    wire [6:0] opcode;
    assign rs1 = ins[19:15];
    assign rs2 = ins[24:20];
    assign rd  = ins[11:7];

    assign opcode = ins[6:0];
    assign f3     = ins[14:12];
    assign f7     = ins[31:25]; 


//输出这些内容
    assign is_alu_i = (opcode == 7'b0010011); // I-type 算术 (addi, slli等)
    assign is_jalr  = (opcode == 7'b1100111); // I-type 寄存器跳转
    assign is_jal   = (opcode == 7'b1101111); // J-type 无条件跳转
    assign is_lui   = (opcode == 7'b0110111); // U-type LUI
    assign is_auipc = (opcode == 7'b0010111); // U-type AUIPC

    //wire is_load  = (opcode == 7'b0000011); // I-type 加载 (lw, lb等)
    //wire is_store = (opcode == 7'b0100011); // S-type 存储 (sw, sb等)
    //wire is_branch= (opcode == 7'b1100011); // B-type 分支 (beq, bne等)


    selector #(
        .NR_KEY  (4),   
        .KEY_LEN (7),   
        .DATA_LEN(23)   
    )
    u_type_selector(
        .out    ({imm,inst_type}),
        .key    (opcode),
        .lut    ({
            7'b0010011,           {ins[31:20],8'b0,3'b001} ,//I-type-1
            7'b1100111,           {ins[31:20],8'b0,3'b001} ,//I-type-1
            7'b0110111,           {ins[31:12],3'b010} ,//U-type-2
            7'b1101111,           {ins[31],ins[19:12],ins[20],ins[30:21],3'b011}//J-type-3
        })
    );


    always @(posedge clk)begin
        if(ins==32'h00100073)begin
            npc_trap(a0);
        end
    end
endmodule
