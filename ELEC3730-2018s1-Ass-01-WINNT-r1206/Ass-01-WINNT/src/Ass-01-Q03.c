//     $Date: 2017-03-13 08:12:38 +1100 (Mon, 13 Mar 2017) $
// $Revision: 821 $
//   $Author: Geoffrey Parry-Grass $

#include "Ass-01.h"

/*
 * Takes the filepath of a PCM encoded wav file, filepath containing FIR filter coefficients and filepath of
 * output wave file. Outputs a file at the output file path that is equal to the FIR filter applied to the
 * input wave file.
 * EXTENSION: More efficient memory usage, only reads in enough data to memory to perform the calcuation at any given time,
              uses a circular buffer.
 */
int filter(char *filter_filename, char *input_wavefilename, char *output_wavefilename)
{
	//Open the wav file to be filtered
    FILE * input = fopen(input_wavefilename, "rb");
    if (!input) {
    	printf("\n\nERROR: Could not open input file!\n");
    	return -1;
    }

    //Open (or create) the file to store the filtered file
    FILE * output = fopen(output_wavefilename, "wb");
    if (!output) {
    	printf("\n\nERROR: Could not open output file!\n");
    	return -1;
    }

    pcm_wavefile_header_t * input_header = (pcm_wavefile_header_t *) malloc(sizeof(pcm_wavefile_header_t));
    if (!input_header) {
    	printf("\n\nERROR: Memory allocation error!\n");
    	exit(1);
    }

    double * filter_coefs; //Stores the coefficients of the FIR filter
    int number_coefs; //Stores the number of coefficients

    if (read_coefficients(&number_coefs, &filter_coefs, filter_filename) == -1) {
    	printf("\n\nERROR: Input file read error! (coefficients)\n");
        free(input_header);
        return -1;
    }

    //Read wave file input header
    if (fread(input_header, sizeof(pcm_wavefile_header_t), 1, input) != 1) {
    	printf("\n\nERROR: Input file read error!\n");
    	free(input_header);
    	return -1;
    }

    //Reduce the subchunk 2 size in the header as the output file will have less data
    input_header->Subchunk2Size -= (number_coefs - 1)*2;

    //Output the adjusted header file to the new wave file
    if (fwrite(input_header, sizeof(pcm_wavefile_header_t), 1, output) != 1) {
    	printf("\n\nERROR: Output file write error!\n");
    	free(input_header);
    	return -1;
    }

    //This stores the active samples (equal in size to number of FIR coefficients)
    //(Used as a circular buffer)
    int16_t * samples = malloc(sizeof(uint16_t)*number_coefs);
    if (!samples) {
    	printf("\n\nERROR: Memory allocation error!\n");
    	exit(1);
    }

    //Fills the samples array with the first set of samples
    if (fread(samples, sizeof(int16_t), number_coefs, input) != number_coefs) {
    	printf("\n\nERROR: Input file read error!\n");
    	return -1;
    }

    //Step through the input file data and filter it (i increments by 2 as each sample is 2 bytes)
	//EXTENSION: More efficient memory usage, only reads in enough data to memory to perform the calcuation at any given time,
	//uses a circular buffer.
    for (int i=0; i<input_header->Subchunk2Size-2; i+=2) {

    	double filtered_sample_value = 0;

    	//Perform the FIR multiplication
    	//Each input sample iteration rotates the coefficient array left 1 position
    	for (int j=0; j<number_coefs; j++) {
    		filtered_sample_value += rot_arr_left_get_double(j, ((i/2)%number_coefs), filter_coefs, number_coefs) * samples[number_coefs-1-j];
    	}

    	//Write the sample to the output file
    	int16_t write_sample = (int16_t) filtered_sample_value;
    	if (fwrite(&write_sample, sizeof(int16_t), 1, output) != 1) {
        	printf("\n\nERROR: Output file write error!\n");
            free(samples);
            free(input_header);
        	return -1;
        }

    	int16_t next_sample;

    	if (fread(&next_sample, sizeof(int16_t), 1, input) != 1) {
        	printf("\n\nERROR: Input file read error!%d\n", i);
            free(samples);
            free(input_header);
        	return -1;
        }

    	//Store the next sample in the position of the oldest sample (circular buffer)
    	samples[(i/2)%number_coefs] = next_sample;

    }

    //Free memory
    free(samples); //Who doesn't like free samples?
    free(input_header);

    //Close files
	if (fclose(input) != 0) {
		printf("\n\nERROR: Input file close error!\n");
		return -1;
	}

	if (fclose(output) != 0) {
		printf("\n\nERROR: Output file close error!\n");
		return -1;
	}

    return 0;
}

/*
 * Takes an array of doubles, the number of items in the array, the index to access and a rotation value.
 * Returns the double located at the given index as if the array had been rotated rot places to the left.
 */
double rot_arr_left_get_double(int index, int rot, double * arr, int numItems) {

	int index_value = (index+rot)%numItems;
	return (arr[index_value]);

}

