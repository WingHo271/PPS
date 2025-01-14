#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

typedef struct {
    int64_t no_of_frames;
    uint8_t channels;
    uint8_t height;
    uint8_t width;
    uint8_t *frames;
} Video;

Video *read_binary_file(const char *file_path);
void write_binary_file(const char *file_path, Video *video);
void reverse(Video *video);
void swap_channel(Video *video, uint8_t ch1, uint8_t ch2);
void clip_channel(Video *video, uint8_t channel,
 uint8_t min_val, uint8_t max_val);
void scale_channel(Video *video, uint8_t channel, float scale_factor);
void free_video(Video *video);
#endif  // VIDEO_H
