/* Copyright (c) 2017 Mozilla */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "rnnoise.h"

#define FRAME_SIZE 480

struct vad_results {
  int numframes;
  float *vad_probs;
};

struct vad_results process_file(char *infile, char *outfile) {
  int i;
  int first = 1;
  float x[FRAME_SIZE];
  float vad_prob;
  FILE *f1, *fout;
  struct stat buffer;
  int status;
  int filenum;
  float *vad_probs;
  struct vad_results results;

  DenoiseState *st;
  st = rnnoise_create();
  f1 = fopen(infile, "r");
  fout = fopen(outfile, "w");

  filenum = fileno(f1);
  status = fstat(filenum, &buffer);
  if(status == -1) {
    results.numframes = -1;
    return results;
  }
  results.numframes = (buffer.st_size * 8 / 16) / FRAME_SIZE;
  vad_probs = (float *)malloc(results.numframes * sizeof(float));
  results.vad_probs = vad_probs;

  int count = 0;
  while (1) {
    short tmp[FRAME_SIZE];
    fread(tmp, sizeof(short), FRAME_SIZE, f1);
    if (feof(f1)) break;
    count += 1;
    for (i=0;i<FRAME_SIZE;i++) x[i] = tmp[i];
    vad_prob = rnnoise_process_frame(st, x, x);
    vad_probs[count] = vad_prob;
    for (i=0;i<FRAME_SIZE;i++) tmp[i] = x[i];
    if (!first) fwrite(tmp, sizeof(short), FRAME_SIZE, fout);
    first = 0;
  }
  rnnoise_destroy(st);
  fclose(f1);
  fclose(fout);
  return results;
}
