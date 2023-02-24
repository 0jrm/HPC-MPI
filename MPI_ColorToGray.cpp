#include <stdio.h>
#include <stdlib.h>
#include "Jpegfile.h"
#include <iostream> // cout
#include "mpi.h"    // MPI header file

int main(int argc, char **argv)
{
    int my_rank;   // process ID
    int num_procs; // number of processes
    UINT height;
    UINT width;
    BYTE *dataBuf;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (my_rank == 0)
    {
        // read the file to dataBuf with RGB format,
        // obtain width and height information by passing arguments as references
        dataBuf = JpegFile::JpegFileToRGB("brigadeiro_kmeans.jpg", &width, &height);
    }

    // broadcast the width and height to all processes
    MPI_Bcast(&width, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // calculate the number of rows per process, and the starting row and ending row for each process
    int rows_per_process = height / num_procs;
    int residual_rows = height % num_procs;

    int my_start_row = my_rank * rows_per_process + std::min(my_rank, residual_rows);
    int my_end_row = my_start_row + rows_per_process + (my_rank < residual_rows ? 1 : 0);
    int my_rows = my_end_row - my_start_row;

    // allocate memory for the local buffer
    BYTE *local_buf = new BYTE[my_rows * width * 3];

    // scatter the dataBuf to all processes
    MPI_Scatter(dataBuf, my_rows * width * 3, MPI_BYTE, local_buf, my_rows * width * 3, MPI_BYTE, 0, MPI_COMM_WORLD);

    // the following code convert RGB to gray luminance.
    for (UINT row = 0; row < my_rows; row++)
    {
        for (UINT col = 0; col < width; col++)
        {
            BYTE *pRed, *pGrn, *pBlu;
            // Get the RGB values for each pixel
            pRed = local_buf + row * width * 3 + col * 3;
            pGrn = local_buf + row * width * 3 + col * 3 + 1;
            pBlu = local_buf + row * width * 3 + col * 3 + 2;

            // calculate luminance of the pixel
            int lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));

            // Set the RGB values of the pixel == luminance
            // this turns every pixel into a shade of gray
            *pRed = (BYTE)lum;
            *pGrn = (BYTE)lum;
            *pBlu = (BYTE)lum;
        }
    }

    // gather the local buffers back to dataBuf
    MPI_Gather(local_buf, my_rows * width * 3, MPI_BYTE, dataBuf, my_rows * width * 3, MPI_BYTE, 0, MPI_COMM_WORLD);

    // write the gray luminance to another jpg file
    if (my_rank == 0)
    {
        JpegFile::RGBToJpegFile("brigamono.jpg", dataBuf, width, height, true, 75);
        delete dataBuf;
    }

    delete local_buf;
    MPI_Finalize();
    return 0;
}