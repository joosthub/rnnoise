from rnnoise import denoise_file
import rnnoise

vals = denoise_file('test.pcm', 'out_python.pcm')
print(vals[:10])
print('test complete')
