/**
 * VES-369 VORTEX PROTOCOL: C++ ENGINE
 * Copyright (c) 2026 Jonathan Alan Reed
 * LICENSE: GNU Affero General Public License v3.0 (AGPL-3.0)
 */

#ifndef VES369_engine_HPP
#define VES369_engine_HPP

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstdint>

class Vortex369 {
private:
    const std::string ALL_CHARS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n\t\r";

    void writeBits(uint16_t value, int count, std::vector<uint8_t>& buffer, uint8_t& currentByte, int& bitOffset) {
        for (int i = count - 1; i >= 0; --i) {
            if ((value >> i) & 1) currentByte |= (1 << (7 - bitOffset));
            if (++bitOffset == 8) {
                buffer.push_back(currentByte);
                currentByte = 0;
                bitOffset = 0;
            }
        }
    }

public:
    std::vector<uint8_t> compress(const std::string& input) {
        if (input.empty()) return {};

        // 1. Frequency Analysis & Lattice Generation
        std::map<char, int> counts;
        for (char c : input) counts[c]++;
        std::vector<char> sorted;
        for (std::map<char, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
            sorted.push_back(it->first);
        }
        std::sort(sorted.begin(), sorted.end(), [&](char a, char b) { return counts[a] > counts[b]; });

        std::string L3 = "";
        for (int i = 0; i < std::min((int)sorted.size(), 8); ++i) L3 += sorted[i];
        while (L3.length() < 8) L3 += " ";

        // 2. Derive L6 and L9 from remaining ALL_CHARS
        std::string L6 = "", L9 = "";
        for (char c : ALL_CHARS) {
            if (L3.find(c) == std::string::npos) {
                if (L6.length() < 64) L6 += c;
                else L9 += c;
            }
        }

        // 3. Header Setup (8 Bytes for Lattice + 4 Bytes for Bit-Stop Count)
        std::vector<uint8_t> buffer;
        for (char c : L3) buffer.push_back((uint8_t)c);
        
        // Placeholder for 32-bit bitCount (to be filled later)
        for(int i=0; i<4; i++) buffer.push_back(0);

        uint8_t currentByte = 0;
        int bitOffset = 0;
        uint32_t totalBitsEncoded = 0;

        // 4. Multi-Layer Vortex Encoding
        for (char c : input) {
            size_t i3 = L3.find(c);
            if (i3 != std::string::npos) {
                writeBits(0b01, 2, buffer, currentByte, bitOffset);
                writeBits(i3, 3, buffer, currentByte, bitOffset);
                totalBitsEncoded += 5;
            } else {
                size_t i6 = L6.find(c);
                if (i6 != std::string::npos) {
                    writeBits(0b10, 2, buffer, currentByte, bitOffset);
                    writeBits(i6, 6, buffer, currentByte, bitOffset);
                    totalBitsEncoded += 8;
                } else {
                    size_t i9 = L9.find(c);
                    uint8_t idx = (i9 == std::string::npos) ? 0 : (uint8_t)i9;
                    writeBits(0b11, 2, buffer, currentByte, bitOffset);
                    writeBits(idx, 7, buffer, currentByte, bitOffset);
                    totalBitsEncoded += 9;
                }
            }
        }
        if (bitOffset > 0) buffer.push_back(currentByte);

        // Fill in the Bit-Stop Count in the Header
        buffer[8] = (totalBitsEncoded >> 24) & 0xFF;
        buffer[9] = (totalBitsEncoded >> 16) & 0xFF;
        buffer[10] = (totalBitsEncoded >> 8) & 0xFF;
        buffer[11] = totalBitsEncoded & 0xFF;

        return buffer;
    }

    std::string decompress(const std::vector<uint8_t>& data) {
        if (data.size() < 12) return "";
        
        std::string L3(data.begin(), data.begin() + 8);
        uint32_t totalBitsEncoded = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
        
        std::string L6 = "", L9 = "";
        for (char c : ALL_CHARS) {
            if (L3.find(c) == std::string::npos) {
                if (L6.length() < 64) L6 += c;
                else L9 += c;
            }
        }

        std::string output = "";
        uint32_t bitsRead = 0;
        int bitPos = 96; // Start after 12-byte header (8+4)

        auto readBit = [&]() mutable {
            bool b = (data[bitPos / 8] >> (7 - (bitPos % 8))) & 1;
            bitPos++;
            bitsRead++;
            return b;
        };

        while (bitsRead < totalBitsEncoded) {
            bool p1 = readBit();
            bool p2 = readBit();
            if (!p1 && p2) { // 01
                int delta = 0;
                for(int i=0; i<3; i++) if(readBit()) delta |= (1 << (2-i));
                output += L3[delta];
            } else if (p1 && !p2) { // 10
                int delta = 0;
                for(int i=0; i<6; i++) if(readBit()) delta |= (1 << (5-i));
                output += L6[delta];
            } else if (p1 && p2) { // 11
                int delta = 0;
                for(int i=0; i<7; i++) if(readBit()) delta |= (1 << (6-i));
                output += L9[delta];
            }
        }
        return output;
    }
};

#endif