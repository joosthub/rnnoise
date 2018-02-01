from crnnoise cimport DenoiseState, rnnoise_create, rnnoise_destroy, rnnoise_process_frame

from libc.stdlib cimport free, malloc
from libc.stdio cimport FILE, fopen, fclose, fwrite, feof, fread
from posix.stat cimport fstat, struct_stat
from posix.stdio cimport fileno


DEF FRAME_SIZE=480


def denoise_file(infile, outfile):
    cdef bytes infile_bytes = infile.encode();
    cdef char* cinfile = infile_bytes;
    cdef bytes outfile_bytes = outfile.encode();
    cdef char* coutfile = outfile_bytes;
    cdef int count = 0;
    cdef int i;
    cdef int first = 1;
    cdef float x[FRAME_SIZE];
    cdef float vad_prob;
    cdef int numframes;
    cdef FILE *f1, *fout;
    cdef struct_stat buf;
    cdef int status;
    cdef int filenum;
    cdef double *vad_probs;
    cdef DenoiseState *st;
    cdef short tmp[FRAME_SIZE];

    st = rnnoise_create();
    f1 = fopen(infile_bytes, "r");
    fout = fopen(outfile_bytes, "w");

    filenum = fileno(f1);
    status = fstat(filenum, &buf);
    if(status == -1):
        return []

    numframes = (buf.st_size * 8 / 16) / FRAME_SIZE;
    vad_probs = <double *>malloc(numframes * sizeof(double));

    while 1:
        fread(tmp, sizeof(short), FRAME_SIZE, f1);
        if feof(f1):
            break;
        count += 1;

        for i in range(0, FRAME_SIZE):
            x[i] = <float> tmp[i];

        vad_prob = rnnoise_process_frame(st, x, x);
        vad_probs[count] = <double> vad_prob;

        for i in range(0, FRAME_SIZE):
            tmp[i] = <short> x[i];
        if (not first):
            fwrite(tmp, sizeof(short), FRAME_SIZE, fout);
        first = 0;

    rnnoise_destroy(st);
    fclose(f1);
    fclose(fout);

    try:
        arr = [ vad_probs[i] for i in range(numframes) ]
        return arr
    finally:
        free(vad_probs)

