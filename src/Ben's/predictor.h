/* Author: Mark Faust
 *
 * C version of predictor file
*/

#ifndef PREDICTOR_H_SEEN
#define PREDICTOR_H_SEEN

#include <cstddef>
#include <cstring>
#include <inttypes.h>
#include <vector>
#include "op_state.h"   // defines op_state_c (architectural state) class 
#include "tread.h"      // defines branch_record_c class

struct msbs
{
  bool lpt_msb;
  bool gpt_msb;
  bool cpt_msb;
};


class PREDICTOR
{
  public:
    PREDICTOR();
    ~PREDICTOR();
    bool get_prediction(const branch_record_c* br, const op_state_c* os);

    void update_predictor(const branch_record_c* br, const op_state_c* os, bool taken);

    bool get_local_prediction();    // Performs local prediction checking
    bool get_global_prediction();   // Performs global prediction checking
    bool get_choice_prediction();   // Performs choice prediction between local or global
    unsigned int update_3bit(unsigned int current_state, bool outcome); // Update 3-bit saturation counter
    unsigned int update_2bit(unsigned int current_state, bool outcome); // Update 2-bit saturation counter
    void update_CPT(bool outcome);  // Update CPT

  private:
    unsigned int * LPT; // Local Prediction Table
    unsigned int * GPT; // Global Prediction Table
    unsigned int * CPT; // Choice Prediction Table
    unsigned int path_history;
    msbs MSBs;
    int iterator;
};

#endif // PREDICTOR_H_SEEN

