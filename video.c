#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "video.h"

Video *read_binary_file(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file for reading");
        return NULL;
    }

    Video *video = malloc(sizeof(Video));
    if (!video) {
        perror("Failed to allocate memory for video");
        fclose(file);
        return NULL;
    }

    fread(&video->no_of_frames, sizeof(int64_t), 1, file);
    fread(&video->channels, sizeof(uint8_t), 1, file);
    fread(&video->height, sizeof(uint8_t), 1, file);
    fread(&video->width, sizeof(uint8_t), 1, file);

    size_t frame_size = video->channels * video->height * video->width;
    video->frames = malloc(frame_size * video->no_of_frames);
    if (!video->frames) {
        perror("Failed to allocate memory for frames");
        free(video);
        fclose(file);
        return NULL;
    }

    fread(video->frames, frame_size, video->no_of_frames, file);
    fclose(file);
    return video;
}

void write_binary_file(const char *file_path, Video *video) {
    FILE *file = fopen(file_path, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    fwrite(&video->no_of_frames, sizeof(int64_t), 1, file);
    fwrite(&video->channels, sizeof(uint8_t), 1, file);
    fwrite(&video->height, sizeof(uint8_t), 1, file);
    fwrite(&video->width, sizeof(uint8_t), 1, file);

    size_t frame_size = video->channels * video->height * video->width;
    fwrite(video->frames, frame_size, video->no_of_frames, file);
    fclose(file);
}

void reverse(Video *video) {
    size_t frame_size = video->channels * video->height * video->width;
    for (int64_t i = 0; i < video->no_of_frames / 2; ++i) {
     uint8_t *frame_start = video->frames + i * frame_size;
     uint8_t *frame_end = video->frames+(video->no_of_frames-1-i)*frame_size;

      for (size_t j = 0; j < frame_size; ++j) {
         uint8_t temp = frame_start[j];
         frame_start[j] = frame_end[j];
         frame_end[j] = temp;
        }
    }
    printf("Reverse operation completed.\n");
}

void swap_channel(Video *video, uint8_t ch1, uint8_t ch2) {
    if (ch1 >= video->channels || ch2 >= video->channels) {
        fprintf(stderr, "Invalid channel numbers: %u, %u\n", ch1, ch2);
        return;
    }
    size_t frame_size = video->channels * video->height * video->width;
    for (int64_t i = 0; i < video->no_of_frames; ++i) {
        uint8_t *frame = video->frames + i * frame_size;
        for (size_t pixel = 0; pixel < video->height * video->width; ++pixel) {
            size_t idx1 = pixel * video->channels + ch1;
            size_t idx2 = pixel * video->channels + ch2;
            uint8_t temp = frame[idx1];
            frame[idx1] = frame[idx2];
            frame[idx2] = temp;
        }
    }
    printf("swap_channel completed successfully.\n");
}


void clip_channel(Video *video, uint8_t channel,
uint8_t min_val, uint8_t max_val) {
    if (channel >= video->channels) {
        fprintf(stderr, "Invalid channel number\n");
        return;
    }

    size_t frame_size = video->channels * video->height * video->width;
    for (int64_t i = 0; i < video->no_of_frames; ++i) {
        uint8_t *frame = video->frames + i * frame_size;
        for (size_t pixel = 0; pixel < video->height * video->width; ++pixel) {
            size_t offset = pixel * video->channels + channel;
            frame[offset] = (frame[offset] < min_val) ? min_val
                          : (frame[offset] > max_val) ? max_val
                          : frame[offset];
        }
    }
}

void scale_channel(Video *video, uint8_t channel, float scale_factor) {
    if (channel >= video->channels) {
        fprintf(stderr, "Invalid channel number\n");
        return;
    }

    size_t frame_size = video->channels * video->height * video->width;
    for (int64_t i = 0; i < video->no_of_frames; ++i) {
        uint8_t *frame = video->frames + i * frame_size;
        for (size_t pixel = 0; pixel < video->height * video->width; ++pixel) {
            size_t offset = pixel * video->channels + channel;
            int64_t scaled_value = (int64_t)(frame[offset] * scale_factor);
            frame[offset] = (scaled_value > 255) ? 255
                           : (scaled_value < 0) ? 0
                           : scaled_value;
        }
    }
}

void free_video(Video *video) {
    if (video) {
        free(video->frames);
        free(video);
    }
}
