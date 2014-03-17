/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 */

#include "predictor.h"



bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os)
{
  bool prediction = true;
  bool local;
  bool global;
  bool choice;
  int lcount, gcount, ccount;

  if (DEBUG) {
	  printf("%0x %0x %1d %1d %1d %1d ",	
			  br->instruction_addr,
			  br->branch_target,
			  br->is_indirect,
			  br->is_conditional,
			  br->is_call,
			  br->is_return);
  }

  if (br->is_conditional) { 
    local = local_predict[br->instruction_addr & PC_INDEX]->get_sign();
    global = global_predict[path_history & PATH_HIST]->get_sign();
    choice = choice_predict[path_history & PATH_HIST]->get_sign();

	// l/g/c count values are used in debug to verify incr/decr counters work
    lcount = local_predict[br->instruction_addr & PC_INDEX]->get_count();
    gcount = global_predict[path_history & PATH_HIST]->get_count();
    ccount = choice_predict[path_history & PATH_HIST]->get_count();

    prediction = local;
    if (choice) 
      prediction = global;
	if (DEBUG)
		printf("\tL:%1d(%d) G:%1d(%d) C:%1d(%d) P:%d",
			local, lcount, global, gcount, choice, ccount, prediction);
  }
  return prediction;   // true for taken, false for not taken
}



// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken)
{
  bool local;
  bool global;
  //bool choice;
  uint old_hist;
  if (DEBUG)
	printf(" T:%1d  HIST:%x",taken, path_history & PATH_HIST);
  // retrieve predictions
  local = local_predict[br->instruction_addr & PC_INDEX]->get_sign();
  global = global_predict[path_history & PATH_HIST]->get_sign();
  old_hist = path_history;

  if (br->is_conditional || br->is_indirect) 
  {
	  path_history = path_history << 1;
	  if (taken) {
		  // update choice
		  if (local && !global)
			  choice_predict[old_hist & PATH_HIST]->decrement();
		  else if (!local && global)
			  choice_predict[old_hist & PATH_HIST]->increment();

		  // update local, global and path history
		  local_predict[br->instruction_addr & PC_INDEX]->increment();
		  global_predict[old_hist & PATH_HIST]->increment();
		  path_history |= 1;
	  }
	  else {
		  // update choice
		  if (local && !global)
			  choice_predict[old_hist & PATH_HIST]->increment();
		  else if (!local && global)
			  choice_predict[old_hist & PATH_HIST]->decrement();

		  // update local, global and path history
		  local_predict[br->instruction_addr & PC_INDEX]->decrement();
		  global_predict[old_hist & PATH_HIST]->decrement();
		  path_history &= (PC_INDEX - 1);
	  }
	  if (DEBUG)
		printf("\tN_HIST:%x\n",path_history & PATH_HIST);
  }
}


PREDICTOR::PREDICTOR() {
  // allocate arrays of pointers to SATCOUNTER objects
  local_predict = new SATCOUNTER*[LOCAL_PRED_SIZE];
  global_predict = new SATCOUNTER*[GLOBAL_PRED_SIZE];
  choice_predict = new SATCOUNTER*[CHOICE_PRED_SIZE];
  // allocate each SATCOUNTER within each array
  for (int i = 0; i < LOCAL_PRED_SIZE; i++)
    local_predict[i] = new SATCOUNTER(LOCAL_PRED_BITS);
  for (int i = 0; i < GLOBAL_PRED_SIZE; i++)
    global_predict[i] = new SATCOUNTER(GLOBAL_PRED_BITS);
  for (int i = 0; i < CHOICE_PRED_SIZE; i++)
    choice_predict[i] = new SATCOUNTER(CHOICE_PRED_BITS);
  
  for (int i = 0; i < CHOICE_PRED_SIZE; i++)
	  choice_predict[i]->set_weak0();

	path_history = 0xaaa;
}

PREDICTOR::~PREDICTOR() {
  // deallocate array objects
  for (int i = 0; i < LOCAL_PRED_SIZE; i++)
    delete local_predict[i];
  for (int i = 0; i < GLOBAL_PRED_SIZE; i++)
    delete global_predict[i];
  for (int i = 0; i < CHOICE_PRED_SIZE; i++)
    delete choice_predict[i];
  // deallocate arrays of pointers
  delete [] local_predict;
  delete [] global_predict;
  delete [] choice_predict;
}




