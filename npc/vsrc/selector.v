module selector #(NR_KEY = 1, KEY_LEN = 7, DATA_LEN = 12) (
  output [DATA_LEN-1:0] out,
  input [KEY_LEN-1:0] key,
  input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
);
  MuxKeyInternal #(NR_KEY, KEY_LEN, DATA_LEN, 0) i0 (out, key, {DATA_LEN{1'b0}}, lut);
endmodule

module MuxKeyInternal #(NR_KEY = 1, KEY_LEN = 1, DATA_LEN = 1, HAS_DEFAULT = 0) (
  output reg [DATA_LEN-1:0] out,
  input [KEY_LEN-1:0] key,
  input [DATA_LEN-1:0] default_out,
  input [NR_KEY*(KEY_LEN + DATA_LEN)-1:0] lut
);
  integer i;
  reg [KEY_LEN-1:0] lut_key;
  reg [DATA_LEN-1:0] lut_data;

  always @(*) begin
    out = default_out;
    for (i = 0; i < NR_KEY; i = i + 1) begin
      lut_key = lut[(NR_KEY - i) * (KEY_LEN + DATA_LEN) - 1 -: KEY_LEN];
      lut_data = lut[(NR_KEY - i) * (KEY_LEN + DATA_LEN) - KEY_LEN - 1 -: DATA_LEN];
      if (key == lut_key) begin
        out = lut_data;
      end
    end
  end
endmodule
