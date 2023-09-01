/*Brando Morley
  bmorley
  117492576
  0108 
*/

#include <stdio.h>
#include <ctype.h>
#include <pthread.h> 
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

/*Uses threads and the wc.c file given to us to count the lines, words, and 
  chars in each file. */

static void *word_count(void *args);

/* Counts the number of words, lines, and characters in the files whose
 * names are given as command-line arguments.  If there are no command-line
 * arguments then the line, word, and character counts will just be 0.
 * Mimics the effects of the UNIX "wc" utility, although does not have
 * exactly the same behavior in all cases.
 */

static void *word_count(void *args){
  int lines = 0, words = 0, chars = 0, *result;
  char ch, next_ch, *filename;
  FILE *fp;

  filename = (char *)args;
  fp = fopen(filename, "r");
  
  if(fp != NULL) {

/* read each file one character at a time, until EOF */
    ch= fgetc(fp);
    while (!feof(fp)) {
      next_ch= fgetc(fp);  /* look ahead and get the next character */
      ungetc(next_ch, fp);   /* unread the next character (see Chapter 15) */

        /* update the counts as needed every time a character is read */

        /* a newline means the line count increases */
      if (ch == '\n')
	lines++;

        /* if` the current character is not whitespace but the next character
           is, or if the current character is not whitespace and it is the
           last character in the input, the word count increases */
      if (!isspace(ch) && (isspace(next_ch) || feof(fp)))
	words++;
      
        /* increasing the character count is a no-brainer */
      chars++;

      ch= fgetc(fp);
    }
    fclose(fp);
  }

  /* adds the lines, words, and chars to the dynamically created return value*/
  result = malloc(3 * sizeof(int));
  result[0] = lines;
  result[1] = words;
  result[2] = chars;       
 
  /*frees the dynamically created file name from the main function */
  free(args);
  return result;
}



int main(int argc, char *argv[]) {
  int total_lines = 0, total_words = 0, total_chars = 0, i;
  char *filename;
  pthread_t *th;
  void *ptr = NULL;

  /*Creates same amount of threads as arguments in the command line */
  th = malloc((argc - 1) * sizeof(pthread_t));
  
  /*Creates a string that can be passed into the thread as well as creating the
    thread*/
  for(i = 1; i < argc; i++) { 
    filename = malloc(strlen(argv[i]) + 1);
    strcpy(filename, argv[i]);
    assert(pthread_create(&th[i - 1], NULL, &word_count, filename) == 0);
  }

  /*This for loop reaps all the threads and adds the lines, characters, and 
    words counted in the thread to the total amount */
  for(i = 0; i < argc - 1; i++) {
    assert(pthread_join(th[i], &ptr) == 0);
    total_lines += ((int*)ptr)[0];
    total_words += ((int*)ptr)[1];
    total_chars += ((int*)ptr)[2];
    free(ptr);
  }  

  /*Frees the dynamically created threads and prints total lines, words, and
    chars*/
  free(th);
  printf("%4d %4d %4d\n", total_lines, total_words, total_chars);

  return 0;
}
