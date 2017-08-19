#pragma once

int adpcm_decode(TAG *tag, int buf_size, int channels, int16_t *samples, int max_samples);
