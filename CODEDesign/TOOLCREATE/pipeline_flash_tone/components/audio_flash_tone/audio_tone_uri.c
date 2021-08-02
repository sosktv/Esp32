/*This is tone file*/

const char* tone_uri[] = {
   "flash://tone/0_sample_00.wav",
   "flash://tone/1_sample_01.wav",
   "flash://tone/2_sample_02.wav",
};

int get_tone_uri_num()
{
    return sizeof(tone_uri) / sizeof(char *) - 1;
}
