
int main(int argc, char *argv[]);

//check instruction type and its conditions
void check_instr_set();
int check_cond(int cond);
void report_err_instruction();

//data processing and its helper functions 
void data_processing();

int get_operand2();
int shift(int shift_type, int shift_amount, int value);
void set_C(int result);

//multiply instruction
void multiply();

//single_data_transferand and its helper funcitons
void single_data_transfer();

void single_data_load();
void single_data_store();
int get_offset();
void add_sub_offset();

//branch 
void branch();

//registers
int read_register(int r);
void write_register(int r, int content);
void change_1_bit_content(int reg, int position, int content);

//other helper functinons
int get_4bit_memory(int start_point);
int build_mask(int instr, int start_point, int length);



