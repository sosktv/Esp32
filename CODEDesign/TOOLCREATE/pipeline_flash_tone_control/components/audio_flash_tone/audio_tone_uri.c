/*This is tone file*/

const char* tone_uri[] = {
   "flash://tone/0_Intro.wav",
};

int get_tone_uri_num()
{
    return sizeof(tone_uri) / sizeof(char *) - 1;
}
