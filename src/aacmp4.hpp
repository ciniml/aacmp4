// SPDX-License-Identifier: BSL-1.0
// Copyright Kenta Ida 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>
#include <vector>
#include <cstring>

#include "primitive_types.hpp"

namespace AACMP4 {
    template<typename T, std::size_t N>
    struct array_adapter {
        using SelfType = array_adapter<T, N>;
        T(&ref)[N];
        constexpr array_adapter(T(&ref)[N]) : ref(ref) {}

        constexpr std::size_t size(void) const { return N; }
        constexpr T& operator[](std::size_t i) { return this->ref[i]; }
        constexpr const T& operator[](std::size_t i) const { return this->ref[i]; }

        constexpr T* begin(void) { return this->ref; }
        constexpr const T* begin(void) const { return this->ref; }
        constexpr T* end(void) { return this->ref + N; }
        constexpr const T* end(void) const { return this->ref + N; }

        constexpr T* data(void) { return this->ref; }
        constexpr const T* data(void) const { return this->ref; }

        constexpr SelfType& operator=(const std::initializer_list<T> other) {
            std::copy(other.begin(), other.end(), this->begin());
            return *this;
        }
    };
    
    struct __attribute__((packed)) BoxType {
        std::uint8_t octets[4];

        constexpr BoxType() : octets {0, 0, 0, 0} {}
        constexpr BoxType(const char* s) : octets {std::uint8_t(s[0]), std::uint8_t(s[1]), std::uint8_t(s[2]), std::uint8_t(s[3])} {}
        constexpr BoxType(const BoxType&) = default;

        constexpr bool operator==(const BoxType& other) const {
            return this->octets[0] == other.octets[0]
                && this->octets[1] == other.octets[1]
                && this->octets[2] == other.octets[2]
                && this->octets[3] == other.octets[3];
        }
        constexpr bool operator!=(const BoxType& other) const {
            return !this->operator==(other);
        }
    };
    
    typedef u8 Version;
    typedef u24 Flags;
    typedef u32 Timestamp;

    template<typename S, typename T>
    static void write(S& stream, const T& value) {
        value.write(stream);
    }

    template<typename S, typename T>
    static void write(S& stream, const T* value, std::size_t size) {
        stream.write(value, size);
    }

    template<typename S>
    static void write(S& stream, const std::vector<u8>& value) {
        stream.write(value.data(), value.size());
    }

    template<typename S>
    static void write(S& stream, u8 value) {
        stream.write(&value, 1);
    }
    template<typename S>
    static void write(S& stream, u16 value) {
        stream.write(value.octets, 2);
    }
    template<typename S>
    static void write(S& stream, u24 value) {
        stream.write(value.octets, 3);
    }
    template<typename S>
    static void write(S& stream, u32 value) {
        stream.write(value.octets, 4);
    }
    template<typename S>
    static void write(S& stream, u64 value) {
        stream.write(value.octets, 8);
    }
    template<typename S>
    static void write(S& stream, BoxType value) {
        stream.write(value.octets, 4);
    }

    struct __attribute__((packed)) AtomHeader {
        u32 size;
        BoxType type;

        AtomHeader() = default;

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->size);
            AACMP4::write(stream, this->type);
        }
    };

    template<typename T>
    struct __attribute__((packed)) Matrix {
        T values[9];
        constexpr Matrix() : values{0x10000, 0x0, 0x0, 0x0, 0x10000, 0x0, 0x0, 0x0, 0x40000000} {}
        constexpr Matrix(const T values[9]) : values(values) {}

        template<typename S> void write(S& stream) const {
            for(std::size_t i = 0; i < 9; i++) {
                AACMP4::write(stream, this->values[i]);
            }
        }
    };

    template<typename T>
    static void default_compute(T& self) {
        self.header.type = T::TYPE;
        self.header.size = sizeof(T);
    }

    struct __attribute__((packed)) MvhdAtom {
        AtomHeader header;
        Version version;
        Flags flags;
        u32 creation_time;
        u32 modification_time;
        u32 timescale;
        u32 duration;
        u32 rate;
        u16 volume;
        u8 reserved[10];
        Matrix<u32> matrix;
        u32 preview_time;
        u32 preview_duration;
        u32 poster_time;
        u32 selection_time;
        u32 selection_duration;
        u32 current_time;
        u32 next_track_id;

        static constexpr const char* TYPE = "mvhd";
        void compute(void) { default_compute(*this); }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->creation_time);
            AACMP4::write(stream, this->modification_time);
            AACMP4::write(stream, this->timescale);
            AACMP4::write(stream, this->duration);
            AACMP4::write(stream, this->rate);
            AACMP4::write(stream, this->volume);
            AACMP4::write(stream, this->reserved, sizeof(this->reserved));
            AACMP4::write(stream, this->matrix);
            AACMP4::write(stream, this->preview_time);
            AACMP4::write(stream, this->preview_duration);
            AACMP4::write(stream, this->poster_time);
            AACMP4::write(stream, this->selection_time);
            AACMP4::write(stream, this->selection_duration);
            AACMP4::write(stream, this->current_time);
            AACMP4::write(stream, this->next_track_id);
        }
    };

    struct __attribute__((packed)) TkhdAtom {
        AtomHeader header;
        Version version;
        Flags flags;
        u32 creation_time;
        u32 modification_time;
        u32 track_id;
        u32 reserved_0;
        u32 duration;
        u32 reserved_1[2];
        u16 layer;
        u16 alternate_group;
        u16 volume;
        u16 reserved_2;
        Matrix<u32> matrix;
        u32 width;
        u32 height;

        static constexpr const char* TYPE = "tkhd";
        void compute(void) { default_compute(*this); }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->creation_time);
            AACMP4::write(stream, this->modification_time);
            AACMP4::write(stream, this->track_id);
            AACMP4::write(stream, this->reserved_0);
            AACMP4::write(stream, this->duration);
            AACMP4::write(stream, this->reserved_1[0]);
            AACMP4::write(stream, this->reserved_1[1]);
            AACMP4::write(stream, this->layer);
            AACMP4::write(stream, this->alternate_group);
            AACMP4::write(stream, this->volume);
            AACMP4::write(stream, this->reserved_2);
            AACMP4::write(stream, this->matrix);
            AACMP4::write(stream, this->width);
            AACMP4::write(stream, this->height);
        }
    };

    struct __attribute__((packed)) ElstAtom {
        struct __attribute__((packed)) ElstEntry {
            u32 segment_duration;
            u32 media_time;
            u32 media_rate;

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->segment_duration);
                AACMP4::write(stream, this->media_time);
                AACMP4::write(stream, this->media_rate);
            }
        };

        AtomHeader header;
        Version version;
        Flags flags;
        u32 entry_count;
        ElstEntry entries[1];

        static constexpr const char* TYPE = "elst";
        void compute(void) { default_compute(*this); }

        template <typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->entry_count);
            for(std::size_t i = 0; i < this->entry_count; i++) {
                AACMP4::write(stream, this->entries[i]);
            }
        }
    };

    struct __attribute__((packed)) EdtsBox {
        AtomHeader header;
        ElstAtom elst;

        static constexpr const char* TYPE = "edts";
        void compute(void) {
            this->elst.compute();
            this->header.size = sizeof(this->header) + this->elst.header.size;
            this->header.type = TYPE;
        }

        template <typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->elst);
        }
    };

    struct __attribute__((packed)) SttsAtom {
        struct __attribute__((packed)) SttsEntry {
            u32 count;
            u32 duration;

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->count);
                AACMP4::write(stream, this->duration);
            }
        };
        AtomHeader header;
        Version version;
        Flags flags;
        u32 number_of_entries;
        SttsEntry entries[2];

        static constexpr const char* TYPE = "stts";
        void compute(void) { 
            this->header.size = sizeof(this->header)
                + sizeof(this->version)
                + sizeof(this->flags)
                + sizeof(this->number_of_entries)
                + this->number_of_entries * sizeof(SttsEntry);
            this->header.type = TYPE;
        }

        template <typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->number_of_entries);
            for(std::size_t i = 0; i < this->number_of_entries; i++) {
                AACMP4::write(stream, this->entries[i]);
            }
        }
    };

    struct __attribute__((packed)) StscAtom {
        struct __attribute__((packed)) StscEntry {
            u32 first_chunk;
            u32 samples_per_chunk;
            u32 sample_description_id;

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->first_chunk);
                AACMP4::write(stream, this->samples_per_chunk);
                AACMP4::write(stream, this->sample_description_id);
            }
        };
        AtomHeader header;
        Version version;
        Flags flags;
        u32 number_of_entries;
        StscEntry entries[1];

        static constexpr const char* TYPE = "stsc";
        void compute(void) {
            this->header.size = sizeof(this->header)
                + sizeof(this->version)
                + sizeof(this->flags)
                + sizeof(this->number_of_entries)
                + this->number_of_entries * sizeof(StscEntry);
            this->header.type = TYPE;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->number_of_entries);
            for(std::size_t i = 0; i < this->number_of_entries; i++) {
                AACMP4::write(stream, this->entries[i]);
            }
        }
    };

    struct __attribute__((packed)) StszAtomHeader {
        AtomHeader header;
        Version version;
        Flags flags;
        u32 sample_size;
        u32 number_of_entries;

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->sample_size);
            AACMP4::write(stream, this->number_of_entries);
        }
    };

    struct StszBox {
        static constexpr const char* TYPE = "stsz";
        StszAtomHeader header;
        std::vector<u32> entries;

        void compute(void) {
            this->header.header.size = sizeof(this->header) + entries.size() * 4;
            this->header.header.type = TYPE;
            this->header.number_of_entries = entries.size();
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            for(std::size_t i = 0; i < this->entries.size(); i++) {
                AACMP4::write(stream, this->entries[i]);
            }
        }
    };

    struct StcoAtom {
        static constexpr const char* TYPE = "stco";
        AtomHeader header;
        Version version;
        Flags flags;
        u32 number_of_entries;
        u32 entries[1];

        void compute(void) {
            default_compute(*this);
            this->number_of_entries = 1;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->number_of_entries);
            AACMP4::write(stream, this->entries[0]);
        }
    };

    struct __attribute__((packed)) MdhdAtom {
        AtomHeader header;
        Version version;
        Flags flags;
        u32 creation_time;
        u32 modification_time;
        u32 timescale;
        u32 duration;
        u16 language;
        u16 quality;

        static constexpr const char* TYPE = "mdhd";
        void compute(void) { default_compute(*this); }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->creation_time);
            AACMP4::write(stream, this->modification_time);
            AACMP4::write(stream, this->timescale);
            AACMP4::write(stream, this->duration);
            AACMP4::write(stream, this->language);
            AACMP4::write(stream, this->quality);
        }
    };
    
    struct __attribute__((packed)) HdlrAtom {
        AtomHeader header;
        Version version;
        Flags flags;
        u32 component_type;
        u32 handler_type;
        u32 reserved[3];
        u8  name[13];
        
        static constexpr const char* TYPE = "hdlr";
        void compute(void) { default_compute(*this); }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->component_type);
            AACMP4::write(stream, this->handler_type);
            AACMP4::write(stream, this->reserved[0]);
            AACMP4::write(stream, this->reserved[1]);
            AACMP4::write(stream, this->reserved[2]);
            AACMP4::write(stream, this->name, sizeof(this->name));
        }
    };
    
    struct __attribute__((packed)) SmhdAtom {
        AtomHeader header;
        u8 reserved[8];
        
        static constexpr const char* TYPE = "smhd";
        void compute(void) { default_compute(*this); }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->reserved, sizeof(this->reserved));
        }
    };

    struct __attribute__((packed)) DrefBox {
        struct __attribute__((packed)) DataEntry {
            AtomHeader header;
            Version version;
            Flags flags;

            void compute(void) { 
                this->header.size = sizeof(*this);
            }
            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->header);
                AACMP4::write(stream, this->version);
                AACMP4::write(stream, this->flags);
            }
        };

        AtomHeader header;
        Version version;
        Flags flags;
        u32 entry_count;
        DataEntry data_entries[1];

        static constexpr const char* TYPE = "dref";
        void compute(void) {
            this->data_entries[0].compute();
            this->header.type = TYPE;
            this->header.size = sizeof(this->header) + sizeof(this->version) + sizeof(this->flags) + sizeof(this->entry_count) + this->entry_count * sizeof(DataEntry);
            this->entry_count = 1;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->flags);
            AACMP4::write(stream, this->entry_count);
            for(std::size_t i = 0; i < this->entry_count; i++) {
                AACMP4::write(stream, this->data_entries[i]);
            }
        }
    };


    struct __attribute__((packed)) DinfBox {
        AtomHeader header;
        DrefBox dref;

        static constexpr const char* TYPE = "dinf";
        void compute(void) { 
            this->dref.compute();
            this->header.size = sizeof(this->header) + this->dref.header.size;
            this->header.type = TYPE;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->dref);
        }
    };

    // MPEG-4 elementary stream descriptor atom 
    // https://developer.apple.com/documentation/quicktime-file-format/mpeg-4_elementary_sound_stream_descriptor_atom
    struct __attribute__((packed)) EsdsAtom {
        static constexpr u8 TAG_ES_DESCRIPTOR = 0x03;
        static constexpr u8 TAG_DECODER_CONFIG = 0x04;
        static constexpr u8 TAG_DECODER_SPECIFIC = 0x05;
        static constexpr u8 TAG_SL_CONFIG_DESCRIPTOR = 0x06;
        struct __attribute__((packed)) SLConfigDescriptor {
            u8 tag;
            u8 size[4];
            u8 predefined;

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->tag);
                AACMP4::write(stream, this->size, sizeof(this->size));
                AACMP4::write(stream, this->predefined);
            }
        };
        struct __attribute__((packed)) DecoderSpecificInfo {
            u8 tag;
            u8 size[4];
            u8 specific[5];

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->tag);
                AACMP4::write(stream, this->size, sizeof(this->size));
                AACMP4::write(stream, this->specific, sizeof(this->specific));
            }
        };
        struct __attribute__((packed)) DecoderConfiguration {
            u8 tag;
            u8 size[4];
            u8 object_type;
            u8 flags;
            u24 buffer_size;
            u32 max_bit_rate;
            u32 average_bit_rate;
            DecoderSpecificInfo decoder_specific;

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->tag);
                AACMP4::write(stream, this->size, sizeof(this->size));
                AACMP4::write(stream, this->object_type);
                AACMP4::write(stream, this->flags);
                AACMP4::write(stream, this->buffer_size);
                AACMP4::write(stream, this->max_bit_rate);
                AACMP4::write(stream, this->average_bit_rate);
                AACMP4::write(stream, this->decoder_specific);
            }
        };
        struct __attribute__((packed)) ESDescriptor {
            u8 tag;
            u8 size[4];
            u16 es_id;
            u8 flags;
            DecoderConfiguration decoder_config;
            SLConfigDescriptor sl_config;

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->tag);
                AACMP4::write(stream, this->size, sizeof(this->size));
                AACMP4::write(stream, this->es_id);
                AACMP4::write(stream, this->flags);
                AACMP4::write(stream, this->decoder_config);
                AACMP4::write(stream, this->sl_config);
            }
        };
        AtomHeader header;
        u32 version;
        ESDescriptor desc;

        static constexpr const char* TYPE = "esds";
        void compute(void) { default_compute(*this); }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->version);
            AACMP4::write(stream, this->desc);
        }
    };

    struct __attribute__((packed)) BtrtAtom {
        AtomHeader header;
        u32 buffer_size;
        u32 max_bit_rate;
        u32 average_bit_rate;

        static constexpr const char* TYPE = "btrt";
        void compute(void) { default_compute(*this); }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->buffer_size);
            AACMP4::write(stream, this->max_bit_rate);
            AACMP4::write(stream, this->average_bit_rate);
        }
    };

    struct StsdBox {
        // Sound Description V0
        // https://developer.apple.com/documentation/quicktime-file-format/sound_sample_description_version_0
        struct __attribute__((packed)) SampleDescriptionEntryHeader {
            AtomHeader header;
            u8 reserved[6];
            u16 data_reference_index;
            u16 version;
            u16 revision_level;
            u32 vendor;
            u16 number_of_channels;
            u16 sample_size;
            u16 compression_id;
            u16 packet_size;
            u32 sample_rate;

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->header);
                AACMP4::write(stream, this->reserved, sizeof(this->reserved));
                AACMP4::write(stream, this->data_reference_index);
                AACMP4::write(stream, this->version);
                AACMP4::write(stream, this->revision_level);
                AACMP4::write(stream, this->vendor);
                AACMP4::write(stream, this->number_of_channels);
                AACMP4::write(stream, this->sample_size);
                AACMP4::write(stream, this->compression_id);
                AACMP4::write(stream, this->packet_size);
                AACMP4::write(stream, this->sample_rate);
            }
        };
        struct __attribute__((packed)) SampleDescriptionEntry {
            SampleDescriptionEntryHeader header;
            EsdsAtom esds;
            BtrtAtom btrt;

            static constexpr const char* TYPE = "mp4a";
            void compute(void) { 
                this->esds.compute();
                this->btrt.compute();
                this->header.header.size = sizeof(this->header) + this->esds.header.size + this->btrt.header.size;
                this->header.header.type = TYPE;
                std::fill(this->header.reserved, this->header.reserved + sizeof(this->header.reserved), 0);
            }

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->header);
                AACMP4::write(stream, this->esds);
                AACMP4::write(stream, this->btrt);
            }
        };
        struct __attribute__((packed)) StsdHeader {
            AtomHeader header;
            Version version;
            Flags flags;
            u32 entry_count;

            template<typename S> void write(S& stream) const {
                AACMP4::write(stream, this->header);
                AACMP4::write(stream, this->version);
                AACMP4::write(stream, this->flags);
                AACMP4::write(stream, this->entry_count);
            }
        };
        StsdHeader header;
        std::vector<SampleDescriptionEntry> sample_description_entries;

        static constexpr const char* TYPE = "stsd";
        void compute(void) {
            std::size_t size = sizeof(this->header);
            for(auto& entry : this->sample_description_entries) {
                entry.compute();
                size += entry.header.header.size;
            }
            this->header.header.size = size;
            this->header.header.type = TYPE; 
            this->header.entry_count = this->sample_description_entries.size();
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            for(const auto& entry : this->sample_description_entries) {
                AACMP4::write(stream, entry);
            }
        }
    };

    struct StblBox {
        AtomHeader header;
        StsdBox stsd;
        SttsAtom stts;
        StscAtom stsc;
        StszBox stsz;
        StcoAtom stco;

        static constexpr const char* TYPE = "stbl";
        void compute(void) { 
            this->stsd.compute();
            this->stts.compute();
            this->stsc.compute();
            this->stsz.compute();
            this->stco.compute();
            this->header.size = sizeof(this->header) 
                + this->stsd.header.header.size 
                + this->stts.header.size 
                + this->stsc.header.size 
                + this->stsz.header.header.size
                + this->stco.header.size;
            this->header.type = TYPE;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->stsd);
            AACMP4::write(stream, this->stts);
            AACMP4::write(stream, this->stsc);
            AACMP4::write(stream, this->stsz);
            AACMP4::write(stream, this->stco);
        }
    };

    struct MinfBox {
        AtomHeader header;
        SmhdAtom smhd;
        DinfBox dinf;
        StblBox stbl;

        static constexpr const char* TYPE = "minf";
        void compute(void) {
            this->smhd.compute();
            this->dinf.compute();
            this->stbl.compute();
            this->header.size = sizeof(this->header) + this->smhd.header.size + this->dinf.header.size + this->stbl.header.size;
            this->header.type = TYPE;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->smhd);
            AACMP4::write(stream, this->dinf);
            AACMP4::write(stream, this->stbl);
        }
    };

    struct MdiaBox {
        AtomHeader header;
        MdhdAtom mdhd;
        HdlrAtom hdlr;
        MinfBox minf;

        static constexpr const char* TYPE = "mdia";
        void compute(void) {
            this->mdhd.compute();
            this->hdlr.compute();
            this->minf.compute();
            this->header.size = sizeof(this->header) 
                + this->mdhd.header.size 
                + this->hdlr.header.size 
                + this->minf.header.size;
            this->header.type = TYPE;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->mdhd);
            AACMP4::write(stream, this->hdlr);
            AACMP4::write(stream, this->minf);
        }
    };

    struct TrakBox {
        AtomHeader header;
        TkhdAtom tkhd;
        EdtsBox edts;
        MdiaBox mdia;

        static constexpr const char* TYPE = "trak";
        void compute(void) {
            this->tkhd.compute();
            this->edts.compute();
            this->mdia.compute();
            this->header.size = sizeof(this->header) 
                + this->tkhd.header.size 
                + this->edts.header.size 
                + this->mdia.header.size;
            this->header.type = TYPE;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->tkhd);
            AACMP4::write(stream, this->edts);
            AACMP4::write(stream, this->mdia);
        }
    };

    struct MoovBox {
        AtomHeader header;
        MvhdAtom mvhd;
        TrakBox trak;

        static constexpr const char* TYPE = "moov";
        void compute(void) {
            this->mvhd.compute();
            this->trak.compute();
            this->header.size = sizeof(this->header) 
                + this->mvhd.header.size 
                + this->trak.header.size;
            this->header.type = TYPE;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->mvhd);
            AACMP4::write(stream, this->trak);
        }
    };

    struct RefMdatBox {
        AtomHeader header;
        const std::vector<u8>& data;

        RefMdatBox(const std::vector<u8>& data) : data(data) {}

        static constexpr const char* TYPE = "mdat";
        void compute(void) {
            this->header.size = sizeof(this->header) + this->data.size();
            this->header.type = TYPE;
        }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header);
            AACMP4::write(stream, this->data);
        }
    };

    struct FtypAtom {
        AtomHeader header;
        BoxType major_brand;
        u32 minor_version;
        BoxType compatible_brands[2];

        static constexpr const char* TYPE = "ftyp";
        void compute(void) { default_compute(*this); }

        template<typename S> void write(S& stream) const {
            AACMP4::write(stream, this->header); 
            AACMP4::write(stream, this->major_brand);
            AACMP4::write(stream, this->minor_version);
            AACMP4::write(stream, this->compatible_brands[0]);
            AACMP4::write(stream, this->compatible_brands[1]);
        }
    };

    struct DummyWriter {
        std::size_t bytes_written = 0;
        void write(const u8*, std::size_t size) {
            this->bytes_written += size;
        }
    };

    template<typename S>
    static void write_aac_mp4(S& stream, const std::vector<u32>& chunks, const std::vector<u8>& data, std::uint32_t sample_rate, std::uint32_t number_of_samples, std::uint32_t max_samples_per_chunk) {
        FtypAtom ftyp;
        ftyp.major_brand = "isom";
        ftyp.minor_version = 0x00000200;
        ftyp.compatible_brands[0] = "isom";
        ftyp.compatible_brands[1] = "mp41";
        ftyp.compute();
        write(stream, ftyp);

        MoovBox moov;
        // mvhd
        moov.mvhd.version = 0;
        moov.mvhd.flags = 0;
        moov.mvhd.creation_time = 0;
        moov.mvhd.modification_time = 0;
        moov.mvhd.timescale = 1000;
        moov.mvhd.duration = number_of_samples * 1000 / sample_rate;
        moov.mvhd.rate = 0x00010000;
        moov.mvhd.volume = 0x0100;
        std::fill(moov.mvhd.reserved, moov.mvhd.reserved + sizeof(moov.mvhd.reserved), 0);
        moov.mvhd.matrix = Matrix<u32>();
        moov.mvhd.preview_time = 0;
        moov.mvhd.preview_duration = 0;
        moov.mvhd.poster_time = 0;
        moov.mvhd.selection_time = 0;
        moov.mvhd.selection_duration = 0;
        moov.mvhd.current_time = 0;
        moov.mvhd.next_track_id = 2;

        // trak/tkhd
        moov.trak.tkhd.version = 0;
        moov.trak.tkhd.flags = 0x0003;
        moov.trak.tkhd.creation_time = 0;
        moov.trak.tkhd.modification_time = 0;
        moov.trak.tkhd.track_id = 1;
        moov.trak.tkhd.reserved_0 = 0;
        moov.trak.tkhd.duration = number_of_samples * 1000 / sample_rate;
        std::fill(moov.trak.tkhd.reserved_1, moov.trak.tkhd.reserved_1 + sizeof(moov.trak.tkhd.reserved_1), 0);
        moov.trak.tkhd.layer = 0;
        moov.trak.tkhd.alternate_group = 1;
        moov.trak.tkhd.volume = 0x0100;
        moov.trak.tkhd.reserved_2 = 0;
        moov.trak.tkhd.matrix = Matrix<u32>();
        moov.trak.tkhd.width = 0;
        moov.trak.tkhd.height = 0;
        // trak/edts/elst
        moov.trak.edts.elst.version = 0;
        moov.trak.edts.elst.flags = 0;
        moov.trak.edts.elst.entry_count = 1;
        moov.trak.edts.elst.entries[0].segment_duration = number_of_samples * 1000 / sample_rate;
        moov.trak.edts.elst.entries[0].media_time = 0x00000800;
        moov.trak.edts.elst.entries[0].media_rate = 0x00010000;

        // trak/mdia/mdhd
        moov.trak.mdia.mdhd.version = 0;
        moov.trak.mdia.mdhd.flags = 0;
        moov.trak.mdia.mdhd.creation_time = 0;
        moov.trak.mdia.mdhd.modification_time = 0;
        moov.trak.mdia.mdhd.timescale = sample_rate;
        moov.trak.mdia.mdhd.duration = number_of_samples;
        moov.trak.mdia.mdhd.language = 0x55c4;
        moov.trak.mdia.mdhd.quality = 0;
        // trak/mdia/hdlr
        moov.trak.mdia.hdlr.version = 0;
        moov.trak.mdia.hdlr.flags = 0;
        moov.trak.mdia.hdlr.component_type = 0;
        moov.trak.mdia.hdlr.handler_type = 0x736F756E;
        std::fill(moov.trak.mdia.hdlr.reserved, moov.trak.mdia.hdlr.reserved + sizeof(moov.trak.mdia.hdlr.reserved), 0);
        std::memcpy(moov.trak.mdia.hdlr.name, "SoundHandler", 13);
        // trak/mdia/minf
        std::fill(moov.trak.mdia.minf.smhd.reserved, moov.trak.mdia.minf.smhd.reserved + sizeof(moov.trak.mdia.minf.smhd.reserved), 0);
        moov.trak.mdia.minf.dinf.dref.version = 0;
        moov.trak.mdia.minf.dinf.dref.flags = 0;
        moov.trak.mdia.minf.dinf.dref.entry_count = 1;
        moov.trak.mdia.minf.dinf.dref.data_entries[0].header.type = "url ";
        moov.trak.mdia.minf.dinf.dref.data_entries[0].version = 1;
        moov.trak.mdia.minf.dinf.dref.data_entries[0].flags = 0;

        StsdBox::SampleDescriptionEntry sd;
        sd.header.data_reference_index = 1;
        sd.header.version = 0;
        sd.header.revision_level = 0;
        sd.header.vendor = 0;
        sd.header.number_of_channels = 1;
        sd.header.sample_size = 16;
        sd.header.compression_id = 0;
        sd.header.packet_size = 0;
        sd.header.sample_rate = std::uint32_t(sample_rate << 16);
        sd.esds.version = 0;
        sd.esds.desc.tag = EsdsAtom::TAG_ES_DESCRIPTOR;
        AACMP4::array_adapter(sd.esds.desc.size) = {0x80, 0x80, 0x80, 0x25};    // 37 bytes
        sd.esds.desc.es_id = 1;
        sd.esds.desc.decoder_config.tag = EsdsAtom::TAG_DECODER_CONFIG;
        AACMP4::array_adapter(sd.esds.desc.decoder_config.size) = {0x80, 0x80, 0x80, 0x17}; // 23 bytes
        sd.esds.desc.decoder_config.object_type = 0x40; // MPEG-4 AAC LC
        sd.esds.desc.decoder_config.flags = 0x15;
        sd.esds.desc.decoder_config.buffer_size = 0;
        sd.esds.desc.decoder_config.max_bit_rate = 69000;
        sd.esds.desc.decoder_config.average_bit_rate = 58223;
        sd.esds.desc.decoder_config.decoder_specific.tag = EsdsAtom::TAG_DECODER_SPECIFIC;
        AACMP4::array_adapter(sd.esds.desc.decoder_config.decoder_specific.size) = {0x80, 0x80, 0x80, 0x05}; // 5 bytes
        AACMP4::array_adapter(sd.esds.desc.decoder_config.decoder_specific.specific) = {0x14, 0x08, 0x56, 0xe5, 0x00};
        sd.esds.desc.sl_config.tag = EsdsAtom::TAG_SL_CONFIG_DESCRIPTOR;
        AACMP4::array_adapter(sd.esds.desc.sl_config.size) = {0x80, 0x80, 0x80, 0x01}; // 1 byte
        sd.esds.desc.sl_config.predefined = 0x02;
        sd.btrt.buffer_size = 0;
        sd.btrt.max_bit_rate = 0;
        sd.btrt.average_bit_rate = 0;
        moov.trak.mdia.minf.stbl.stsd.header.flags = 0;
        moov.trak.mdia.minf.stbl.stsd.header.version = 0;
        moov.trak.mdia.minf.stbl.stsd.sample_description_entries.push_back(sd);

        moov.trak.mdia.minf.stbl.stts.version = 0;
        moov.trak.mdia.minf.stbl.stts.flags = 0;
        std::uint32_t remainder_samples = number_of_samples % max_samples_per_chunk;
        moov.trak.mdia.minf.stbl.stts.number_of_entries = remainder_samples == 0 ? 1 : 2;
        moov.trak.mdia.minf.stbl.stts.entries[0].count = number_of_samples / max_samples_per_chunk;
        moov.trak.mdia.minf.stbl.stts.entries[0].duration = max_samples_per_chunk;
        if(remainder_samples != 0) {
            moov.trak.mdia.minf.stbl.stts.entries[1].count = 1;
            moov.trak.mdia.minf.stbl.stts.entries[1].duration = remainder_samples;
        }
        moov.trak.mdia.minf.stbl.stsc.version = 0;
        moov.trak.mdia.minf.stbl.stsc.flags = 0;
        moov.trak.mdia.minf.stbl.stsc.number_of_entries = 1;
        moov.trak.mdia.minf.stbl.stsc.entries[0].first_chunk = 1;
        moov.trak.mdia.minf.stbl.stsc.entries[0].samples_per_chunk = (number_of_samples + max_samples_per_chunk - 1) / max_samples_per_chunk;
        moov.trak.mdia.minf.stbl.stsc.entries[0].sample_description_id = 1;
        moov.trak.mdia.minf.stbl.stsz.header.version = 0;
        moov.trak.mdia.minf.stbl.stsz.header.flags = 0;
        moov.trak.mdia.minf.stbl.stsz.header.sample_size = 0;
        moov.trak.mdia.minf.stbl.stsz.entries = chunks;
        
        moov.trak.mdia.minf.stbl.stco.version = 0;
        moov.trak.mdia.minf.stbl.stco.flags = 0;
        moov.trak.mdia.minf.stbl.stco.number_of_entries = 1;
        moov.trak.mdia.minf.stbl.stco.entries[0] = 0;

        StsdBox::SampleDescriptionEntry mp4a;
        mp4a.btrt.average_bit_rate = 58223;
        mp4a.btrt.max_bit_rate = 69000;
        mp4a.btrt.buffer_size = 0;
        
        moov.trak.mdia.minf.stbl.stsz.entries = chunks;

        moov.compute();
        // Update the chunk offset
        moov.trak.mdia.minf.stbl.stco.entries[0] = ftyp.header.size + moov.header.size + 8;    // ftyp box + moov box + mdat header
        moov.write(stream);

        RefMdatBox mdat(data);
        mdat.compute();
        mdat.write(stream);
    }
} // namespace AACMP4
