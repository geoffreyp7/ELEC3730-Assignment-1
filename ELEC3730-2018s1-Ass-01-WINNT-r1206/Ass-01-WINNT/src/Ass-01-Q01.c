//     $Date: 2018-03-11 05:18:25 +1100 (Sun, 11 Mar 2018) $
// $Revision: 1206 $
//   $Author: Geoffrey Parry-Grass $

// Question 1

#include "Ass-01.h"

/*
 * Takes a pointer to an int for storing the number of coefficients read,
 * A pointer to a double array to store the coefficients and an input file name.
 * Saves the number of coefficients read and the coefficient data
 */
int read_coefficients(int *coeff_num_p, double **coeff_values_p, char *filename)
{
    FILE * input = fopen(filename, "rb");
    if (!input) {
    	printf("\nERROR: File could not be opened!\n\n");
    	return -1;
    }

    char int_bytes[4]; //Stores the bytes that represent the number of coefficients

    if (fread(&int_bytes, 4, 1, input) != 1) {
    	printf("\nERROR: Invalid coefficient file!\n\n");
    	return -1;
    }
    //Converts the bytes to a 32 bit int
    int32_t num_coef = *((int*) int_bytes);
    *coeff_num_p = num_coef;

    double * coefs = malloc(num_coef*sizeof(double));
    if (!coefs) {
    	printf("\nERROR: Memory allocation error!\n\n");
    	exit(1);
    }

    //read the doubles representing the coefficients
    if (fread(coefs, sizeof(double), num_coef, input) != num_coef) {
    	printf("\nERROR: Invalid coefficient file!\n\n");
    	free(coefs);
    	return -1;
    }

    *coeff_values_p = coefs;

	if (fclose(input) != 0) {
		printf("\n\nERROR: Input file close error!\n");
		return -1;
	}
    return 0;
}
