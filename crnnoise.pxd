cdef extern from "rnnoise.h":

    ctypedef struct DenoiseState:
        pass

    int rnnoise_get_size()

    int rnnoise_init(DenoiseState *st)

    DenoiseState *rnnoise_create()

    void rnnoise_destroy(DenoiseState *st)

    float rnnoise_process_frame(DenoiseState *st, float *out, float *inp)

