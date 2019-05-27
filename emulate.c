#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define int8      unsigned char
#define int32      unsigned int
#include "emulate_struct.h"
#include "emulate.h"

//0xa0
//0b1011
int memory[65536] = {0};  //initialise to 0, store instructions and data
int reg[17] = {0};  //0-12, 15, 16 are useful, store registers
int PC = 0x00000000;
CPSR_type CPSR; 
instruction instr;
int data_length = 32; //data have 32 bits
int shift_carry_out = 0;
int add_carry_out = 0;

int main(int argc, char *argv[]) {	
  FILE *file = fopen(argv[1], "rb");  //rb means read binary
  int8 *buffer = malloc(sizeof(int8));  
  int counter = 0;
  //int pc = 0;
  
  FILE* fplog = NULL;
  int i = 0;
  CPSR.CPSR_32bits = 0x00000000;

  fplog = fopen("output.out", "w+");

  while(fread(buffer, sizeof(int8), 1, file) == 1){  
    memory[counter++] = *buffer;  //store instructions into the memory
  }
  //printf("%d \n", counter);
  counter = counter/4;	

  /*
  for(int i=0;i!= counter;i++)  //memory: 8 bit per block, 4 block per instruction
  {
	instr.instr_Byt.Byt0 = memory[4*i];
	instr.instr_Byt.Byt1 = memory[4*i+1];
	instr.instr_Byt.Byt2 = memory[4*i+2];
	instr.instr_Byt.Byt3 = memory[4*i+3];
	fprintf(fplog," instruction is 0x%08x\n",instr.instr_32bits);
	check_instr_set();
	PC+= 4;
	fprintf(fplog," instruction is 0x%08x\n",instr.instr_32bits);
  }
  PC+= 8;
  */

  for(;PC < 4 * counter;PC += 4)  //memory: 8 bit per block, 4 block per instruction
  {
	instr.instr_Byt.Byt0 = memory[PC];
	instr.instr_Byt.Byt1 = memory[PC + 1];
	instr.instr_Byt.Byt2 = memory[PC + 2];
	instr.instr_Byt.Byt3 = memory[PC + 3];
	fprintf(fplog," instruction is 0x%08x\n",instr.instr_32bits);
	check_instr_set();
	fprintf(fplog," instruction is 0x%08x\n",instr.instr_32bits);
  }
  PC+= 8;
  
  fprintf(fplog, "Register: \n");
  for(int x = 0; x < 13; x++) {
    fprintf(fplog, "$%d :  %d(0x%08x) \n", x, read_register(x), read_register(x));
  }
  fprintf(fplog, "PC  :  %d(0x%08x) \n", PC, PC);
  fprintf(fplog, "CPSR:  %d(0x%08x) \n", CPSR.CPSR_32bits, CPSR.CPSR_32bits);
  
  free(buffer);  //delete buffer
  fclose(file);  //close file

  fprintf(fplog,"Non-zero memory:\n");
  for(int i=0;i!= counter;i++)
  {
  	fprintf(fplog,"0x%08x: 0x%02x%02x%02x%02x\n",i*4,memory[4*i],memory[4*i+1],memory[4*i+2],memory[4*i+3]);
   }
 
  fclose(fplog);
  return 0;
}

void check_instr_set() { //check which type of instructions it is
	if(instr.instr_set.bit2726 == 0){
    	if((instr.instr_set.bit2524 == 00)&&(instr.instr_set.bit23 == 0)&&(instr.instr_set.bit22 == 0)&&(instr.instr_set.bit74 == 9)) {
        
      		multiply();
    	}else {
      		data_processing();
    	}
  	}else if(instr.instr_set.bit2726 == 1){
		if((instr.instr_set.bit22 == 0)&&(instr.instr_set.bit21 == 0)) {
			single_data_transfer();
      	}else{
			report_err_instruction();
      	}
    }else if(instr.instr_set.bit2726 == 2){
		if(instr.instr_set.bit2524 == 2) {
        
			branch();
		}else{
			report_err_instruction();
		}
    }
    else
		report_err_instruction();
}

//check_cond() ---wrong
void data_processing(){

    int operand1 = read_register(instr.data_pro.Rn);
    int operand2 = get_operand2();
    int result;
    int C;
    int Z;
    int V;

  if(check_cond(instr.instr_set.Cond)){
   
    switch(instr.data_pro.OpCode) {
    case 0:
      result = operand1&operand2;
      write_register(instr.data_pro.Rd, result);
      break;
    case 1:
      result = operand1 ^ operand2;  //eor??
      write_register(instr.data_pro.Rd, result);
      break;
    case 2:
      result = operand1 - operand2;
      write_register(instr.data_pro.Rd, result);
      break;
    case 3:
      result = operand2 - operand1;
      write_register(instr.data_pro.Rd, result);
      break;
    case 4:
      result = operand1 + operand2;
      if(result < operand1 || result < operand2){
	add_carry_out = 1;
      }else {
	add_carry_out = 0;
      }
      write_register(instr.data_pro.Rd, result);
      break;
    case 8:
      result = operand1&operand2;
      break;
    case 9:
      result = operand1 ^ operand2;
      break;
    case 10:
      result = operand1 - operand2;
      break;
    case 12:
      result = operand1 ^ operand2;
      write_register(instr.data_pro.Rd, result);
      break;
    case 13:
      result = operand2;
      write_register(instr.data_pro.Rd, result);
      break;
    }

  }

  if (instr.data_pro.S == 1) {
	    if (result == 0) {//xj
	    	CPSR.CPSR_Bits.Z =1;
	    }
    	
    	CPSR.CPSR_Bits.N = build_mask(result, 31, 1);
	        set_C(result);
  }
}


int get_operand2(){

  int value;
  if (instr.data_pro.I == 1) {
    value = instr.data_pro_imm.Imm;
    if (instr.data_pro_imm.Rotate != 0) {
      value <<= (32 - (2 * instr.data_pro_imm.Rotate));
   
    }
  }else {
    int value_Rm = read_register(instr.data_pro_reg.Rm);
    if (instr.data_pro_reg.bit4 == 0) {
      value = shift(instr.data_pro_reg.Shift_type, instr.data_pro_reg.Integer, value_Rm);
    }else {
      if (instr.data_pro_reg.Integer % 2 == 0) {
	value = read_register(instr.data_pro_reg.Rm);
      }else {
        int value_Rs = read_register(instr.data_pro_reg.Integer/2);
        value = shift(instr.data_pro_reg.Shift_type, value_Rs, value_Rm);
      }
    }
  }
  return value;
}


void set_C(int result) { //data processing helper function 

  printf("C value:%d \n", CPSR.CPSR_Bits.C);
  printf("result:%8x \n", result);

  switch(instr.data_pro.OpCode) {

  case 0:
  case 1:
  case 8:
  case 9:
  case 12:
  case 13:
    CPSR.CPSR_Bits.C = shift_carry_out; break;
  case 4:
    CPSR.CPSR_Bits.C = add_carry_out; break;
  defalt:
    CPSR.CPSR_Bits.C = 0;
  }

  printf("C value:%d \n", CPSR.CPSR_Bits.C);
  printf("%8x", instr.instr_32bits);


}


int shift(int shift_type, int shift_amount, int value) { //data processing helper functnion

  int bits;
  int bit_sign;
  int bits_low;
  int bits_high;

  switch(shift_type) {
  case 0:
    bits = build_mask(value, 0, data_length - shift_amount);
    if (instr.data_pro.S == 1) {
    shift_carry_out = build_mask(value, data_length - shift_amount, 1);
    }
    return bits << shift_amount;
  case 1:
    bits = build_mask(value, shift_amount, data_length - shift_amount);
    if (instr.data_pro.S == 1) {
    shift_carry_out = build_mask(value, shift_amount - 1, 1);
    }
    return bits;
  case 2:
    bits = build_mask(value, shift_amount, data_length - shift_amount - 1);
    bit_sign = build_mask(value, 31, 1);
    bit_sign << 31;
    if (instr.data_pro.S == 1) {
    shift_carry_out = build_mask(value, shift_amount - 1, 1);
    }
    return bits ^ bit_sign;
  case 3:
    bits_high = build_mask(value, shift_amount, data_length - shift_amount);
    bits_low = build_mask(value, 0, shift_amount);
    bits_low << (data_length - shift_amount);
    if (instr.data_pro.S == 1) {
    shift_carry_out = build_mask(value, shift_amount - 1, 1);
    }
    return bits_high ^ bits_low;
  }
}

void multiply() { //multiply instruction
  int content;
  if(check_cond(instr.Multiply.Cond)){
    
    if(instr.Multiply.A){
      content = read_register(instr.Multiply.Rm)*read_register(instr.Multiply.Rs)
	+read_register(instr.Multiply.Rn);
    } else {
      content = read_register(instr.Multiply.Rm)*read_register(instr.Multiply.Rs);
    }
    write_register(instr.Multiply.Rd, content);
    }
  
  if(instr.Multiply.S){
    if(read_register(instr.Multiply.Rd)<0){
      change_1_bit_content(instr.Multiply.Rd,31,1);
    } else {
      change_1_bit_content(instr.Multiply.Rd,31,0);
    }

    if(read_register(instr.Multiply.Rd)==0){
     change_1_bit_content(instr.Multiply.Rd,31,1); 
    } else {
     change_1_bit_content(instr.Multiply.Rd,31,1);
    }
    
  }
  
  printf("multiply");

}

void change_1_bit_content(int reg, int position, int content){ //helper function, change one bit in the register
  int n = (int)pow((double)10,(double) position);
  int result = read_register(reg);
  if(build_mask(read_register(reg), position, 1)){
    if(!content){
      result -= n;
    } 
  } else {
    if(content){
      result += n;
    }
  }
  write_register(reg, result);
}


int read_register(int r){ //read the content of the register, r is 4-bit binary number
  return reg[r];
}

void write_register(int r, int content){ //write the content of the register, r is the 4-bit binary number
  reg[r] = content;
}

int check_cond(int cond) {  //check the initial condition for every instruction
  int n, z, c, v;
  n = CPSR.CPSR_Bits.N;	//n = build_mask(reg[16], 31, 1);
  z = CPSR.CPSR_Bits.Z;	//z = build_mask(reg[16], 30, 1);
  c = CPSR.CPSR_Bits.C;	//c = build_mask(reg[16], 29, 1);
  v = CPSR.CPSR_Bits.V;	//v = build_mask(reg[16], 28, 1);

  switch(cond) {
  case 0:
    if (z){ return 1;}
    else {return 0;}
  case 1:
    if (!z){return 1;}
    else {return 0;}
  case 10:
    if (n==v){return 1;}
    else {return 0;}
  case 11:
    if (n!=v){return 1;}
    else {return 0;}
  case 12:
    if (!z && (n == v)){return 1;}
    else {return 0; }
  case 13:
    if (z || (n!=v)){return 1;}
    else {return 0;}
  case 14:
    return 1;
  defalt:
    return 0;
  }
  return 0;
 }


void branch(){
    if (check_cond(instr.Branch.Cond)){
      printf("shift:%8x \n", instr.Branch.Offset);
      int offset_shift = instr.Branch.Offset << 2;
      printf("shift:%8x \n", offset_shift);
      if (offset_shift == 0) {
	PC += 4;

      }else {
        if (instr.Branch.Sign) {
            reg[15] = reg[15] - offset_shift;
        } else {
            reg[15] = reg[15] + offset_shift;
        }
      }
    }
}

/*
void branch(instruction instr){
      int temp = instr.Branch.Offset;
        if (instr.Branch.Sign) {
	  temp = pow(2, 24)+1-temp;
	  reg[15] = reg[15] - temp*4;
        } else {
            reg[15] = reg[15] + temp * 4;
        }
}
*/

//build mask from start bpoint 
int build_mask(int instr, int start_point, int length) { //startpoint starts from 0

  instr = instr >> start_point; 
  int mask = 0;
  for(int x = 0; x < length; x++) {
    mask += (int)pow((double)2, (double)x);
  }
  return mask&instr;
}



void single_data_transfer() { //single data transfer instruction

  if(check_cond(instr.SingleDataTransfer.Cond)){
    if(instr.SingleDataTransfer.L){  //load
      if(instr.SingleDataTransfer.P){  //add before
      add_sub_offset(instr);
      single_data_load(instr);
    } else {
	single_data_load(instr);
	add_sub_offset(instr);
      }
    } else { //store
      if(instr.SingleDataTransfer.P){
	add_sub_offset(instr);  //add/sub before
	single_data_store(instr);
      } else {  //add/sub after 
	single_data_store(instr);
	add_sub_offset(instr);
      }
    }
  }
}


void single_data_load(){
  int rn = read_register(instr.SingleDataTransfer.Rn);
    int result = get_4bit_memory(rn);
  write_register(instr.SingleDataTransfer.Rd, result);
  
}

void single_data_store(){
  int rn = read_register(instr.SingleDataTransfer.Rn);
  int result = read_register(instr.SingleDataTransfer.Rd);
  memory[rn] = result;
}


int get_offset(){
  int value = 0;
  if (!instr.SingleDataTransfer.I) {
    value = instr.SingleDataTransfer.Offset;
  }else {
    int value_Rm = read_register(instr.data_pro_reg.Rm);
    if (instr.data_pro_reg.bit4 == 0) {
      value = shift(instr.data_pro_reg.Shift_type, instr.data_pro_reg.Integer, value_Rm);
    }else {
      if (instr.data_pro_reg.Integer % 2 == 0) {
	value = read_register(instr.data_pro_reg.Rm);
      }else {
        int value_Rs = read_register(instr.data_pro_reg.Integer/2);
        value = shift(instr.data_pro_reg.Shift_type, value_Rs, value_Rm);
      }
    }
  }
  return value;
}

void add_sub_offset(){
  int result = read_register(instr.SingleDataTransfer.Rn);
  int offset = get_offset(instr)*4;
  if(instr.SingleDataTransfer.U){
    result += offset;
  } else {
    result -= offset;
  }
  printf("%d", result);
  write_register(instr.SingleDataTransfer.Rn, result);  
}


int get_4bit_memory(int start_point){

  int res = 0;
  for(int i = 0; i < 4; i++) {
    /* if (i >= ) {
      break;
      }*/
    res += memory[start_point + i] * pow(16, 2*i);
  }
  return res;
}  

void report_err_instruction() {

  printf("wrong instruction!");

}













 
