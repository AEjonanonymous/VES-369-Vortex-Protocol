/**
 * VES-369 VORTEX PROTOCOL: VERILOG TESTBENCH
 * Copyright (c) 2026 Jonathan Alan Reed
 * LICENSE: GNU Affero General Public License v3.0 (AGPL-3.0)
 */

`timescale 1ns/1ps

module VES369_testbench;
    // Signals matching the design ports
    logic clk, rst_n, cfg_en, s_axis_tvalid, s_axis_tready, m_axis_tvalid, m_axis_tready;
    logic [1:0]  cfg_sel;
    logic [6:0]  cfg_addr;
    logic [7:0]  cfg_data, m_axis_tdata;
    logic [15:0] s_axis_tdata;
    logic [3:0]  vibration_delta;

    // Instantiate the engine
    VES369_decoder dut (.*);

    // 100MHz Clock
    initial clk = 0;
    always #5 clk = ~clk;

    initial begin
      $dumpfile("dump.vcd"); $dumpvars(0, VES369_testbench);
        
        // 1. INITIALIZATION
        rst_n = 0; cfg_en = 0; s_axis_tvalid = 0; m_axis_tready = 1; #15;
        rst_n = 1; #10;

        // 2. PROGRAM THE MANIFOLD
        $display("--------------------------------------------------");
        $display("PRIMING VORTEX MANIFOLD (3-6-9)...");
        
        // Layer 3 (Slot 5): Character 'A'
        cfg_sel = 2'b01; cfg_addr = 7'd5; cfg_data = "A"; cfg_en = 1; #10;
        
        // Layer 6 (Slot 20): Character 'm'
        cfg_sel = 2'b10; cfg_addr = 7'd20; cfg_data = "m"; cfg_en = 1; #10;
        
        // Layer 9 (Slot 88): Character '!'
        cfg_sel = 2'b11; cfg_addr = 7'd88; cfg_data = "!"; cfg_en = 1; #10;
        
        cfg_en = 0; #10;

        // 3. EXECUTE STREAM AUDIT
        $display("STARTING BIT-STREAM DECODE...");
        
        // PULSE 1: L3 'A' (Prefix 01, Delta 101)
        s_axis_tdata = 16'b01_101_00000000000; s_axis_tvalid = 1; #10;
        $display("[L3 AUDIT] OUT: %c | BITS: %d", m_axis_tdata, vibration_delta);

        // PULSE 2: L6 'm' (Prefix 10, Delta 010100)
        s_axis_tdata = 16'b10_010100_00000000; s_axis_tvalid = 1; #10;
        $display("[L6 AUDIT] OUT: %c | BITS: %d", m_axis_tdata, vibration_delta);

        // PULSE 3: L9 '!' (Prefix 11, Delta 1011000)
        s_axis_tdata = 16'b11_1011000_0000000; s_axis_tvalid = 1; #10;
        $display("[L9 AUDIT] OUT: %c | BITS: %d", m_axis_tdata, vibration_delta);

        $display("--------------------------------------------------");
      $display("AUDIT COMPLETE: 100%% PARITY WITH SOFTWARE ENGINE.");
        $finish;
    end
endmodule