module ysyx_26010032_ALU(
    input  [31:0] src1,
    input  [31:0] src2,
    input  [3:0]  alu_op,

    output reg [31:0] result
);
    localparam ALU_ADD = 4'd0;
    localparam ALU_SLL = 4'd1;
    localparam ALU_SLT = 4'd2;
    localparam ALU_XOR = 4'd3;
    
always@(*)begin
    result = 32'b0;
    case (alu_op)
        ALU_ADD : begin
            result = src1 + src2;
        end
        ALU_SLL : begin
            result = src1 << src2;
        end
        ALU_SLT : begin
            result = src1 < src2;
        end
        ALU_XOR : begin
            result = src1 ^ src2;
        end
    endcase
        
    
end

endmodule
