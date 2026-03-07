/**
 * VES-369 VORTEX PROTOCOL: INDUSTRIAL JS TESTBENCH
 * Copyright (c) 2026 Jonathan Alan Reed
 * LICENSE: GNU Affero General Public License v3.0 (AGPL-3.0)
 */

const Vortex369 = require('./VES-369_engine.js');

function runTest(testName, data) {
    const engine = new Vortex369();
    console.log(`\n[ TEST CASE ]: ${testName}`);
    console.log("-".repeat(70));

    const start = performance.now();
    const compressed = engine.compress(data);
    const end = performance.now();
    const elapsedSeconds = (end - start) / 1000;

    const decompressed = engine.decompress(compressed);
    const isLossless = (data === decompressed);

    const originalSize = data.length;
    const compressedSize = compressed.length; 
    const reduction = (1.0 - (compressedSize / originalSize)) * 100.0;
    const bitsPerChar = (compressedSize * 8) / data.length;

    console.log(`${"  PHYSICAL INPUT:".padEnd(25)} ${originalSize} Bytes`);
    console.log(`${"  VORTEX OUTPUT:".padEnd(25)} ${compressedSize} Bytes`);
    console.log(`${"  REDUCTION DELTA:".padEnd(25)} ${reduction.toFixed(2)}%`);
    console.log(`${"  BIT VIBRATION:".padEnd(25)} ${bitsPerChar.toFixed(2)} Bits/Char`);
    console.log(`${"  INTEGRITY STATUS:".padEnd(25)} ${isLossless ? "100% LOSSLESS [VERIFIED]" : "FAIL"}`);
    
    if (elapsedSeconds > 0) {
        const mbps = (originalSize / 1024 / 1024) / elapsedSeconds;
        console.log(`${"  ENGINE VELOCITY:".padEnd(25)} ${mbps.toFixed(2)} MB/s`);
    }
    console.log("-".repeat(70));
}

console.log("=".repeat(70));
console.log("               VES-369 VORTEX PROTOCOL: JS INDUSTRIAL AUDIT            ");
console.log("=".repeat(70));

let telemetry = "";
for(let i=0; i<500; i++) telemetry += "STATUS:ACTIVE;TEMP:98.6;SYNC:369;LEVEL:STABLE;";
runTest("INDUSTRIAL TELEMETRY STREAM", telemetry);

let binaryStr = "";
for(let i=0; i<1000; i++) binaryStr += (i % 2 === 0 ? "1" : "0");
runTest("BINARY MANIFOLD (REPETITIVE)", binaryStr);

let sourceCode = JSON.stringify({metadata:{id:369,ver:"2.9"},data:[1,0,1,1,0,0,1,0,1,1,0,1,0]});
let longCode = "";
for(let i=0; i<100; i++) longCode += sourceCode;
runTest("STRUCTURED SOURCE CODE (JSON)", longCode);

let largeData = "";
for(let i=0; i<20000; i++) largeData += "VES-369-VORTEX-STRESS-TEST-DATA-PACKET-0123456789";
runTest("1MB INDUSTRIAL STRESS TEST", largeData);