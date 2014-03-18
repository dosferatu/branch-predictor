/* predictor.h   
 * Description: This file defines the two required functions for the branch predictor.
 *
 * Andrei Kniazev, Michael Walton, Kevin Bedrossian, Benjamin Huntsman
 * ECE 486/586 final project
 * March 19, 2014
 */

#ifndef PREDICTOR_H_SEEN
#define PREDICTOR_H_SEEN

#define DEBUG 0
#define PC_INDEX 0x3FF
#define PATH_HIST 0xFFF
#define LOCAL_PRED_SIZE 1024
#define LOCAL_PRED_BITS 3
#define GLOBAL_PRED_SIZE 4096
#define GLOBAL_PRED_BITS 2
#define CHOICE_PRED_SIZE 4096
#define CHOICE_PRED_BITS 2


#include <cstddef>
#include <cstring>
#include <inttypes.h>
#include <vector>
#include "op_state.h"   // defines op_state_c (architectural state) class 
#include "tread.h"      // defines branch_record_c class
#include "scount.h"		// defines SATCOUNTER class

class PREDICTOR
{
private:
	SATCOUNTER **local_predict;
	SATCOUNTER **global_predict;
	SATCOUNTER **choice_predict;
	uint path_history;
public:
    PREDICTOR();
	~PREDICTOR();
	bool get_prediction(const branch_record_c* br, const op_state_c* os);

    void update_predictor(const branch_record_c* br, const op_state_c* os, bool taken);

};


#endif // PREDICTOR_H_SEEN

