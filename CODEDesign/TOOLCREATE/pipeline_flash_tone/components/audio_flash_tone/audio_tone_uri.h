#ifndef __AUDIO_TONEURI_H__
#define __AUDIO_TONEURI_H__

extern const char* tone_uri[];

typedef enum {
    TONE_TYPE_SAMPLE_00,
    TONE_TYPE_SAMPLE_01,
    TONE_TYPE_SAMPLE_02,
    TONE_TYPE_MAX,
} tone_type_t;

int get_tone_uri_num();

#endif
