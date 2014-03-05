/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
*/

#include "predictor.h"



    bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os)
        {
		/* replace this code with your own */	// os is not being used anywhere
            bool prediction = true;
			bool local;
			bool global;
			bool choice;

			printf("%0x %0x %1d %1d %1d %1d ",	br->instruction_addr,
												br->branch_target,
												br->is_indirect,
												br->is_conditional,
												br->is_call,
												br->is_return);

			if (br->is_conditional) { 
				local = local_predict[br->instruction_addr & PC_INDEX]->get_sign();
				global = global_predict[path_history & PATH_HIST]->get_sign();
				choice = choice_predict[path_history & PATH_HIST]->get_sign();

				prediction = local;
				if (choice) 
					prediction = global;
				printf("\tL:%1d G:%1d C:%1d P:%d",
						local, global, choice, prediction);
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
			bool choice;
			uint old_hist;
			printf(" T:%1d\n",taken);
			// retrieve predictions
			local = local_predict[br->instruction_addr & PC_INDEX]->get_sign();
			global = global_predict[path_history & PATH_HIST]->get_sign();
			choice = choice_predict[path_history & PATH_HIST]->get_sign();
			old_hist = path_history;

			path_history = path_history << 1;
			if (taken) {
				// update choice
				if (local && !global)
					choice_predict[old_hist & PATH_HIST]->decrement();
				else if (!local && global)
					choice_predict[old_hist & PATH_HIST]->increment();

				// update local, global and path history
				local_predict[br->instruction_addr & PC_INDEX]->increment();
				global_predict[br->instruction_addr & PATH_HIST]->increment();
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
				global_predict[br->instruction_addr & PATH_HIST]->decrement();
				path_history &= (PC_INDEX - 1);
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

	for (int i=0; i < LOCAL_PRED_SIZE; i++)
		local_predict[i]->reset();
	for (int i=0; i < GLOBAL_PRED_SIZE; i++)
		global_predict[i]->reset();
	for (int i=0; i < CHOICE_PRED_SIZE; i++)
		choice_predict[i]->reset();
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




