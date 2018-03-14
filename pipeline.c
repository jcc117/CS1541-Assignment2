/**************************************************************/
/* CS/COE 1541				 			
   just compile with gcc -o pipeline pipeline.c			
   and execute using							
   ./pipeline  /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr	0  
***************************************************************/

#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h"

/*Helper method to get the hash index for branch table(s)*/
int hash(unsigned int pc)
{
  return (pc & 0x000001f8) >> 3;
}

/*Locates a branch instruction in the table and returns a prediction if the branch was taken or not.
Returns 1 for taken, 0 for not taken*/
int predict_branch(unsigned int pc, unsigned int t_address, int prediction_method, int rows, int cols, unsigned int branch_table[rows][cols])
{
  //Predict never taken
  if(prediction_method == 0)
  {
    return 0;
  }
  //1 bit table
  else if(prediction_method == 1)
  {
    /*Column 0 is 1 for taken, 0 for not taken*/
    unsigned int index = hash(pc); //Find index of the pc in the table

    //printf("Index: %d\n", index);

    //Match - Predict taken
    if(branch_table[index][2] == pc && branch_table[index][0] == 1)
    {
      //printf("prediction: %d\n", 1);
      return 1;
    }
    //Match - Predict not taken
    else if(branch_table[index][2] == pc && branch_table[index][0] == 0)
    {
     // printf("prediction: %d\n", 0);
      return 0;
    }
    //No match - predict not taken and store pc in table
    else if(branch_table[index][2] != pc)
    {
      branch_table[index][2] = pc;
      branch_table[index][0] = 0;
      branch_table[index][3] = t_address;
      //printf("prediction: %d\n", 0);
      return 0;
    }
  }
  //2 bit table
  else
  {
    int index = hash(pc);
    //PC is in the table
    if(branch_table[index][2] == pc)
    {
      //Predict not taken
      if(branch_table[index][0] == 0 && branch_table[index][1] == 0)
      {
        //printf("prediction: %d\n", 0);
        return 0;
      }
      //Predict not taken
      else if(branch_table[index][0] == 0 && branch_table[index][1] == 1)
      {
        //printf("prediction: %d\n", 0);
        return 0;
      }
      //Predict taken
      else if(branch_table[index][0] == 1 && branch_table[index][1] == 0)
      {
        //printf("prediction: %d\n", 1);
        return 1;
      }
      //Predict taken
      else
      {
        //printf("prediction: %d\n", 1);
        return 1;
      }
    }
    //PC is not in the table
    else
    {
      branch_table[index][2] = pc;
      branch_table[index][3] = t_address;
      branch_table[index][0] = 0;
      branch_table[index][1] = 0;
      //printf("prediction: %d\n", 0);
      return 0;
    }
  }
}

/*Updates the branch prediction table after determining if the branch was predicted correctly or not.
prediction is 1 for taken, 0 for not taken*/
void update_branch(unsigned int pc, unsigned int t_address, int prediction_method, int prediction, int rows, int cols, unsigned int branch_table[rows][cols])
{
	int index = hash(pc);
  //1 bit table
  if(prediction_method == 1)
  {
  	if(branch_table[index][0] != prediction) //Reassign the data
  	{
  		branch_table[index][0] = prediction;
      //printf("Reassignment: %d\tIndex:%d\n", prediction, branch_table[index][0]);
  	}
  }
  //2 bit table
  else if(prediction_method == 2)
  {
  	if(branch_table[index][0] == 0) //Bit 0 is 0
  	{
  		if(prediction==1&&(branch_table[index][1]==0))  //Bit 1 is 0 and taken
  		{
  			branch_table[index][1] = 1;
  		}
  		else if(prediction==1&&(branch_table[index][1]==1)) //Bit 1 is 1 and taken
  		{
  			branch_table[index][0] = 1; 
  		}
  		else if(prediction==0&&(branch_table[index][1]==0)) //Bit 1 is 0 and not taken
  		{
  			//Do nothing, correct prediction
  		}
  		else if(prediction==0&&(branch_table[index][1]==1)) //Bit 1 is 1 and not taken
  		{
  			branch_table[index][1] = 0;
  		}
  	}
  	else if(branch_table[index][0] == 1)  //Bit 0 is 1
  	{
  		if(prediction==1&&(branch_table[index][1]==0))  //Bit 1 is 0 and taken
  		{
  			branch_table[index][1] = 1;
  		}
  		else if(prediction==0&&(branch_table[index][1]==0)) //Bit 1 is 0 and not taken
  		{
  			branch_table[index][0] = 0;
  		}
      else if(prediction==1&&(branch_table[index][1]==1)) //Bit 1 is 1 and taken
      {
        //do nothing, correct prediction
      }
      else if(prediction==0&&(branch_table[index][1]==1)) //Bit 1 is 1 and not taken
      {
        branch_table[index][1] = 0;
      }
  	}
  }
  /*On prediction_method == 0, do nothing*/
}

/*Print the contents of the stage of the pipeline*/
int print_stage(struct trace_item *tr_entry, int cycle_number)
{
  switch(tr_entry->type) {
      case ti_NOP:
        printf("[cycle %d] NOP\n:",cycle_number) ;
        break;
      case ti_RTYPE:
        printf("[cycle %d] RTYPE:",cycle_number) ;
        printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->dReg);
        break;
      case ti_ITYPE:
        printf("[cycle %d] ITYPE:",cycle_number) ;
        printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
        break;
      case ti_LOAD:
        printf("[cycle %d] LOAD:",cycle_number) ;      
        printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
        break;
      case ti_STORE:
        printf("[cycle %d] STORE:",cycle_number) ;      
        printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
        break;
      case ti_BRANCH:
        printf("[cycle %d] BRANCH:",cycle_number) ;
        printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
        break;
      case ti_JTYPE:
        printf("[cycle %d] JTYPE:",cycle_number) ;
        printf(" (PC: %x)(addr: %x)\n", tr_entry->PC,tr_entry->Addr);
        break;
      case ti_SPECIAL:
        printf("[cycle %d] SPECIAL:\n",cycle_number) ;        
        break;
      case ti_JRTYPE:
        printf("[cycle %d] JRTYPE:",cycle_number) ;
        printf(" (PC: %x) (sReg_a: %d)(addr: %x)\n", tr_entry->PC, tr_entry->dReg, tr_entry->Addr);
        break;
    }
}

//Check that there are still instructions in the pipe
//Returns 1 if it is empty, 0 if not
int is_empty(char if1_if2, char if2_id, char id_ex, char ex_mem1, char mem1_mem2, char mem2_wb, char exit_item)
{
  if(if1_if2 == ti_NOP && if2_id == ti_NOP && id_ex == ti_NOP && ex_mem1 == ti_NOP && mem1_mem2 == ti_NOP && mem2_wb == ti_NOP && exit_item == ti_NOP)
    return 1;
  return 0;
}

int main(int argc, char **argv)
{
  struct trace_item *tr_entry;
  size_t size;
  char *trace_file_name;
  int trace_view_on = 0;
  int prediction_method = 0;
  
  
  unsigned char t_type = 0;
  unsigned char t_sReg_a= 0;
  unsigned char t_sReg_b= 0;
  unsigned char t_dReg= 0;
  unsigned int t_PC = 0;
  unsigned int t_Addr = 0;

  unsigned int cycle_number = 0;

  int table_size = 64;

  /*Branch prediction table
    First 2 columns represent the 2 bits to predict from
    Second 2 columns represent PC of branch instruction and address respectively
    If using a 1 bit predictor, use column 0 and disregard column 1*/
  unsigned int branch_table[table_size][4];

  if (argc == 1) {
    fprintf(stdout, "\nUSAGE: tv <trace_file> <predictor - integer value of 0, 1 or 2> <switch - any character>\n");
    fprintf(stdout, "\n(switch) to turn on or off individual item view.\n");
    fprintf(stdout, "\n(predictor) to select a branch prediction method\n\n");
    exit(0);
  }
    
  trace_file_name = argv[1];
  if (argc == 3) prediction_method = atoi(argv[2]) ;

  if (argc == 4)
  {
    trace_view_on = atoi(argv[3]);
    prediction_method = atoi(argv[2]);
  }

  fprintf(stdout, "\n ** opening file %s\n", trace_file_name);

  trace_fd = fopen(trace_file_name, "rb");

  if (!trace_fd) {
    fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
    exit(0);
  }

  trace_init();

  struct trace_item noop;
  noop.type = ti_NOP;
  noop.dReg = 255;
  noop.sReg_a = 255;
  noop.sReg_b = 255;

  //Create buffers for each stage
  struct trace_item buffer[10000]; //Holds an extra instructions for a flush of an arbitrary size
  struct trace_item if1_if2 = noop;
  struct trace_item if2_id = noop;
  struct trace_item id_ex = noop;
  struct trace_item ex_mem1 = noop;
  struct trace_item mem1_mem2 = noop;
  struct trace_item mem2_wb = noop;
  struct trace_item exit_item = noop;
  struct trace_item real_exit = noop;

  int d_stall = 0;  //Flag to trip if the pipeline is to be stalled for data hazard
  int s_stall = 0; //Flag to trip if the pipeline stalls at the structural hazard
  int flush_p = 0;
  int counter = 0; //Keep track of buffer index

  while(1) {
    if(d_stall == 0 && s_stall == 0 && counter == 0)
    {
      size = trace_get_item(&tr_entry);
    }
    d_stall = 0;
    s_stall = 0;
    flush_p = 0;

    if(!size && counter == 0) //While the pipeline still isn't empty, send it noops
      tr_entry = &noop;
   
    if (!size && is_empty(if1_if2.type, if2_id.type, id_ex.type, ex_mem1.type, mem1_mem2.type, mem2_wb.type, exit_item.type)) {       /* no more instructions (trace_items) to simulate */
      printf("+ Simulation terminates at cycle : %u\n", cycle_number);
      break;
    }
    else{              /* parse the next instruction to simulate */
      cycle_number++;
      t_type = tr_entry->type;
      t_sReg_a = tr_entry->sReg_a;
      t_sReg_b = tr_entry->sReg_b;
      t_dReg = tr_entry->dReg;
      t_PC = tr_entry->PC;
      t_Addr = tr_entry->Addr;

      /*Check for all types of hazards*/

      /*Check for the structural hazard*/
      //Not sure which version is correct yet
      //if((mem2_wb.dReg == if2_id.sReg_a && mem2_wb.dReg != 255 && if2_id.sReg_a != 255) || (mem2_wb.dReg == if2_id.sReg_b && mem2_wb.dReg != 255 && if2_id.sReg_b != 255))
      if((mem2_wb.dReg != 255 && if2_id.sReg_a != 255) || (mem2_wb.dReg != 255 && if2_id.sReg_b != 255))
      {
        s_stall = 1;
      }

      /*Check data hazard b*/
      if(mem1_mem2.type == ti_LOAD && (mem1_mem2.dReg == id_ex.sReg_a || mem1_mem2.dReg == id_ex.sReg_b) && id_ex.type != ti_NOP)
      {
        d_stall = 1;
      }

      /*Check for data hazard a*/
      if(ex_mem1.type == ti_LOAD && (ex_mem1.dReg == id_ex.sReg_a || ex_mem1.dReg == id_ex.sReg_b) && id_ex.type != ti_NOP)
      {
        d_stall = 1;
      }
      //Check for a control hazard
      if(id_ex.type == ti_BRANCH)
      {
        int prediction = predict_branch(id_ex.PC, id_ex.Addr, prediction_method, table_size, 4, branch_table);
        //Predict taken, actually taken
  			if(prediction == 1 && (abs(id_ex.Addr - if2_id.PC) > 4))
  			{
  				//call update_table to make branch prediction table match
  				update_branch(id_ex.PC, id_ex.Addr, prediction_method, 1, table_size, 4, branch_table);
  			}
          //Predict taken, actually not taken
  			else if(prediction == 1 && !(abs(id_ex.Addr - if2_id.PC) > 4))
  			{
  				//call update_table to make branch prediction table match
  				update_branch(id_ex.PC, id_ex.Addr, prediction_method, 0, table_size, 4, branch_table);
          flush_p = 1;
  			}
          //Predict not taken, actually not taken
  			else if(prediction == 0 && !(abs(id_ex.Addr - if2_id.PC) > 4))
  			{
  				//call update_table to make branch prediction table match
  				update_branch(id_ex.PC, id_ex.Addr, prediction_method, 0, table_size, 4, branch_table);
  			}
          //Predict not taken, actually taken
  			else if(prediction == 0 && (abs(id_ex.Addr - if2_id.PC) > 4))
  			{
  				//call update_table to make branch prediction table match
  				update_branch(id_ex.PC, id_ex.Addr, prediction_method, 1, table_size, 4, branch_table);
          flush_p = 1;
  			}
      }

      //Process the instructions according to what types of hazards exist currently in the pipeline
      if(d_stall && s_stall)  //Stall at ex_mem1, precedence for a data stall
      {
        real_exit = exit_item;
        exit_item = mem2_wb;
        mem2_wb = mem1_mem2;
        mem1_mem2 = ex_mem1;
        ex_mem1 = noop;
      }
      else if(d_stall)  //Insert bubble at ex_mem1
      {
        real_exit = exit_item;
        exit_item = mem2_wb;
        mem2_wb = mem1_mem2;
        mem1_mem2 = ex_mem1;
        ex_mem1 = noop;
      }
      else if(s_stall && !flush_p)  //Insert bubble at id_ex, if you need to flush the instructions it can be ignored
      {
        real_exit = exit_item;
        exit_item = mem2_wb;
        mem2_wb = mem1_mem2;
        mem1_mem2 = ex_mem1;
        ex_mem1 = id_ex;
        id_ex = noop;
      }
      else if(flush_p)  //Flush the instructions
      {
        //Install squashed instructions and store these ones in a buffer
        buffer[counter] = if2_id;
        buffer[counter + 1] = if1_if2;
        buffer[counter + 2] = *tr_entry;
        counter += 3;

        if2_id = noop;
        if1_if2 = noop;
        tr_entry = &noop;

        real_exit = exit_item;
        exit_item = mem2_wb;
        mem2_wb = mem1_mem2;
        mem1_mem2 = ex_mem1;
        ex_mem1 = id_ex;
        id_ex = if2_id;
        if2_id = if1_if2;
        if1_if2 = *tr_entry;
      }
      else  //No hazards, normal execution
      {
        real_exit = exit_item;
        exit_item = mem2_wb;
        mem2_wb = mem1_mem2;
        mem1_mem2 = ex_mem1;
        ex_mem1 = id_ex;
        if(counter == 0)  //Load instructions as normal
        {
          id_ex = if2_id;
          if2_id = if1_if2;
          if1_if2 = *tr_entry;
        }
        else  //Get instructions from the buffer
        {
          id_ex = if2_id;
          if2_id = if1_if2;
          if1_if2 = buffer[0];
          counter -= 1;

          //Shift everything in the buffer down by one
          if(counter > 0)
          {
            //printf("here\n");
            int i;
            for(i = 0; i < counter; i++)
            {
              buffer[i] = buffer[i + 1];
            }
          }
        }
      }
    }

    /*Don't worry about trace for now*/
    if (trace_view_on) {/* print the executed instruction if trace_view_on=1 */
      printf("IF1 STAGE: ");
      print_stage(&if1_if2, cycle_number);
      printf("IF2 STAGE: ");
      print_stage(&if2_id, cycle_number);
      printf("ID STAGE: ");
      print_stage(&id_ex, cycle_number);
      printf("EX STAGE: ");
      print_stage(&ex_mem1, cycle_number);
      printf("MEM1 STAGE: ");
      print_stage(&mem1_mem2, cycle_number);
      printf("MEM2 STAGE: ");
      print_stage(&mem2_wb, cycle_number);
      printf("WB STAGE: ");
      print_stage(&exit_item, cycle_number);
      printf("EXITING: ");
      print_stage(&real_exit, cycle_number);
      printf("==================================================\n\n");
    }

  }

  trace_uninit();

  exit(0);
}