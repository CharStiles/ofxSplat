#pragma once

#include <string>
#include <vector>
#include "ofMain.h" // Include openFrameworks header here if needed

namespace dataset {

    struct Splat {
        // Must match splat buffer in `shaders/shader.vs`
        // Must be aligned according to std430 layout.
        alignas(16) float center[3];
        alignas(4)  float alpha;
        alignas(16) float covA[3];
        alignas(16) float covB[3];
        alignas(16) float sh[16][4]; // vec4 for alignment, TODO: wasteful
    };

    using SplatBuffer = std::vector<Splat>;

    struct SortResult {
        void reset(size_t num_vertices) {
            depth_index.resize(num_vertices);

            // Scratch space
            depths.resize(num_vertices);
            sizes.resize(num_vertices);
            counts0.fill(0);
            starts0.fill(0);
        }

        size_t num_vertices() const {
            return depth_index.size();
        }

        std::vector<uint32_t> depth_index;

        // Scratch space
        std::vector<float> depths;
        std::vector<int32_t> sizes;
        std::array<float, 256 * 256> counts0;
        std::array<float, 256 * 256> starts0;
    };

    class Dataset {
    public:
        Dataset(SplatBuffer&& buffer) : buffer_(buffer) {}
        const SplatBuffer& buffer() const { return buffer_; }
        void sort(const ofMatrix4x4& P, SortResult* out) const;
    private:
        SplatBuffer buffer_;
    };
    Dataset from_ply(const std::string& filename);
}
