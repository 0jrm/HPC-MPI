About this folder
In the root of this folder you find the files for running the program. The library files needed (downloaded from Canvas) are in folder JpegLib. The pdf file with the report is in the folder Report 

ColorToGray.cpp
This program reads an image file in RGB format and converts it to grayscale using the luminance method. The program also measures the runtime of the conversion for different number of threads and reports the results.

Jpegfile (.h and .cpp)
Provided for the homework, and edited for fixing compilation errors.

Usage
The program uses the "Jpegfile.h" library to read and write the image files. The input file should be located in the same directory as the program and should be named "brigadeiro.jpg". The output file will be named "brigamono.jpg".
You can compile and run this program by simply using the command "make" on the terminal. It will generate and run the program ColorToGray.exe.

Functionality
The program first reads the input file into a buffer, then converts each pixel of the image to grayscale using the luminance formula:

lum = 0.299 * R + 0.587 * G + 0.114 * B

where R, G, and B are the red, green, and blue values of the pixel respectively. The program then writes the grayscale image to the output file.

Additionally, the program measures the runtime of the conversion for different number of threads (1, 2, 4, 8, 16) using OpenMP library and reports the results.

Changes Made
Fixed bug at lines 147, 326 and 499 of omp_get_wtime():
Replaced printf(buf) with printf("%s",buf)

Note
The program uses the OpenMP library for parallel processing, so it requires a compiler that supports OpenMP.
