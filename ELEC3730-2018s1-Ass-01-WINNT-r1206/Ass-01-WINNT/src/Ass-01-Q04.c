//     $Date: 2018-03-11 05:18:25 +1100 (Sun, 11 Mar 2018) $
// $Revision: 1206 $
//   $Author: Geoffrey Parry-Grass $

#include "Ass-01.h"

/*
 * Takes an input string and pointer to an array of words.
 * Breaks the input string into words and fills the array of words with the words.
 * Allocates heap memory to store the char * and each word is given a separate
 * allocation of heap memory to store the chars
 */
int string_parser(char *inp, char **array_of_words_p[])
{
	  int string_length;
	  int num_words = 0;

	  char ** arr = malloc(sizeof(char*)*INIT_NUM_WORDS);
	  if (!arr) {
		  printf("\nERROR: Memory allocation error!\n\n");
		  exit(1);
	  }
	  int word_mem = INIT_NUM_WORDS;

	  //Check if input string is null and then if not,
	  //store length of string and check if string is empty
	  if (inp && (string_length = strlen(inp))) {

		  //true when a space has been encountered,
		  //initially set to true to capture first word,
		  //reset at beginning of new word
		  int space_found = 1;

		  //Step through the entire string
		  for (int i=0; i<string_length; i++) {

			  if (inp[i] == ' ') {
				  space_found = 1;
			  }

              //A new word has been found
			  if ((inp[i] != ' ' && inp[i]) && space_found) {

				  if (num_words >= word_mem) {
					  arr = realloc(arr, sizeof(char*)*(word_mem += INIT_NUM_WORDS));
					  if (!arr) {
						  printf("\nERROR: Memory allocation error!\n\n");
						  exit(1);
					  }
				  }


				  //Malloc an inital piece of memory
				  arr[num_words] = malloc(sizeof(char)*INIT_WORD_LENGTH);
				  if (!arr[num_words]) {
					  printf("\nERROR: Memory allocation error!\n\n");
					  exit(1);
				  }

				  int mem_size = INIT_WORD_LENGTH; //Size of current memory allocation
				  int num_chars; //Number of characters currently in word

				  //Step through the word and copy it into char * word
				  for (num_chars = 0; inp[i] != ' ' && inp[i] != '\0'; i++, num_chars++) {

                      if (num_chars < mem_size) {
                    	  //If there is enough memory simply store the char
                    	  arr[num_words][num_chars] = inp[i];
                      } else {
                    	  //If there is not enough memory increase the size
                    	  mem_size += INIT_WORD_LENGTH;
                    	  arr[num_words] = realloc(arr[num_words], sizeof(char)*(mem_size));
        				  if (!arr[num_words]) {
        					  printf("\nERROR: Memory allocation error!\n\n");
        					  exit(1);
        				  }
        				  arr[num_words][num_chars] = inp[i];
                      }
				  }

				  //Make the memory allocation only as big as the current word length
				  //plus enough for the NULL character
				  arr[num_words] = realloc(arr[num_words], sizeof(char) * (num_chars+1));
				  if (!arr[num_words]) {
					  printf("\nERROR: Memory allocation error!\n\n");
					  exit(1);
				  }

				  //Terminate the string with the NULL character
				  arr[num_words][num_chars] = '\0';
				  num_chars++;

				  space_found = 0; //reset for next word
				  i--; //Step the index back to the end of the last word
				  num_words++; //Increase the word count

			  }
		  }

		  arr = realloc(arr, sizeof(char*)*(num_words));
		  if (!arr) {
			  printf("\nERROR: Memory allocation error!\n\n");
			  exit(1);
		  }
		  *(array_of_words_p) = arr;

	  }

	  return num_words;
}
