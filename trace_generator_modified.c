/**************************************************************/
/* CS/COE 1541				 			
AThis program allows the user to input the instructions for a trace 
and produce a trace file with these instructions readable by CPU.c.
The program takes the name of the file to be generated as an argument.
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 

int main(int argc, char **argv)
{
  struct trace_item *tr_entry=malloc(sizeof(struct trace_item));
  size_t size;
  char *trace_file_name;
  int trace_view_on = 1;
  
  unsigned char t_type ;
  unsigned char t_sReg_a;
  unsigned char t_sReg_b;
  unsigned char t_dReg;
  unsigned int t_PC ;
  unsigned int t_Addr ;

  unsigned int cycle_number = 0;

  if (!(argc == 3)) {
    fprintf(stdout, "\nMissing argument: the name of the file to be generated or run type\n");
    exit(0);
  }
  trace_file_name = argv[1];
  fprintf(stderr, "\nArgv2 = %s!!\n", argv[2]);
  
	char type[4] = {'R','L','S','B'};
	srand(time(0));

	int check;
int trcount = 6, i, repeat;
char itype ;

int typepat[50];
char regApat[50];
char regBpat[50];
char regDpat[50];
int addrpat[50];

// Randomly generated test files
if(strcmp(argv[2],"1") == 0) {
	trcount = rand()%100;
}
//Data Hazards
else if(strcmp(argv[2], "2") == 0) {
	fprintf(stderr,"Creating Data Hazards\n");
	trcount = 6;

	typepat[0] = 0; // R
	regApat[0] = 3;
	regBpat[0] = 2;
	regDpat[0] = 1;
	addrpat[0] = 0;

	typepat[1] = 1; // L 
	regApat[1] = 1;
	regBpat[1] = 1;
	regDpat[1] = 4;
	addrpat[1] = 0;
	
	typepat[2] = 1; // L
	regApat[2] = 5;
	regBpat[2] = 5;
	regDpat[2] = 7;
	addrpat[2] = 0;
	
	typepat[3] = 0; // R
	regApat[3] = 7;
	regBpat[3] = 8;
	regDpat[3] = 9;
	addrpat[3] = 0;
	
	typepat[4] = 0; // R
	regApat[4] = 9;
	regBpat[4] = 10;
	regDpat[4] = 11;
	addrpat[4] = 0;
	
	typepat[5] = 1; // L
	regApat[5] = 11;
	regBpat[5] = 11;
	regDpat[5] = 12;
	addrpat[5] = 0;
}
// Structural Hazard
else if(strcmp(argv[2], "3") == 0) {
	fprintf(stderr,"Creating Structural Hazards\n");
	trcount = 6;

	typepat[0] = 0; // R
	fprintf(stderr, "Typepat = %c\n", typepat[0]);
	regApat[0] = 3;
	regBpat[0] = 2;
	regDpat[0] = 1;
	addrpat[0] = 0;

	typepat[1] = 0; // R
	regApat[1] = 3;
	regBpat[1] = 2;
	regDpat[1] = 1;
	addrpat[1] = 0;
	
	typepat[2] = 0; // R
	regApat[2] = 3;
	regBpat[2] = 2;
	regDpat[2] = 1;
	addrpat[2] = 0;
	
	typepat[3] = 0; // R
	regApat[3] = 3;
	regBpat[3] = 2;
	regDpat[3] = 1;
	addrpat[3] = 0;
	
	typepat[4] = 0; // R
	regApat[4] = 3;
	regBpat[4] = 2;
	regDpat[4] = 1;
	addrpat[4] = 0;
	
	typepat[5] = 0; // R
	regApat[5] = 3;
	regBpat[5] = 2;
	regDpat[5] = 1;
	addrpat[5] = 0;
}

// Control Hazard
else if(strcmp(argv[2], "4") == 0) {
	fprintf(stderr,"Creating Control Hazards\n");
	trcount = 6;

	typepat[0] = 3; // B
	regApat[0] = 5;
	regBpat[0] = 5;
	regDpat[0] = 1;
	addrpat[0] = 100;

	typepat[1] = 0; // R
	regApat[1] = 1;
	regBpat[1] = 2;
	regDpat[1] = 3;
	addrpat[1] = 0;
	
	typepat[2] = 0; // R
	regApat[2] = 1;
	regBpat[2] = 2;
	regDpat[2] = 3;
	addrpat[2] = 0;
	
	typepat[3] = 0; // R
	regApat[3] = 1;
	regBpat[3] = 2;
	regDpat[3] = 3;
	addrpat[3] = 0;
	
	typepat[4] = 0; // R
	regApat[4] = 1;
	regBpat[4] = 2;
	regDpat[4] = 3;
	addrpat[4] = 0;
	
	typepat[5] = 1; // L
	regApat[5] = 4;
	regBpat[5] = 4;
	regDpat[5] = 5;
	addrpat[5] = 0;
}

// Structural + Data Hazard
else if(strcmp(argv[2], "5") == 0) {
	fprintf(stderr,"Creating Structural + Data Hazards\n");
	trcount = 6;

	typepat[0] = 0; // L
	regApat[0] = 5;
	regBpat[0] = 5;
	regDpat[0] = 2;
	addrpat[0] = 0;

	typepat[1] = 0; // R
	regApat[1] = 3;
	regBpat[1] = 2;
	regDpat[1] = 1;
	addrpat[1] = 0;
	
	typepat[2] = 0; // R
	regApat[2] = 3;
	regBpat[2] = 2;
	regDpat[2] = 1;
	addrpat[2] = 0;
	
	typepat[3] = 0; // R
	regApat[3] = 3;
	regBpat[3] = 2;
	regDpat[3] = 1;
	addrpat[3] = 0;
	
	typepat[4] = 0; // R
	regApat[4] = 3;
	regBpat[4] = 2;
	regDpat[4] = 1;
	addrpat[4] = 0;
	
	typepat[5] = 0; // R
	regApat[5] = 3;
	regBpat[5] = 2;
	regDpat[5] = 1;
	addrpat[5] = 0;
}

// Data + Cntrl Hazard
else if(strcmp(argv[2], "6") == 0) {
	fprintf(stderr,"Creating Data + Control Hazards\n");
	trcount = 6;

	typepat[0] = 1; // L
	regApat[0] = 4;
	regBpat[0] = 4;
	regDpat[0] = 5;
	addrpat[0] = 0;

	typepat[1] = 3; // B
	regApat[1] = 5;
	regBpat[1] = 6;
	regDpat[1] = 0;
	addrpat[1] = 7000;
	
	typepat[2] = 0; // R
	regApat[2] = 3;
	regBpat[2] = 2;
	regDpat[2] = 1;
	addrpat[2] = 0;
	
	typepat[3] = 0; // R
	regApat[3] = 3;
	regBpat[3] = 2;
	regDpat[3] = 1;
	addrpat[3] = 0;
	
	typepat[4] = 0; // R
	regApat[4] = 3;
	regBpat[4] = 2;
	regDpat[4] = 1;
	addrpat[4] = 0;
	
	typepat[5] = 0; // R
	regApat[5] = 3;
	regBpat[5] = 2;
	regDpat[5] = 1;
	addrpat[5] = 0;
}

// Control + Structural Hazard
else if(strcmp(argv[2], "7") == 0) {
	fprintf(stderr,"Creating Control + Structural Hazards\n");
	trcount = 9;

	typepat[0] = 0; // R
	regApat[0] = 3;
	regBpat[0] = 2;
	regDpat[0] = 1;
	addrpat[0] = 0;

	typepat[1] = 0; // R
	regApat[1] = 3;
	regBpat[1] = 2;
	regDpat[1] = 1;
	addrpat[1] = 0;
	
	typepat[2] = 0; // R
	regApat[2] = 3;
	regBpat[2] = 2;
	regDpat[2] = 1;
	addrpat[2] = 0;
	
	typepat[3] = 0; // R
	regApat[3] = 3;
	regBpat[3] = 2;
	regDpat[3] = 1;
	addrpat[3] = 0;
	
	typepat[4] = 0; // R
	regApat[4] = 3;
	regBpat[4] = 2;
	regDpat[4] = 1;
	addrpat[4] = 0;
	
	typepat[5] = 0; // B
	regApat[5] = 5;
	regBpat[5] = 6;
	regDpat[5] = 0;
	addrpat[5] = 7000;
	
	typepat[6] = 0; // R
	regApat[6] = 3;
	regBpat[6] = 2;
	regDpat[6] = 1;
	addrpat[6] = 0;
	
	typepat[7] = 0; // R
	regApat[7] = 3;
	regBpat[7] = 2;
	regDpat[7] = 1;
	addrpat[7] = 0;
	
	typepat[8] = 0; // R
	regApat[8] = 3;
	regBpat[8] = 2;
	regDpat[8] = 1;
	addrpat[8] = 0;
}

// All Hazard
else if(strcmp(argv[2], "8") == 0) {
	fprintf(stderr,"Creating All Hazards\n");
	trcount = 9;

	typepat[0] = 0; // R
	regApat[0] = 3;
	regBpat[0] = 2;
	regDpat[0] = 1;
	addrpat[0] = 0;

	typepat[1] = 1; // L 
	regApat[1] = 5;
	regBpat[1] = 5;
	regDpat[1] = 4;
	addrpat[1] = 0;
	
	typepat[2] = 1; // L
	regApat[2] = 3;
	regBpat[2] = 3;
	regDpat[2] = 5;
	addrpat[2] = 0;
	
	typepat[3] = 3; // B
	regApat[3] = 4;
	regBpat[3] = 5;
	regDpat[3] = 0;
	addrpat[3] = 6000;
	
	typepat[4] = 0; // R
	regApat[4] = 3;
	regBpat[4] = 2;
	regDpat[4] = 1;
	addrpat[4] = 0;
	
	typepat[5] = 0; // R
	regApat[5] = 3;
	regBpat[5] = 2;
	regDpat[5] = 1;
	addrpat[5] = 0;
	
	typepat[6] = 0; // R
	regApat[6] = 3;
	regBpat[6] = 2;
	regDpat[6] = 1;
	addrpat[6] = 0;
	
	typepat[7] = 0; // R
	regApat[7] = 3;
	regBpat[7] = 2;
	regDpat[7] = 1;
	addrpat[7] = 0;
	
	typepat[8] = 0; // R
	regApat[8] = 3;
	regBpat[8] = 2;
	regDpat[8] = 1;
	addrpat[8] = 0;
}

// Unsquashed Control Hazard
else if(strcmp(argv[2], "9") == 0) {
	fprintf(stderr,"Creating Unsquashed Control Hazard\n");
	trcount = 6;

	typepat[0] = 0; // B
	regApat[0] = 4;
	regBpat[0] = 1;
	regDpat[0] = 0;
	addrpat[0] = 6000;

	typepat[1] = 0; // R
	regApat[1] = 3;
	regBpat[1] = 2;
	regDpat[1] = 1;
	addrpat[1] = 0;
	
	typepat[2] = 0; // R
	regApat[2] = 3;
	regBpat[2] = 2;
	regDpat[2] = 1;
	addrpat[2] = 0;
	
	typepat[3] = 0; // R
	regApat[3] = 3;
	regBpat[3] = 2;
	regDpat[3] = 1;
	addrpat[3] = 0;
	
	typepat[4] = 0; // R
	regApat[4] = 3;
	regBpat[4] = 2;
	regDpat[4] = 1;
	addrpat[4] = 0;
	
	typepat[5] = 0; // R
	regApat[5] = 3;
	regBpat[5] = 2;
	regDpat[5] = 1;
	addrpat[5] = 0;
}

// Repeat R-Type
else if(strcmp(argv[2], "10") == 0) {
	fprintf(stderr,"Repeat R Type\n");
	trcount = 6;

	typepat[0] = 0; // R
	regApat[0] = 3;
	regBpat[0] = 2;
	regDpat[0] = 1;
	addrpat[0] = 0;

	typepat[1] = 0; // R
	regApat[1] = 3;
	regBpat[1] = 2;
	regDpat[1] = 1;
	addrpat[1] = 0;
	
	typepat[2] = 0; // R
	regApat[2] = 3;
	regBpat[2] = 2;
	regDpat[2] = 1;
	addrpat[2] = 0;
	
	typepat[3] = 0; // R
	regApat[3] = 4;
	regBpat[3] = 5;
	regDpat[3] = 6;
	addrpat[3] = 0;
	
	typepat[4] = 0; // R
	regApat[4] = 4;
	regBpat[4] = 5;
	regDpat[4] = 6;
	addrpat[4] = 0;
	
	typepat[5] = 0; // R
	regApat[5] = 4;
	regBpat[5] = 5;
	regDpat[5] = 6;
	addrpat[5] = 0;
}
// Repeat L-Type
else if(strcmp(argv[2], "11") == 0) {
	fprintf(stderr,"Repeat L Type\n");
	trcount = 6;

	typepat[0] = 1; // L
	regApat[0] = 2;
	regBpat[0] = 2;
	regDpat[0] = 1;
	addrpat[0] = 0;

	typepat[1] = 1; // L
	regApat[1] = 2;
	regBpat[1] = 2;
	regDpat[1] = 1;
	addrpat[1] = 0;
	
	typepat[2] = 1; // L
	regApat[2] = 2;
	regBpat[2] = 2;
	regDpat[2] = 1;
	addrpat[2] = 0;
	
	typepat[3] = 1; // L
	regApat[3] = 1;
	regBpat[3] = 1;
	regDpat[3] = 4;
	addrpat[3] = 0;
	
	typepat[4] = 1; // L
	regApat[4] = 5;
	regBpat[4] = 5;
	regDpat[4] = 6;
	addrpat[4] = 0;
	
	typepat[5] = 1; // L
	regApat[5] = 5;
	regBpat[5] = 5;
	regDpat[5] = 6;
	addrpat[5] = 0;
}
// Repeat S-Type
else if(strcmp(argv[2], "12") == 0) {
	fprintf(stderr,"Repeat S Type\n");
	trcount = 6;

	typepat[0] = 2; // S
	regApat[0] = 1;
	regBpat[0] = 1;
	regDpat[0] = 2;
	addrpat[0] = 0;

	typepat[1] = 2; // S
	regApat[1] = 2;
	regBpat[1] = 2;
	regDpat[1] = 3;
	addrpat[1] = 0;
	
	typepat[2] = 2; // S
	regApat[2] = 2;
	regBpat[2] = 2;
	regDpat[2] = 3;
	addrpat[2] = 0;
	
	typepat[3] = 2; // S
	regApat[3] = 3;
	regBpat[3] = 3;
	regDpat[3] = 4;
	addrpat[3] = 0;
	
	typepat[4] = 2; // S
	regApat[4] = 5;
	regBpat[4] = 5;
	regDpat[4] = 6;
	addrpat[4] = 0;
	
	typepat[5] = 2; // S
	regApat[5] = 7;
	regBpat[5] = 7;
	regDpat[5] = 8;
	addrpat[5] = 0;
}
// Repeat B-Type
else if(strcmp(argv[2], "13") == 0) {
	fprintf(stderr,"Repeat B Type\n");
	trcount = 6;

	typepat[0] = 3; // B
	regApat[0] = 3;
	regBpat[0] = 2;
	regDpat[0] = 0;
	addrpat[0] = 10;

	typepat[1] = 3; // B 
	regApat[1] = 3;
	regBpat[1] = 3;
	regDpat[1] = 0;
	addrpat[1] = 100;
	
	typepat[2] = 3; // B
	regApat[2] = 3;
	regBpat[2] = 4;
	regDpat[2] = 0;
	addrpat[2] = 1000;
	
	typepat[3] = 3; // B
	regApat[3] = 4;
	regBpat[3] = 5;
	regDpat[3] = 0;
	addrpat[3] = 20;
	
	typepat[4] = 3; // B
	regApat[4] = 6;
	regBpat[4] = 7;
	regDpat[4] = 0;
	addrpat[4] = 200;
	
	typepat[5] = 3; // B
	regApat[5] = 8;
	regBpat[5] = 9;
	regDpat[5] = 0;
	addrpat[5] = 2000;
}

fprintf(stderr,"%d instructions!\n", trcount);

for (i = 0 ; i < trcount ; i++) {
	
	if(strcmp(argv[2], "1") == 0) // Random tests
	{	
		itype = type[rand()%4];
		tr_entry->dReg = (char) rand()%32;
		tr_entry->sReg_a = (char) rand()%32;
		tr_entry->sReg_b = (char) rand()%32;
		tr_entry->PC = i;
		tr_entry->Addr = rand();
	}
	else // Anything else
	{
		itype = type[typepat[i]];
		tr_entry->dReg = regDpat[i];
		tr_entry->sReg_a = regApat[i];
		tr_entry->sReg_b = regBpat[i];
		tr_entry->PC = i;
		tr_entry->Addr = addrpat[i];
	}
	fprintf(stderr,"itype = %c\n",itype);
repeat = 0 ;
if(itype == 'R') {tr_entry->type = ti_RTYPE ;} 
  else if (itype == 'L') {tr_entry->type = ti_LOAD;} 
  else if (itype == 'S') {tr_entry->type = ti_STORE;} 
  else if (itype == 'B') {tr_entry->type = ti_BRANCH ;} 
  else {printf("unrecognized instruction type\n") ; repeat = 1;  i-- ; break;}
if (repeat == 0) write_trace(*tr_entry, trace_file_name);
} 
trace_fd = fopen(trace_file_name, "rb");
trace_init();
  while(1) {
    size = trace_get_item(&tr_entry);
   
    if (!size) {       /* no more instructions (trace_items) to simulate */
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
    }  

// Display the generated trace 

    if (trace_view_on) {/* print the executed instruction if trace_view_on=1 */
      switch(tr_entry->type) {
        case ti_NOP:
          printf("[cycle %d] NOP:",cycle_number) ;
          break;
        case ti_RTYPE:
          printf("[cycle %d] RTYPE:",cycle_number) ;
          printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->dReg);
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
      }
    }
  }

  trace_uninit();

  exit(0);
}