/**
 * VES-369 VORTEX PROTOCOL: JS ENGINE
 * Copyright (c) 2026 Jonathan Alan Reed
 * LICENSE: GNU Affero General Public License v3.0 (AGPL-3.0)
 */

class Vortex369 {
    constructor() {
        this.ALL_CHARS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n\t\r";
    }

    // Internal bit-packer
    _writeBits(value, count, state) {
        for (let i = count - 1; i >= 0; i--) {
            if ((value >> i) & 1) {
                state.buffer[state.byteIdx] |= (1 << (7 - state.bitOffset));
            }
            state.bitOffset++;
            state.totalBits++;
            if (state.bitOffset === 8) {
                state.byteIdx++;
                state.bitOffset = 0;
            }
        }
    }

    compress(input) {
        if (!input) return new Uint8Array(0);

        // 1. Frequency Analysis & Lattice Generation
        let counts = {};
        for (let char of input) counts[char] = (counts[char] || 0) + 1;
        let sorted = Object.keys(counts).sort((a, b) => counts[b] - counts[a]);

        let L3 = sorted.slice(0, 8).join('');
        while (L3.length < 8) L3 += " ";

        const remaining = this.ALL_CHARS.split('').filter(c => !L3.includes(c));
        const L6 = remaining.slice(0, 64).join('');
        const L9 = remaining.slice(64).join('');

        // 2. Initialize Binary Buffer (Header: 8 bytes Lattice + 4 bytes BitCount)
        let bufSize = Math.ceil(input.length * 1.2) + 12; 
        let state = {
            buffer: new Uint8Array(bufSize),
            byteIdx: 12,
            bitOffset: 0,
            totalBits: 0
        };

        // Write L3 Lattice to Header
        for (let i = 0; i < 8; i++) state.buffer[i] = L3.charCodeAt(i);

        // 3. Multi-Layer Encoding
        for (let char of input) {
            let i3 = L3.indexOf(char);
            if (i3 !== -1) {
                this._writeBits(0b01, 2, state);
                this._writeBits(i3, 3, state);
            } else {
                let i6 = L6.indexOf(char);
                if (i6 !== -1) {
                    this._writeBits(0b10, 2, state);
                    this._writeBits(i6, 6, state);
                } else {
                    let i9 = L9.indexOf(char);
                    let finalIdx = (i9 === -1) ? 0 : i9;
                    this._writeBits(0b11, 2, state);
                    this._writeBits(finalIdx, 7, state);
                }
            }
        }

        // 4. Finalize Header: Write 32-bit Bit-Stop Count
        const view = new DataView(state.buffer.buffer);
        view.setUint32(8, state.totalBits, false);

        return state.buffer.slice(0, state.byteIdx + (state.bitOffset > 0 ? 1 : 0));
    }

    decompress(data) {
        if (data.length < 12) return "";
        
        // Read Header
        const L3 = String.fromCharCode(...data.slice(0, 8));
        const view = new DataView(data.buffer, data.byteOffset, data.byteLength);
        const totalBits = view.getUint32(8, false);

        const remaining = this.ALL_CHARS.split('').filter(c => !L3.includes(c));
        const L6 = remaining.slice(0, 64).join('');
        const L9 = remaining.slice(64).join('');

        let result = "";
        let bitsRead = 0;
        let bitPos = 96; // Start after 12-byte header

        const readBit = () => {
            let b = (data[Math.floor(bitPos / 8)] >> (7 - (bitPos % 8))) & 1;
            bitPos++;
            bitsRead++;
            return b;
        };

        while (bitsRead < totalBits) {
            let p1 = readBit();
            let p2 = readBit();
            if (!p1 && p2) { // 01
                let delta = 0;
                for(let i=0; i<3; i++) if(readBit()) delta |= (1 << (2-i));
                result += L3[delta];
            } else if (p1 && !p2) { // 10
                let delta = 0;
                for(let i=0; i<6; i++) if(readBit()) delta |= (1 << (5-i));
                result += L6[delta];
            } else if (p1 && p2) { // 11
                let delta = 0;
                for(let i=0; i<7; i++) if(readBit()) delta |= (1 << (6-i));
                result += L9[delta];
            }
        }
        return result;
    }
}

if (typeof module !== 'undefined') module.exports = Vortex369;