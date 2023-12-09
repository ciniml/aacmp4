// SPDX-License-Identifier: BSL-1.0
// Copyright Kenta Ida 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <cstring>

#include <fdk-aac/aacenc_lib.h>

#include "aacmp4.hpp"
#include "stream_adapter.hpp"

using namespace std;

int main()
{
    HANDLE_AACENCODER handle;
    auto err = aacEncOpen(&handle, 0x0, 1);
    if(err != AACENC_OK) {
        std::printf("aacEncOpen failed: %d\n", err);
        return 1;
    }
    err = aacEncoder_SetParam(handle, AACENC_AOT, AOT_AAC_LC);
    if(err != AACENC_OK) {
        std::printf("aacEncoder_SetParam(AACENC_AOT) failed: %d\n", err);
        return 1;
    }
    err = aacEncoder_SetParam(handle, AACENC_SAMPLERATE, 16000);
    if(err != AACENC_OK) {
        std::printf("aacEncoder_SetParam(AACENC_SAMPLERATE) failed: %d\n", err);
        return 1;
    }
    err = aacEncoder_SetParam(handle, AACENC_CHANNELMODE, MODE_1);
    if(err != AACENC_OK) {
        std::printf("aacEncoder_SetParam(AACENC_CHANNELMODE) failed: %d\n", err);
        return 1;
    }
    err = aacEncoder_SetParam(handle, AACENC_CHANNELORDER, 1);
    if(err != AACENC_OK) {
        std::printf("aacEncoder_SetParam(AACENC_CHANNELORDER) failed: %d\n", err);
        return 1;
    }
    err = aacEncoder_SetParam(handle, AACENC_BITRATE, 9000);
    if(err != AACENC_OK) {
        std::printf("aacEncoder_SetParam(AACENC_BITRATE) failed: %d\n", err);
        return 1;
    }
    err = aacEncoder_SetParam(handle, AACENC_TRANSMUX, TT_MP4_RAW);
    if(err != AACENC_OK) {
        std::printf("aacEncoder_SetParam(AACENC_TRANSMUX) failed: %d\n", err);
        return 1;
    }
    err = aacEncEncode(handle, nullptr, nullptr, nullptr, nullptr);
    if(err != AACENC_OK) {
        std::printf("aacEncEncode failed: %d\n", err);
        return 1;
    }

    AACENC_InfoStruct info = { 0 };
    err = aacEncInfo(handle, &info);
    if(err != AACENC_OK) {
        std::printf("aacEncInfo failed: %d\n", err);
        return 1;
    }

    auto frame_size = 1*2*info.frameLength;
    std::vector<uint8_t> out_buffer;
    out_buffer.reserve(info.maxOutBufBytes * 1024);
    std::vector<AACMP4::u32> chunks;
    chunks.reserve(1024);

    vector<uint8_t> input;
    ifstream input_file("../../ashita_asatte_16k.wav", ios::binary);
    input_file.seekg(0, ios::end);
    size_t input_size = input_file.tellg();
    input.resize(input_size + frame_size);
    input_file.seekg(0, ios::beg);
    input_file.read(reinterpret_cast<char*>(input.data()), input_size);
    std::printf("input size: %lu\n", input_size);
    fill(input.begin() + input_size, input.end(), 0);
    size_t input_offset = 0;
    
    // Skip to the head of the data chunk
    while(input_offset < input.size()) {
        if(input[input_offset] == 'd' && input[input_offset + 1] == 'a' && input[input_offset + 2] == 't' && input[input_offset + 3] == 'a') {
            input_offset += 8;
            break;
        }
        input_offset++;
    }

    while(input.size() - input_offset >= frame_size) {
        AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
        AACENC_InArgs in_args = { 0 };
        AACENC_OutArgs out_args = { 0 };
        int in_identifier = IN_AUDIO_DATA;
        int in_size = frame_size;
        int in_elem_size = 2;

        in_args.numInSamples = frame_size / 2;
        void* in_ptr = input.data() + input_offset;
        in_buf.numBufs = 1;
        in_buf.bufs = &in_ptr;
        in_buf.bufferIdentifiers = &in_identifier;
        in_buf.bufSizes = &in_size;
        in_buf.bufElSizes = &in_elem_size;

        size_t out_offset = out_buffer.size();
        out_buffer.resize(out_offset + info.maxOutBufBytes);
        void* out_ptr = out_buffer.data() + out_offset;
        int out_size = out_buffer.size() - out_offset;
        int out_elem_size = 1;
        int out_identifier = OUT_BITSTREAM_DATA;
        out_buf.numBufs = 1;
        out_buf.bufs = &out_ptr;
        out_buf.bufferIdentifiers = &out_identifier;
        out_buf.bufSizes = &out_size;
        out_buf.bufElSizes = &out_elem_size;

        err = aacEncEncode(handle, &in_buf, &out_buf, &in_args, &out_args);
        if( err == AACENC_ENCODE_EOF ) {
            break;
        }
        if( err != AACENC_OK ) {
            std::printf("aacEncEncode failed: %d\n", err);
            return 1;
        }
        
        input_offset += in_args.numInSamples * 2;
        if(out_args.numOutBytes == 0) continue;
        printf("%d/%d\n", in_size, out_args.numOutBytes);
        chunks.push_back(out_args.numOutBytes);
        out_buffer.resize(out_offset + out_args.numOutBytes);
        out_offset += out_args.numOutBytes;
    }

    aacEncClose(&handle);

    // write mp4
    ofstream output_file("output.mp4", ios::binary);
    auto adapter = AACMP4::StreamAdapter(output_file);
    AACMP4::write_aac_mp4(adapter, chunks, out_buffer, 16000, input_offset / 2, info.frameLength);

    return 0;
}