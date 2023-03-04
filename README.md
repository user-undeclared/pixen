# PIXEN

pixen (short for PIXel ENlarger) is a tool to upscale images (or, in other words, enlarge their pixels) without interpolation or anti-aliasing, making the resulting image as crisp and sharp as the original. this is useful for upscaling pixel art or other images with pixel-precise details

### building

to build pixen, you must have `stb_image.h` and `stb_image_write.h` present in your system's include path. both headers can be found [here](https://github.com/nothings/stb).
from there, the only thing left to do is to compile the program:

```console
$ cc -O3 -o pixen src/*.c -lm -Wall -Wextra -Werror -pedantic
```
### usage

pixen takes two arguments: a muliplier for the size of resulting image (where 2 means "twice as big", and so on), and the file path to the source image. it will then print the scaled image to stdout, which can be redirected to a file or to some other program

```console
$ ./pixen 16 example.png > example-scaled.png
$ ./pixen 8 sample.png | feh -
```
