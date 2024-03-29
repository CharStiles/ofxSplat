#pragma once

//#include "logging.h"

#include <vector>
#include <string>
//#include <llfio.hpp>
#include "ofMain.h" // Include openFrameworks header here if needed

// Helper for reading memory-mapped PLY files with a single element.

namespace ply {
   // namespace llfio = LLFIO_V2_NAMESPACE;

    enum class PlyType {
        Double,
        Float,
        Int,
        UInt,
        Short,
        UShort,
        Char,
        UChar,
    };

    PlyType ply_type_from_string(const std::string& t);

    size_t ply_type_size(PlyType t);

    struct PlyProperty {
        PlyProperty(const std::string& type_,
                    const std::string& name_)
            : type(ply_type_from_string(type_))
            , name(name_) {}

        PlyType type;
        std::string name;
    };

    struct PlyHeader {
        PlyHeader(const std::string& filename);
        size_t header_end_idx;
        size_t num_vertices;
        std::vector<PlyProperty> props;
        size_t row_length;
        std::vector<size_t> offsets;
        char* address; 
    };

    template <typename T>
    class PlyAccessor {
    public:
        PlyAccessor(char* buf, size_t row_length, size_t offset)
            : buf_(buf), row_length_(row_length), offset_(offset) {}

        PlyAccessor(char* buf, const PlyHeader& h, size_t prop_idx)
            : PlyAccessor(buf, h.row_length, h.offsets.at(prop_idx)) {}

        T operator()(size_t i) const {
            return *reinterpret_cast<T*>(buf_ + offset_ + i * row_length_);
        }

    private:
        char* buf_;
        size_t row_length_;
        size_t offset_;
    };

    class PlyFile {
    public:
        PlyFile(const std::string& filename)
            
            : header_(filename)
            , ply_body_(reinterpret_cast<char*>(header_.address) + header_.header_end_idx)
            {}

        template <typename T>
        PlyAccessor<T> accessor(const std::string& prop_name) {
            const auto it = std::find_if(header_.props.begin(), header_.props.end(),
                                         [&](const PlyProperty& prop) {
                                             return prop_name == prop.name;
                                         });
            if (it == header_.props.end())
                ofLogFatalError() << "not exist:" << prop_name.c_str();
            const size_t idx = std::distance(header_.props.begin(), it);
            if (sizeof(T) != ply_type_size(header_.props.at(idx).type))
                ofLogFatalError() << "invalid accessor type for property:" <<prop_name.c_str();
            return PlyAccessor<T>(ply_body_, header_, idx);
        }

        size_t num_vertices() const { return header_.num_vertices; }

    private:
        //llfio::mapped_file_handle file_;
        PlyHeader header_;
        char* ply_body_;
    };
}
