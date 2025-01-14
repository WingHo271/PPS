#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "video.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr,
             "Usage:%s<input_file><output_file> [-S|-M]<operation>[options]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];
    const char *mode = argv[3];
    const char *operation = argv[4];

    if (strcmp(mode, "-S") != 0 && strcmp(mode, "-M") != 0) {
        fprintf(stderr, "Invalid mode: %s. Use -S or -M.\n", mode);
        return EXIT_FAILURE;
    }

    Video *video = read_binary_file(input_file);
    if (!video) {
        fprintf(stderr, "Failed to load video from %s\n", input_file);
        return EXIT_FAILURE;
    }

    if (strcmp(operation, "reverse") == 0) {
        reverse(video);
    } else if (strcmp(operation, "swap_channel") == 0) {
        if (argc < 6) {
            fprintf(stderr, "swap_channel requires <ch1>,<ch2>\n");
            free_video(video);
            return EXIT_FAILURE;
        }
        char *comma = strchr(argv[5], ',');
        if (!comma) {
            fprintf(stderr,
             "Invalid format for swap_channel: use <ch1>,<ch2>\n");
            free_video(video);
            return EXIT_FAILURE;
        }
        *comma = '\0';
        int ch1 = atoi(argv[5]);
        int ch2 = atoi(comma + 1);
        swap_channel(video, ch1, ch2);
    } else if (strcmp(operation, "clip_channel") == 0) {
        if (argc < 7) {
            fprintf(stderr,
             "clip_channel requires <channel> [<min_val>,<max_val>]\n");
            free_video(video);
            return EXIT_FAILURE;
        }
        const char *min_max_str = argv[6];
        if (min_max_str[0] != '['|| min_max_str[strlen(min_max_str)-1] !=']') {
            fprintf(stderr,
              "Invalid format for clip_channel: use [<min_val>,<max_val>]\n");
            free_video(video);
            return EXIT_FAILURE;
        }
        char min_val_str[10], max_val_str[10];
        if (sscanf(min_max_str,
         "[%9[^,],%9[^]]]", min_val_str, max_val_str) != 2) {
            fprintf(stderr,
               "Invalid format for clip_channel: use [<min_val>,<max_val>]\n");
            free_video(video);
            return EXIT_FAILURE;
        }
     clip_channel(video, atoi(argv[5]), atoi(min_val_str), atoi(max_val_str));
    } else if (strcmp(operation, "scale_channel") == 0) {
        if (argc < 7) {
            fprintf(stderr,
             "scale_channel requires <channel> <scale_factor>\n");
            free_video(video);
            return EXIT_FAILURE;
        }
        scale_channel(video, atoi(argv[5]), atof(argv[6]));
    } else {
        fprintf(stderr, "Unknown operation: %s\n", operation);
        free_video(video);
        return EXIT_FAILURE;
    }

    write_binary_file(output_file, video);

    if (strcmp(mode, "-S") == 0) {
        printf("Speed mode (-S) was used for optimization.\n");
    } else if (strcmp(mode, "-M") == 0) {
        printf("Memory mode (-M) was used for optimization.\n");
    }

    free_video(video);
    return EXIT_SUCCESS;
}
