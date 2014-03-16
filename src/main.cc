#include <stdlib.h>
#include <stdio.h>
#include "predictor.h"
#include "tread.h"

int main(int argc, char *argv[])
{
  /******************************************************************************
   *                            PARSE COMMAND LINE ARGS
   *****************************************************************************/
  //Parse command line arguments
  if (2 != argc) {
    printf("usage: %s <trace>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /******************************************************************************
   *                            PARSE TRACE FILE
   *****************************************************************************/
  FILE *traceFile;
  FILE *dumpFile;

  branch_record_c *branch = new branch_record_c();
  PREDICTOR *predictor = new PREDICTOR();
  bool prediction = false;
  bool outcome = false;
  uint branches = 0;
  uint correctPredicts = 0;
  float accuracy = 0;
  char *newline = new char[1];
  int ioError;

  // Do validation on command-line arg first
  traceFile = fopen(argv[1], "r");
  dumpFile = fopen("dump", "w");

  // Handle error condition/*{{{*/
  if (!traceFile)
  {
    printf("Error opening trace file!\n");
    return EXIT_FAILURE;
  }

  if (!dumpFile)
  {
    printf("Error opening dump file!\n");
    return EXIT_FAILURE;
  }/*}}}*/

  /*
   * Format of trace file line is:
   * instr. addr. | target addr. | 4 types | outcome
   */
  while ((ioError = fscanf(traceFile, "%x %x %d %d %d %d %d%c",
        &branch->instruction_addr,
        &branch->instruction_next_addr,
        (int*) &branch->is_indirect,
        (int*) &branch->is_conditional,
        (int*) &branch->is_call,
        (int*) &branch->is_return,
        (int*) &outcome,
        newline)) != EOF)
  {
    // Increment branch count
    ++branches;

    // Get prediction and update statistics
    prediction = predictor->get_prediction(branch, NULL);
    predictor->update_predictor(branch, NULL, outcome);

    // Update the correct prediction count
    if (prediction == outcome)
    {
      ++correctPredicts;
    }

    // Dump to trace file
    ioError = fprintf(dumpFile, "%x %x %d %d %d %d %d\n", 
       branch->instruction_addr,
       branch->instruction_next_addr,
       (int) branch->is_indirect,
       (int) branch->is_conditional,
       (int) branch->is_call,
       (int) branch->is_return,
       (int) prediction);
  }

  // File IO is finished, so close the file
  fclose(traceFile);
  fclose(dumpFile);

  /******************************************************************************
   *                            PRINT SUMMARY
   *****************************************************************************/
  accuracy = 100 * correctPredicts / branches;
  printf("\n\n");

  printf("    ███        ▄█    █▄       ▄████████         ▄████████    ▄████████    ▄████████    ▄████████ \n");
  printf("▀█████████▄   ███    ███     ███    ███        ███    ███   ███    ███   ███    ███   ███    ███ \n");
  printf("   ▀███▀▀██   ███    ███     ███    █▀         ███    █▀    ███    █▀    ███    █▀    ███    ███ \n");
  printf("    ███   ▀  ▄███▄▄▄▄███▄▄  ▄███▄▄▄            ███         ▄███▄▄▄      ▄███▄▄▄      ▄███▄▄▄▄██▀ \n");
  printf("    ███     ▀▀███▀▀▀▀███▀  ▀▀███▀▀▀          ▀███████████ ▀▀███▀▀▀     ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   \n");
  printf("    ███       ███    ███     ███    █▄                ███   ███    █▄    ███    █▄  ▀███████████ \n");
  printf("    ███       ███    ███     ███    ███         ▄█    ███   ███    ███   ███    ███   ███    ███ \n");
  printf("   ▄████▀     ███    █▀      ██████████       ▄████████▀    ██████████   ██████████   ███    ███ \n");
  printf("                                                                                      ███    ███ \n");

  printf("\n");
  printf("total branches:\t\t\t%d\n", branches);
  printf("total correct predictions:\t%d\n", correctPredicts);
  printf("accuracy:\t\t\t%u%%\n", (unsigned int)accuracy);
  printf("\n");

  // Garbage collection
  delete branch;
  delete [] newline;
  delete predictor;

  return EXIT_SUCCESS;
}
