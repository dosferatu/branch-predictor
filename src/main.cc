#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "predictor.h"
#include "tread.h"

int main(int argc, char *argv[])
{
  std::fstream *traceFile = new std::fstream();

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
  try
  {
    traceFile->open(argv[1]);
    std::string buffer;
    std::stringstream sstream;
    branch_record_c *branch = new branch_record_c();
    PREDICTOR *predictor = new PREDICTOR();
    bool prediction = false;
    bool outcome = false;
    uint branches = 0;
    uint correctPredicts = 0;
    float accuracy;

    /*
     * Format of trace file is:
     * instr. addr. | target addr. | 4 types | outcome
     */
    while (!traceFile->eof())
    {
      // Increment branch count
      ++branches;

      // Instruction address
      std::getline(*traceFile, buffer, ' ');
      sstream << buffer;
      sstream >> branch->instruction_addr;
      sstream.str(std::string());
      sstream.clear();

      // Target address
      std::getline(*traceFile, buffer, ' ');
      sstream << buffer;
      sstream >> branch->instruction_next_addr;
      sstream.str(std::string());
      sstream.clear();

      // Is indirect?
      std::getline(*traceFile, buffer, ' ');
      sstream << buffer;
      sstream >> branch->is_indirect;
      sstream.str(std::string());
      sstream.clear();

      // Is conditional?
      std::getline(*traceFile, buffer, ' ');
      sstream << buffer;
      sstream >> branch->is_conditional;
      sstream.str(std::string());
      sstream.clear();

      // Is call?
      std::getline(*traceFile, buffer, ' ');
      sstream << buffer;
      sstream >> branch->is_call;
      sstream.str(std::string());
      sstream.clear();

      // Is return?
      std::getline(*traceFile, buffer, ' ');
      sstream << buffer;
      sstream >> branch->is_return;
      sstream.str(std::string());
      sstream.clear();

      // Outcome
      std::getline(*traceFile, buffer, '\n');
      sstream << buffer;
      sstream >> outcome;

      // Get prediction and update statistics
      prediction = predictor->get_prediction(branch, NULL);
      predictor->update_predictor(branch, NULL, outcome);

      // Update the correct prediction count
      if (prediction == outcome)
      {
        ++correctPredicts;
      }
    }

    // File IO is finished, so close the file
    traceFile->close();

    // Print summary
    accuracy = 100 * correctPredicts / branches;
    std::cout << std::endl;
    std::cout << std::endl;
    //std::cout << "__ __|  |   |  ____|       ___|   ____|  ____|   _ \\" << std::endl;
    //std::cout << "   |    |   |  __|       \\___ \\   __|    __|    |   |" << std::endl; 
    //std::cout << "   |    ___ |  |               |  |      |      __ <" << std::endl;   
    //std::cout << "  _|   _|  _| _____|     _____/  _____| _____| _| \\_\\" << std::endl;  

    std::cout << "    ███        ▄█    █▄       ▄████████         ▄████████    ▄████████    ▄████████    ▄████████ " << std::endl;
    std::cout << "▀█████████▄   ███    ███     ███    ███        ███    ███   ███    ███   ███    ███   ███    ███ " << std::endl;
    std::cout << "   ▀███▀▀██   ███    ███     ███    █▀         ███    █▀    ███    █▀    ███    █▀    ███    ███ " << std::endl;
    std::cout << "    ███   ▀  ▄███▄▄▄▄███▄▄  ▄███▄▄▄            ███         ▄███▄▄▄      ▄███▄▄▄      ▄███▄▄▄▄██▀ " << std::endl;
    std::cout << "    ███     ▀▀███▀▀▀▀███▀  ▀▀███▀▀▀          ▀███████████ ▀▀███▀▀▀     ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   " << std::endl;
    std::cout << "    ███       ███    ███     ███    █▄                ███   ███    █▄    ███    █▄  ▀███████████ " << std::endl;
    std::cout << "    ███       ███    ███     ███    ███         ▄█    ███   ███    ███   ███    ███   ███    ███ " << std::endl;
    std::cout << "   ▄████▀     ███    █▀      ██████████       ▄████████▀    ██████████   ██████████   ███    ███ " << std::endl;
    std::cout << "                                                                                      ███    ███ " << std::endl;

    std::cout << std::endl;
    std::cout << "total branches:\t\t\t" << branches << std::endl;
    std::cout << "total correct predictions:\t" << correctPredicts << std::endl;
    std::cout << "accuracy:\t\t\t" << accuracy << "%" <<  std::endl;
    std::cout << std::endl;
  }

  catch (const std::ios_base::failure &e)
  {
    // Implement file not found, access denied, and !success eventually

    // Ghetto file not found version
    std::cout << "Error parsing trace file!" << std::endl;
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
