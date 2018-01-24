#cimport crnnoise
from libc.stdlib cimport free

cdef extern from "python_wrapper.c":
    cdef struct vad_results:
        int numframes;
        float *vad_probs;

    vad_results process_file(char *infile, char *outfile)

def denoise_file(infile, outfile):
    cdef bytes infile_bytes = infile.encode();
    cdef char* cinfile = infile_bytes;
    cdef bytes outfile_bytes = outfile.encode();
    cdef char* coutfile = outfile_bytes;
    try:
        results = process_file(cinfile, coutfile)
        arr = [ results.vad_probs[i] for i in range(results.numframes) ]
        return arr
    finally:
        free(results.vad_probs)
