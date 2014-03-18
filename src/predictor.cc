/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.  */

#include "predictor.h"

static const bool TAKEN = true;
static const bool NOT_TAKEN = false;
static const bool DEBUG = false;
#define MAX_LOCAL_HISTORY 1024
#define MAX_GLOBAL_HISTORY 4096
#define MAX_CHOICE_HISTORY 4096
#define DEFAULT_LOCAL   3
#define DEFAULT_GLOBAL  1
#define DEFAULT_CHOICE  1
#define MASK10 0x3FF
#define MASK12 0xFFF


PREDICTOR::PREDICTOR()
{
  // Initialize the local history
  LPT = new unsigned int [MAX_LOCAL_HISTORY];
  GPT = new unsigned int [MAX_GLOBAL_HISTORY];
  CPT = new unsigned int [MAX_CHOICE_HISTORY];

  for(int i = 0; i <= MAX_LOCAL_HISTORY; ++i)
  {
    LPT[i] = DEFAULT_LOCAL; // Initialize 3-bit saturation counters to 4 (weakly favor taken)
  }
  for(int i = 0; i <= MAX_GLOBAL_HISTORY; ++i)
  {
    GPT[i] = DEFAULT_GLOBAL; // Initialize 2-bit saturation counters to 2 (weakly favor taken)
  }
  for(int i = 0; i <= MAX_CHOICE_HISTORY; ++i)
  {
    CPT[i] = DEFAULT_CHOICE; // Initialize 2-bit saturation counters to 0 (strongly favor Local Predictor)
  }
  // Initialize path_history to all zero
  path_history = 0;
  iterator = 3;
}


PREDICTOR::~PREDICTOR()
{
  // Deallocate all history
  delete [] LPT;
  delete [] GPT;
  delete [] CPT;
} 


bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os)
{
  /* replace this code with your own */
  bool prediction = TAKEN;
  bool LPT_prediction;
  bool GPT_prediction; 

  if (DEBUG)
  {
    printf("# %d ",iterator++);
    printf("%0x %0x %1d %1d %1d %1d ",br->instruction_addr,     // Considered as PC, p.27 of manual
        br->branch_target,
        br->is_indirect,
        br->is_conditional,
        br->is_call,
        br->is_return);
  }
  // Perform a prediction to a branch using the given algorithm
  if (br->is_conditional || br->is_indirect)
  {
    // Get Local Prediction
    if((LPT[br->instruction_addr & MASK10] & 0x4) >> 2)
    {
      // MSB of LPT saturation counter is 1
      LPT_prediction = TAKEN;
      if (DEBUG)
      {
        printf("Local %x     -> ", LPT[br->instruction_addr & MASK10]);
      }
    }
    else
    {
      // MSB of LPT saturation counter is 0
      LPT_prediction = NOT_TAKEN;
      if (DEBUG)
      {
        printf("Local  %x    -> ", LPT[br->instruction_addr & MASK10]);
      }
    }


    // Get Global Prediction
    if((GPT[path_history & MASK12] & 0x2) >> 1)
    {
      // MSB of GPT saturation counter is 1
      GPT_prediction = TAKEN;
      if (DEBUG)
      {
        printf("Global %x    -> ", GPT[path_history & MASK12]);
      }
    }
    else
    {
      // MSB of GPT saturation counter is 0
      GPT_prediction = NOT_TAKEN;
      if (DEBUG)
      {
        printf("Global %x    -> ", GPT[path_history & MASK12]);
      }
    }


    // Get choice of global or local prediction
    if((CPT[path_history & MASK12] & 0x2) >> 1)
    {
      prediction = GPT_prediction;
      if (DEBUG)
      {
        printf("Choice  %x   -> ", CPT[path_history & MASK12]);
      }
    }
    else
    {
      prediction = LPT_prediction;
      if (DEBUG)
      {
        printf("Choice  %x   -> ", CPT[path_history & MASK12]);
      }
    }
  }
  else
  {
    if (DEBUG)
    {
      printf("Predict taken  -> ");
    }
  }

  return prediction;   // true for taken, false for not taken
}


// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.

// Works like the Local History Table of the branch prediction logic
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken)
{
  // Get current states of each of the prediction tables
  if(LPT[br->instruction_addr & MASK10] <= 3)
    MSBs.lpt_msb = NOT_TAKEN;
  else
    MSBs.lpt_msb = TAKEN;
  if(GPT[path_history & MASK12] <= 1)
    MSBs.gpt_msb = NOT_TAKEN;
  else
    MSBs.gpt_msb = TAKEN;
  if(CPT[path_history & MASK12] <= 1)
    MSBs.cpt_msb = NOT_TAKEN;
  else
    MSBs.cpt_msb = TAKEN;


  // Update branches accordingly
  if(taken) // The branch was taken
  {
    update_CPT(taken); // Update CPT
    if(br->is_conditional || br->is_indirect) // Update LPT only when conditional or indirect
      LPT[br->instruction_addr & MASK10] = update_3bit(LPT[br->instruction_addr & MASK10], TAKEN);  // Update LPT
    GPT[path_history & MASK12]           = update_2bit(GPT[path_history & MASK12], TAKEN);          // Update GPT
    path_history                         = (((path_history << 1) | 0x1) & MASK12);// Update for new branch and mask all
    //  upper bits (12+)
    if (DEBUG)
    {
      printf(" -> taken\n");
    }
  }
  else // The branch was not taken
  {
    update_CPT(taken);
    if(br->is_conditional || br->is_indirect) // Update LPT only when conditional or indirect
      LPT[br->instruction_addr & MASK10] = update_3bit(LPT[br->instruction_addr & MASK10], NOT_TAKEN);  // Update LPT
    GPT[path_history & MASK12]           = update_2bit(GPT[path_history & MASK12], NOT_TAKEN);          // Update GPT
    path_history                         = (((path_history << 1) | 0x0) & MASK12);// Update for new branch and mask all
    //  upper bits (12+)
    if (DEBUG)
    {
      printf(" -> not taken\n");
    }
  }
}


// Update function for 3 bit saturation counter
unsigned int PREDICTOR::update_3bit(unsigned int current_state, bool outcome)
{
  switch(current_state)
  {
    case 0: if(outcome == TAKEN)
              return 1;
            else
              return 0;
    case 1: if(outcome == TAKEN)
              return 2;
            else
              return 0;
    case 2: if(outcome == TAKEN)
              return 3;
            else
              return 1;
    case 3: if(outcome == TAKEN)
              return 4;
            else
              return 2;
    case 4: if(outcome == TAKEN)
              return 5;
            else
              return 3;
    case 5: if(outcome == TAKEN)
              return 6;
            else
              return 4;
    case 6: if(outcome == TAKEN)
              return 7;
            else
              return 5;
    case 7: if(outcome == TAKEN)
              return 7;
            else
              return 6;
    default: return DEFAULT_LOCAL;
  }
  return DEFAULT_LOCAL;
}


// Update function for 2 bit saturation counter
unsigned int PREDICTOR::update_2bit(unsigned int current_state, bool outcome)
{
  switch(current_state)
  {
    case 0: if(outcome == TAKEN)
              return 1;
            else
              return 0;
    case 1: if(outcome == TAKEN)
              return 2;
            else
              return 0;
    case 2: if(outcome == TAKEN)
              return 3;
            else
              return 1;
    case 3: if(outcome == TAKEN)
              return 3;
            else
              return 2;
    default: return DEFAULT_GLOBAL;
  }
  return DEFAULT_GLOBAL;
}

void PREDICTOR::update_CPT(bool outcome)
{
  unsigned int current_state = CPT[path_history & MASK12];
  switch(current_state)
  {
    // LPT was chosen
    case 0: if(!(MSBs.lpt_msb ^ outcome) || (!(MSBs.lpt_msb ^ MSBs.gpt_msb) ^ outcome)) // LPT was correct or both were wrong
            {} // Do nothing because it stays the same
            else if((MSBs.lpt_msb ^ outcome) && !(MSBs.gpt_msb ^ outcome))              // LPT was incorrect and GPT was right
              CPT[path_history & MASK12] = update_2bit(current_state, TAKEN);
            break;

    case 1: if(!(MSBs.lpt_msb ^ outcome) && (MSBs.gpt_msb ^ outcome))                   // LPT was correct and GPT was wrong
              CPT[path_history & MASK12] = update_2bit(current_state, NOT_TAKEN);      
            else if((MSBs.lpt_msb ^ outcome) && !(MSBs.gpt_msb ^ outcome))              // LPT was wrong and GPT was correct
              CPT[path_history & MASK12] = update_2bit(current_state, TAKEN);
            else                                                                        // LPT and GPT both wrong
            {} // Do nothing because it stays the same
            break;

            // GPT was chosen
    case 2: if(!(MSBs.gpt_msb ^ outcome) && (MSBs.lpt_msb ^ outcome))                   // GPT was correct and LPT was wrong
              CPT[path_history & MASK12] = update_2bit(current_state, TAKEN);
            else if((MSBs.gpt_msb ^ outcome) && !(MSBs.lpt_msb ^ outcome))              // GPT was wrong and LPT was correct
              CPT[path_history & MASK12] = update_2bit(current_state, NOT_TAKEN);
            else                                                                        // GPT and LPT both wrong
            {} // Do nothing because it stays the same
            break;

    case 3: if(!(MSBs.gpt_msb ^ outcome) || (!(MSBs.lpt_msb ^ MSBs.gpt_msb) ^ outcome)) // GPT was correct or both were wrong
            {} // Do nothing because it statys the same
            if((MSBs.gpt_msb ^ outcome) && !(MSBs.lpt_msb ^ outcome))                   // GPT was incorrect and LPT was right
              CPT[path_history & MASK12] = update_2bit(current_state, NOT_TAKEN);
            break;
  }
}
