// fixed bug at lines 147, 326 and 499 of Jpegfile.cpp:
// replaced printf(buf) with printf("%s",buf)

#include <stdio.h>
#include <stdlib.h>
#include "Jpegfile.h"

#include <iostream> // cout
#include <omp.h>	// Necessary for parallelization

int main()
{
	UINT height;
	UINT width;
	BYTE *dataBuf;
	// read the file to dataBuf with RGB format,
	// obtain width and height information by passing arguments as references
	dataBuf = JpegFile::JpegFileToRGB("brigadeiro.jpg", &width, &height);

	// the following code convert RGB to gray luminance.
	UINT row, col;

	// Header of the result table
	std::cout << "Threads\t\t|\tRuntime (s)\n";

	// The following loop repeats the experiment for an increasing number of threads
	for (int ncores = 1; ncores < 17; ncores *= 2)
	{
		double t1;
		t1 = omp_get_wtime();

// Parallelization of the experiment. "collapse" guarantees the parallelization of nested loops (1 and 2)
#pragma omp parallel for num_threads(ncores) collapse(2)

// Loop over rows of the image
		for (row = 0; row < height; row++)
		{
			// Loop over the columns of the image
			for (col = 0; col < width; col++)
			{
				BYTE *pRed, *pGrn, *pBlu;
				// Get the RGB values for each pixel 
				pRed = dataBuf + row * width * 3 + col * 3;
				pGrn = dataBuf + row * width * 3 + col * 3 + 1;
				pBlu = dataBuf + row * width * 3 + col * 3 + 2;

				// calculate luminance of the pixel
				int lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));

				// Set the RGB values of the pixel == luminance
				// this turns every pixel into a shade of gray 
				*pRed = (BYTE)lum;
				*pGrn = (BYTE)lum;
				*pBlu = (BYTE)lum;
			}
		}

		std::cout << ncores << "\t\t|\t" << omp_get_wtime() - t1 << "\n";
	}
	// write the gray luminance to another jpg file
	JpegFile::RGBToJpegFile("brigamono.jpg", dataBuf, width, height, true, 75);

	delete dataBuf;
	return 0;
}