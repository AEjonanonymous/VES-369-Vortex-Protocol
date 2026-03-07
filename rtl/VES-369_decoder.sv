/**
 * VES-369 VORTEX PROTOCOL: VERILOG DECODER MODULE
 * Copyright (c) 2026 Jonathan Alan Reed
 * LICENSE: GNU Affero General Public License v3.0 (AGPL-3.0)
 */

`timescale 1ns/1ps

module VES369_decoder (
    input  logic        clk,
    input  logic        rst_n,

    // --- CONFIGURATION BUS (Lattice Loader) ---
    // Use this to program the top 8, next 64, and remaining characters
    input  logic [1:0]  cfg_sel,     // 01=L3, 10=L6, 11=L9
    input  logic [6:0]  cfg_addr,    // Index within the lattice
    input  logic [7:0]  cfg_data,    // Character to store
    input  logic        cfg_en,      // Write enable

    // --- SLAVE INTERFACE (Compressed Input) ---
    input  logic [15:0] s_axis_tdata,
    input  logic        s_axis_tvalid,
    output logic        s_axis_tready,

    // --- MASTER INTERFACE (Decoded Output) ---
    output logic [7:0]  m_axis_tdata,
    output logic        m_axis_tvalid,
    input  logic        m_axis_tready,

    // --- TELEMETRY ---
    output logic [3:0]  vibration_delta
);

    // Physical Lattice Storage
    logic [7:0] L3 [0:7];   // Layer 3: 8 slots
    logic [7:0] L6 [0:63];  // Layer 6: 64 slots
    logic [7:0] L9 [0:127]; // Layer 9: 128 slots

    // Internal wires to prevent "constant select" compiler warnings
    logic [1:0] prefix;
    logic [2:0] idx3;
    logic [5:0] idx6;
    logic [6:0] idx9;

    assign prefix = s_axis_tdata[15:14];
    assign idx3   = s_axis_tdata[13:11]; // 3-bit delta
    assign idx6   = s_axis_tdata[13:8];  // 6-bit delta
    assign idx9   = s_axis_tdata[13:7];  // 7-bit delta

    // Lattice Loader (Sequential)
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (int i=0; i<8;   i++) L3[i] <= 8'h0;
            for (int i=0; i<64;  i++) L6[i] <= 8'h0;
            for (int i=0; i<128; i++) L9[i] <= 8'h0;
        end else if (cfg_en) begin
            case (cfg_sel)
                2'b01: L3[cfg_addr[2:0]] <= cfg_data;
                2'b10: L6[cfg_addr[5:0]] <= cfg_data;
                2'b11: L9[cfg_addr[6:0]] <= cfg_data;
            endcase
        end
    end

    // Flow Control
    assign s_axis_tready = m_axis_tready;

    // The Vortex Decoding Engine (Combinational)
    always_comb begin
        m_axis_tdata    = 8'h00;
        m_axis_tvalid   = 1'b0;
        vibration_delta = 4'd0;

        if (rst_n && s_axis_tvalid) begin
            case (prefix)
                2'b01: begin // L3 (5-bit vibration)
                    m_axis_tdata    = L3[idx3];
                    vibration_delta = 4'd5;
                    m_axis_tvalid   = 1'b1;
                end
                2'b10: begin // L6 (8-bit vibration)
                    m_axis_tdata    = L6[idx6];
                    vibration_delta = 4'd8;
                    m_axis_tvalid   = 1'b1;
                end
                2'b11: begin // L9 (9-bit vibration)
                    m_axis_tdata    = L9[idx9];
                    vibration_delta = 4'd9;
                    m_axis_tvalid   = 1'b1;
                end
                default: m_axis_tvalid = 1'b0;
            endcase
        end
    end
endmodule