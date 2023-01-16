#pragma once

#include <stdint.h>
#include <stdio.h>

struct VGMSTREAM;

typedef int16_t sample_t;

// VGMStream description in structure format
typedef struct {
    int sample_rate;
    int channels;
    struct mixing_info {
        int input_channels;
        int output_channels;
    } mixing_info;
    int channel_layout;
    struct loop_info {
        int start;
        int end;
    } loop_info;
    size_t num_samples;
    char encoding[128];
    char layout[128];
    struct interleave_info {
        int value;
        int first_block;
        int last_block;
    } interleave_info;
    int frame_size;
    char metadata[128];
    int bitrate;
    struct stream_info {
        int current;
        int total;
        char name[128];
    } stream_info;
} vgmstream_info;

/* 64-bit offset is needed for banks that hit +2.5GB (like .fsb or .ktsl2stbin).
 * Leave as typedef to toggle since it's theoretically slower when compiled as 32-bit.
 * ATM it's only used in choice places until more performance tests are done.
 * uint32_t could be an option but needs to test when/how neg offsets are used.
 *
 * On POSIX 32-bit off_t can become off64_t by passing -D_FILE_OFFSET_BITS=64,
 * but not on MSVC as it doesn't have proper POSIX support, so a custom type is needed.
 * fseeks/tells also need to be adjusted for 64-bit support.
 */
typedef int64_t offv_t; //off64_t
//typedef int64_t sizev_t; // size_t int64_t off64_t


/* struct representing a file with callbacks. Code should use STREAMFILEs and not std C functions
 * to do file operations, as plugins may need to provide their own callbacks.
 * Reads from arbitrary offsets, meaning internally may need fseek equivalents during reads. */
typedef struct _STREAMFILE {
    /* read 'length' data at 'offset' to 'dst' */
    size_t (*read)(struct _STREAMFILE* sf, uint8_t* dst, offv_t offset, size_t length);

    /* get max offset */
    size_t (*get_size)(struct _STREAMFILE* sf);

    //todo: DO NOT USE, NOT RESET PROPERLY (remove?)
    offv_t (*get_offset)(struct _STREAMFILE* sf);

    /* copy current filename to name buf */
    void (*get_name)(struct _STREAMFILE* sf, char* name, size_t name_size);

    /* open another streamfile from filename */
    struct _STREAMFILE* (*open)(struct _STREAMFILE* sf, const char* const filename, size_t buf_size);

    /* free current STREAMFILE */
    void (*close)(struct _STREAMFILE* sf);

    /* Substream selection for formats with subsongs.
     * Not ideal here, but it was the simplest way to pass to all init_vgmstream_x functions. */
    int stream_index; /* 0=default/auto (first), 1=first, N=Nth */

} STREAMFILE;


#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------*/
/* vgmstream "public" API                                                   */
/* -------------------------------------------------------------------------*/

/* do format detection, return pointer to a usable VGMSTREAM, or NULL on failure */
VGMSTREAM* init_vgmstream(const char* const filename);

/* init with custom IO via streamfile */
VGMSTREAM* init_vgmstream_from_STREAMFILE(STREAMFILE* sf);

/* reset a VGMSTREAM to start of stream */
void reset_vgmstream(VGMSTREAM* vgmstream);

/* close an open vgmstream */
void close_vgmstream(VGMSTREAM* vgmstream);

/* calculate the number of samples to be played based on looping parameters */
int32_t get_vgmstream_play_samples(double looptimes, double fadeseconds, double fadedelayseconds, VGMSTREAM* vgmstream);

/* Decode data into sample buffer. Returns < sample_count on stream end */
int render_vgmstream(sample_t* buffer, int32_t sample_count, VGMSTREAM* vgmstream);

/* Seek to sample position (next render starts from that point). Use only after config is set (vgmstream_apply_config) */
void seek_vgmstream(VGMSTREAM* vgmstream, int32_t seek_sample);

/* Write a description of the stream into array pointed by desc, which must be length bytes long.
 * Will always be null-terminated if length > 0 */
void describe_vgmstream(VGMSTREAM* vgmstream, char* desc, int length);
void describe_vgmstream_info(VGMSTREAM* vgmstream, vgmstream_info* desc);

/* Return the average bitrate in bps of all unique files contained within this stream. */
int get_vgmstream_average_bitrate(VGMSTREAM* vgmstream);

/* List supported formats and return elements in the list, for plugins that need to know.
 * The list disables some common formats that may conflict (.wav, .ogg, etc). */
const char** vgmstream_get_formats(size_t* size);

/* same, but for common-but-disabled formats in the above list. */
const char** vgmstream_get_common_formats(size_t* size);

/* Force enable/disable internal looping. Should be done before playing anything (or after reset),
 * and not all codecs support arbitrary loop values ATM. */
void vgmstream_force_loop(VGMSTREAM* vgmstream, int loop_flag, int loop_start_sample, int loop_end_sample);

/* Set number of max loops to do, then play up to stream end (for songs with proper endings) */
void vgmstream_set_loop_target(VGMSTREAM* vgmstream, int loop_target);

/* Return 1 if vgmstream detects from the filename that said file can be used even if doesn't physically exist */
int vgmstream_is_virtual_filename(const char* filename);

/* -------------------------------------------------------------------------*/
/* vgmstream "private" API                                                  */
/* -------------------------------------------------------------------------*/

/* Allocate initial memory for the VGMSTREAM */
VGMSTREAM* allocate_vgmstream(int channel_count, int looped);

/* Prepare the VGMSTREAM's initial state once parsed and ready, but before playing. */
void setup_vgmstream(VGMSTREAM* vgmstream);

/* Open the stream for reading at offset (taking into account layouts, channels and so on).
 * Returns 0 on failure */
int vgmstream_open_stream(VGMSTREAM* vgmstream, STREAMFILE* sf, off_t start_offset);
int vgmstream_open_stream_bf(VGMSTREAM* vgmstream, STREAMFILE* sf, off_t start_offset, int force_multibuffer);

/* Get description info */
void get_vgmstream_coding_description(VGMSTREAM* vgmstream, char* out, size_t out_size);
void get_vgmstream_layout_description(VGMSTREAM* vgmstream, char* out, size_t out_size);
void get_vgmstream_meta_description(VGMSTREAM* vgmstream, char* out, size_t out_size);

void setup_state_vgmstream(VGMSTREAM* vgmstream);

#ifdef __cplusplus
}
#endif
