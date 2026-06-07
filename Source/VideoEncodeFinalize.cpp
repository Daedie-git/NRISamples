// © 2026 NVIDIA Corporation

#include "NRI.h"
#include "Extensions/NRIVideo.h"

#include <cinttypes>
#include <cstdint>
#include <cstdio>

uint64_t GetVideoEncodeEndOfStreamAppendOffset(const nri::VideoBitstreamRange& dstBitstream, const nri::VideoEncodeFeedback& feedback) {
    return dstBitstream.offset + feedback.encodedBitstreamOffset + feedback.encodedBitstreamWrittenBytes;
}

void RecordVideoEncodeEndOfStreamFinalize(nri::VideoInterface& video, nri::CommandBuffer& commandBuffer, nri::Buffer& bitstream, nri::Buffer& eosUploadBuffer,
    const nri::VideoEncodeFeedback& feedback) {
    constexpr uint64_t bitstreamOffset = 4096;
    constexpr uint64_t bitstreamSize = 2 * 1024 * 1024;

    const nri::VideoEncodeEndOfStreamFinalizeDesc finalizeDesc = {
        .codec = nri::VideoCodec::H264,
        .dstBitstream = {.buffer = &bitstream, .offset = bitstreamOffset, .size = bitstreamSize - bitstreamOffset},
        .feedback = &feedback,
        .eosUploadBuffer = &eosUploadBuffer,
        .eosUploadOffset = 0,
    };

    video.CmdFinalizeVideoEncodeEndOfStream(commandBuffer, finalizeDesc);
}

int main() {
    const nri::VideoBitstreamRange dstBitstream = {
        .buffer = nullptr,
        .offset = 4096,
        .size = 2 * 1024 * 1024 - 4096,
    };
    const nri::VideoEncodeFeedback feedback = {
        .encodedBitstreamWrittenBytes = 123456,
        .writtenSubregionNum = 1,
        .encodedBitstreamOffset = 256,
    };

    std::printf("Finalize Annex-B EOS/EOB at byte offset %" PRIu64 " after CmdEncodeVideo + feedback resolve.\n",
        GetVideoEncodeEndOfStreamAppendOffset(dstBitstream, feedback));
    return 0;
}
