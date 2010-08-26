/* Copyright (c) 2008 CSIRO
   Copyright (c) 2008-2009 Xiph.Org Foundation
   Written by Jean-Marc Valin */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include "modes.h"
#include "celt.h"
#include "rate.h"

#define INT16 "%d"
#define INT32 "%d"
#define FLOAT "%f"

#ifdef FIXED_POINT
#define WORD16 INT16
#define WORD32 INT32
#else
#define WORD16 FLOAT
#define WORD32 FLOAT
#endif


void dump_modes(FILE *file, CELTMode **modes, int nb_modes)
{
   int i, j, k;
   fprintf(file, "/* The contents of this file is automatically generated and contains static\n");
   fprintf(file, "   definitions for some pre-defined modes */\n");
   fprintf(file, "#include \"modes.h\"\n");
   fprintf(file, "#include \"rate.h\"\n");

   fprintf(file, "\n");

   for (i=0;i<nb_modes;i++)
   {
      CELTMode *mode = modes[i];
      int mdctSize;
      mdctSize = mode->shortMdctSize*mode->nbShortMdcts;
      fprintf(file, "#ifndef DEF_EBANDS%d_%d\n", mode->Fs, mdctSize);
      fprintf(file, "#define DEF_EBANDS%d_%d\n", mode->Fs, mdctSize);
      fprintf (file, "static const celt_int16 eBands%d_%d[%d] = {\n", mode->Fs, mdctSize, mode->nbEBands+2);
      for (j=0;j<mode->nbEBands+2;j++)
         fprintf (file, "%d, ", mode->eBands[j]);
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");
      
      
      fprintf(file, "#ifndef DEF_WINDOW%d\n", mode->overlap);
      fprintf(file, "#define DEF_WINDOW%d\n", mode->overlap);
      fprintf (file, "static const celt_word16 window%d[%d] = {\n", mode->overlap, mode->overlap);
      for (j=0;j<mode->overlap;j++)
         fprintf (file, WORD16 ", ", mode->window[j]);
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");
      
      fprintf(file, "#ifndef DEF_ALLOC_VECTORS%d_%d\n", mode->Fs, mdctSize);
      fprintf(file, "#define DEF_ALLOC_VECTORS%d_%d\n", mode->Fs, mdctSize);
      fprintf (file, "static const unsigned char allocVectors%d_%d[%d] = {\n", mode->Fs, mdctSize, mode->nbEBands*mode->nbAllocVectors);
      for (j=0;j<mode->nbAllocVectors;j++)
      {
         for (k=0;k<mode->nbEBands;k++)
            fprintf (file, "%2d, ", mode->allocVectors[j*mode->nbEBands+k]);
         fprintf (file, "\n");
      }
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");

      fprintf(file, "#ifndef DEF_LOGN%d_%d\n", mode->Fs, mdctSize);
      fprintf(file, "#define DEF_LOGN%d_%d\n", mode->Fs, mdctSize);
      fprintf (file, "static const celt_int16 logN%d_%d[%d] = {\n", mode->Fs, mdctSize, mode->nbEBands);
      for (j=0;j<mode->nbEBands;j++)
         fprintf (file, "%d, ", mode->logN[j]);
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");

      /* Pulse cache */
      fprintf(file, "#ifndef DEF_PULSE_CACHE%d_%d\n", mode->Fs, mdctSize);
      fprintf(file, "#define DEF_PULSE_CACHE%d_%d\n", mode->Fs, mdctSize);
      fprintf (file, "static const celt_int16 cache_index%d_%d[%d] = {\n", mode->Fs, mdctSize, (mode->maxLM+2)*mode->nbEBands);
      for (j=0;j<mode->nbEBands*(mode->maxLM+2);j++)
         fprintf (file, "%d, ", mode->cache.index[j]);
      fprintf (file, "};\n");
      fprintf (file, "static const unsigned char cache_bits%d_%d[%d] = {\n", mode->Fs, mdctSize, mode->cache.size);
      for (j=0;j<mode->cache.size;j++)
         fprintf (file, "%d, ", mode->cache.bits[j]);
      fprintf (file, "};\n");
      fprintf(file, "#endif\n");
      fprintf(file, "\n");

      /* FFT twiddles */
      fprintf(file, "#ifndef FFT_TWIDDLES%d_%d\n", mode->Fs, mdctSize);
      fprintf(file, "#define FFT_TWIDDLES%d_%d\n", mode->Fs, mdctSize);
      fprintf (file, "static const kiss_twiddle_cpx fft_twiddles%d_%d[%d] = {\n",
            mode->Fs, mdctSize, mode->mdct.kfft[0]->nfft);
      for (j=0;j<mode->mdct.kfft[0]->nfft;j++)
         fprintf (file, "{" WORD16 ", " WORD16 "}, ", mode->mdct.kfft[0]->twiddles[j].r, mode->mdct.kfft[0]->twiddles[j].i);
      fprintf (file, "};\n");

      /* FFT Bitrev tables */
      for (i=0;i<=mode->mdct.maxshift;i++)
      {
         fprintf(file, "#ifndef FFT_BITREV%d_%d\n", mode->Fs, mdctSize>>i);
         fprintf(file, "#define FFT_BITREV%d_%d\n", mode->Fs, mdctSize>>i);
         fprintf (file, "static const celt_int16 fft_bitrev%d_%d[%d] = {\n",
               mode->Fs, mdctSize>>i, mode->mdct.kfft[i]->nfft);
         for (j=0;j<mode->mdct.kfft[i]->nfft;j++)
            fprintf (file, "%d, ", mode->mdct.kfft[i]->bitrev[j]);
         fprintf (file, "};\n");

         fprintf(file, "#endif\n");
         fprintf(file, "\n");
      }

      /* FFT States */
      for (i=0;i<=mode->mdct.maxshift;i++)
      {
         fprintf(file, "#ifndef FFT_STATE%d_%d\n", mode->Fs, mdctSize>>i);
         fprintf(file, "#define FFT_STATE%d_%d\n", mode->Fs, mdctSize>>i);
         fprintf (file, "static const kiss_fft_state fft_state%d_%d = {\n",
               mode->Fs, mdctSize>>i);
         fprintf (file, "%d,\t/* nfft */\n", mode->mdct.kfft[i]->nfft);
#ifndef FIXED_POINT
         fprintf (file, "%f,\t/* scale */\n", mode->mdct.kfft[i]->scale);
#endif
         fprintf (file, "%d,\t/* shift */\n", mode->mdct.kfft[i]->shift);
         fprintf (file, "{");
         for (j=0;j<2*MAXFACTORS;j++)
            fprintf (file, "%d, ", mode->mdct.kfft[i]->factors[j]);
         fprintf (file, "},\t/* factors */\n");
         fprintf (file, "fft_bitrev%d_%d,\t/* bitrev */\n", mode->Fs, mdctSize>>i);
         fprintf (file, "fft_twiddles%d_%d,\t/* bitrev */\n", mode->Fs, mdctSize);
         fprintf (file, "};\n");

         fprintf(file, "#endif\n");
         fprintf(file, "\n");
      }

      fprintf(file, "#endif\n");
      fprintf(file, "\n");

      /* MDCT twiddles */
      fprintf(file, "#ifndef MDCT_TWIDDLES%d_%d\n", mode->Fs, mdctSize);
      fprintf(file, "#define MDCT_TWIDDLES%d_%d\n", mode->Fs, mdctSize);
      fprintf (file, "static const celt_word16 mdct_twiddles%d_%d[%d] = {\n",
            mode->Fs, mdctSize, mode->mdct.n/4);
      for (j=0;j<mode->mdct.n/4;j++)
         fprintf (file, WORD16 ", ", mode->mdct.trig[j]);
      fprintf (file, "};\n");

      fprintf(file, "#endif\n");
      fprintf(file, "\n");


      /* Print the actual mode data */
      fprintf(file, "static const CELTMode mode%d_%d_%d = {\n", mode->Fs, mdctSize, mode->overlap);
      fprintf(file, "0x%x,\t/* marker */\n", 0xa110ca7e);
      fprintf(file, INT32 ",\t/* Fs */\n", mode->Fs);
      fprintf(file, "%d,\t/* overlap */\n", mode->overlap);
      fprintf(file, "%d,\t/* nbEBands */\n", mode->nbEBands);
      fprintf(file, "%d,\t/* effEBands */\n", mode->effEBands);
      fprintf(file, "{");
      for (j=0;j<4;j++)
         fprintf(file, WORD16 ", ", mode->preemph[j]);
      fprintf(file, "},\t/* preemph */\n");
      fprintf(file, "eBands%d_%d,\t/* eBands */\n", mode->Fs, mdctSize);
      fprintf(file, "%d,\t/* nbAllocVectors */\n", mode->nbAllocVectors);
      fprintf(file, "allocVectors%d_%d,\t/* allocVectors */\n", mode->Fs, mdctSize);

      fprintf(file, "{%d, %d, {", mode->mdct.n, mode->mdct.maxshift);
      for (i=0;i<=mode->mdct.maxshift;i++)
         fprintf(file, "&fft_state%d_%d, ", mode->Fs, mdctSize>>i);
      fprintf (file, "}, mdct_twiddles%d_%d},\t/* mdct */\n", mode->Fs, mdctSize);

      fprintf(file, "window%d,\t/* window */\n", mode->overlap);
      fprintf(file, "%d,\t/* maxLM */\n", mode->maxLM);
      fprintf(file, "%d,\t/* nbShortMdcts */\n", mode->nbShortMdcts);
      fprintf(file, "%d,\t/* shortMdctSize */\n", mode->shortMdctSize);
      fprintf(file, "0,\t/* prob */\n");
      fprintf(file, "logN%d_%d,\t/* logN */\n", mode->Fs, mdctSize);
      fprintf(file, "{%d, cache_index%d_%d, cache_bits%d_%d},\t/* cache */\n",
            mode->cache.size, mode->Fs, mdctSize, mode->Fs, mdctSize);
      fprintf(file, "0x%x,\t/* marker */\n", 0xa110ca7e);
      fprintf(file, "};\n");
   }
   fprintf(file, "\n");
   fprintf(file, "/* List of all the available modes */\n");
   fprintf(file, "#define TOTAL_MODES %d\n", nb_modes);
   fprintf(file, "static const CELTMode * const static_mode_list[TOTAL_MODES] = {\n");
   for (i=0;i<nb_modes;i++)
   {
      CELTMode *mode = modes[i];
      int mdctSize;
      mdctSize = mode->shortMdctSize*mode->nbShortMdcts;
      fprintf(file, "&mode%d_%d_%d,\n", mode->Fs, mdctSize, mode->overlap);
   }
   fprintf(file, "};\n");
}

void dump_header(FILE *file, CELTMode **modes, int nb_modes)
{
   int i;
   int channels = 0;
   int frame_size = 0;
   int overlap = 0;
   fprintf (file, "/* This header file is generated automatically*/\n");
   for (i=0;i<nb_modes;i++)
   {
      CELTMode *mode = modes[i];
      if (frame_size==0)
         frame_size = mode->shortMdctSize*mode->nbShortMdcts;
      else if (frame_size != mode->shortMdctSize*mode->nbShortMdcts)
         frame_size = -1;
      if (overlap==0)
         overlap = mode->overlap;
      else if (overlap != mode->overlap)
         overlap = -1;
   }
   if (channels>0)
   {
      fprintf (file, "#define CHANNELS(mode) %d\n", channels);
      if (channels==1)
         fprintf (file, "#define DISABLE_STEREO\n");
   }
   if (frame_size>0)
   {
      fprintf (file, "#define FRAMESIZE(mode) %d\n", frame_size);
   }
   if (overlap>0)
   {
      fprintf (file, "#define OVERLAP(mode) %d\n", overlap);
   }
}

int main(int argc, char **argv)
{
   int i, nb;
   FILE *file;
   CELTMode **m;
   if (argc%2 != 1)
   {
      fprintf (stderr, "must have a multiple of 2 arguments\n");
      return 1;
   }
   nb = (argc-1)/2;
   m = malloc(nb*sizeof(CELTMode*));
   for (i=0;i<nb;i++)
   {
      int Fs, frame;
      Fs      = atoi(argv[2*i+1]);
      frame   = atoi(argv[2*i+2]);
      m[i] = celt_mode_create(Fs, frame, NULL);
   }
   file = fopen("static_modes.c", "w");
   dump_modes(file, m, nb);
   fclose(file);
   file = fopen("static_modes.h", "w");
   dump_header(file, m, nb);
   fclose(file);
   for (i=0;i<nb;i++)
      celt_mode_destroy(m[i]);
   free(m);
   return 0;
}
