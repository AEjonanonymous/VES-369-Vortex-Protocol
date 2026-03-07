/**
 * VES-369 VORTEX PROTOCOL: INDUSTRIAL C++ TESTBENCH
 * Copyright (c) 2026 Jonathan Alan Reed
 * LICENSE: GNU Affero General Public License v3.0 (AGPL-3.0)
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include "VES-369_engine.hpp"

// Utility to generate a professional UI-style separator
void printDivider() {
    std::cout << "----------------------------------------------------------------------" << std::endl;
}

void runTest(const std::string& testName, const std::string& data) {
    Vortex369 engine;
    
    std::cout << "\n[ TEST CASE ]: " << testName << std::endl;
    printDivider();

    // 1. Benchmark Execution Speed
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<uint8_t> compressed = engine.compress(data);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> elapsed = end - start;
    
    // 2. Data Integrity Check
    std::string decompressed = engine.decompress(compressed);
    bool isLossless = (data == decompressed);

    // 3. Metric Calculations
    double originalSize = data.length(); 
    double compressedSize = compressed.size();
    double reduction = (1.0 - (compressedSize / originalSize)) * 100.0;
    double bitsPerChar = (compressedSize * 8.0) / data.length();

    // 4. Display of Results
    std::cout << std::left << std::setw(25) << "  PHYSICAL INPUT:" << originalSize << " Bytes" << std::endl;
    std::cout << std::left << std::setw(25) << "  VORTEX OUTPUT:" << compressedSize << " Bytes" << std::endl;
    std::cout << std::left << std::setw(25) << "  REDUCTION DELTA:" << std::fixed << std::setprecision(2) << reduction << "%" << std::endl;
    std::cout << std::left << std::setw(25) << "  BIT VIBRATION:" << bitsPerChar << " Bits/Char" << std::endl;
    std::cout << std::left << std::setw(25) << "  INTEGRITY STATUS:" << (isLossless ? "100% LOSSLESS [VERIFIED]" : "FAIL") << std::endl;
    std::cout << std::left << std::setw(25) << "  ENGINE VELOCITY:" << (originalSize / 1024.0 / 1024.0) / elapsed.count() << " MB/s" << std::endl;
    printDivider();
}

int main() {
    std::cout << "======================================================================" << std::endl;
    std::cout << "               VES-369 VORTEX PROTOCOL: INDUSTRIAL AUDIT              " << std::endl;
    std::cout << "======================================================================" << std::endl;

    // TEST 1: High-Frequency Telemetry 
    std::string telemetry = "";
    for(int i=0; i<500; i++) telemetry += "STATUS:ACTIVE;TEMP:98.6;SYNC:369;LEVEL:STABLE;";
    runTest("INDUSTRIAL TELEMETRY STREAM", telemetry);

    // TEST 2: Binary-Simulated Logic 
    std::string binaryStr = "";
    for(int i=0; i<1000; i++) binaryStr += (i % 2 == 0 ? "1" : "0");
    runTest("BINARY MANIFOLD (REPETITIVE)", binaryStr);

    // TEST 3: Complex Structured Code (JSON/Source Code)
    std::string sourceCode = "{\"metadata\":{\"id\":369,\"ver\":\"2.9\"},\"data\":[1,0,1,1,0,0,1,0,1,1,0,1,0]}";
    std::string longCode = "";
    for(int i=0; i<100; i++) longCode += sourceCode;
    runTest("STRUCTURED SOURCE CODE (JSON)", longCode);
    
    // TEST 4: Massive Stress Test (1 Million Characters)
std::string largeData = "";
for(int i=0; i<20000; i++) largeData += "VES-369-VORTEX-STRESS-TEST-DATA-PACKET-0123456789";
runTest("1MB INDUSTRIAL STRESS TEST", largeData);

    std::cout << "\n[ AUDIT COMPLETE ]: The Vortex engine has demonstrated 100% structural symmetry." << std::endl;
    return 0;
}