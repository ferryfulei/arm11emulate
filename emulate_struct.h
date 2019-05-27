
#define u8			unsigned char
#define u16			unsigned short
#define u32			unsigned int

// general structure definition
typedef union
{
	u32 CPSR_32bits;
    struct 
    {
		u32		reserved		: 28;	// Bit[0:27]
		u32 	V				: 1;	// Bit[29] 
		u32 	C				: 1;	// Bit[29] 
		u32		Z				: 1;	// Bit[30]
		u32		N				: 1;	// Bit[31]
    } CPSR_Bits;    
} CPSR_type;


typedef union
{
    u32 instr_32bits;
    struct 
    {
		u32		Byt0		: 8;	// Bit[0:7]
		u32 	        Byt1		: 8;	// Bit[8:15] 
		u32		Byt2		: 8;	// Bit[16:23]
		u32		Byt3		: 8;	// Bit[24:31]
    } instr_Byt;    
    struct 
    {
		u32		reserved2	: 4;	// Bit[0:3]
		u32 	        bit74		: 4;	// Bit[4:7] 
		u32		reserved1	: 13;	// Bit[8:20]
		u32		bit21		: 1;	// Bit[21]
		u32 	        bit22		: 1;	// Bit[22] 
		u32 	        bit23		: 1;	// Bit[23]	
		u32 	        bit2524		: 2;	// Bit[24:25]
		u32 	        bit2726		: 2;	// Bit[26:27]		
		u32 	        Cond		: 4; 	// Bit[28:31]
    } instr_set;
    struct
    {
		u32  	Operand2 	: 12;	// Bit[0:11]
		u32  	Rd 			: 4;	// Bit[12:15]
		u32  	Rn			: 4;	// Bit[16:19]
		u32  	S			: 1;	// Bit[20];
		u32  	OpCode		: 4;	// Bit[21:24]
		u32  	I			: 1;	// Bit[25];
		u32 	bit2726		: 2;	// Bit[26:27]	00
		u32 	Cond		: 4; 	// Bit[28:31]
    } data_pro;

    struct
       {
	        u32     Imm                     : 8;     //Bit[0:7]
	        u32     Rotate                  : 4;     //Bit[8:11]
	        u32  	Rd 			: 4;	// Bit[12:15]
		u32  	Rn			: 4;	// Bit[16:19]
		u32  	S			: 1;	// Bit[20];
		u32  	OpCode		: 4;	// Bit[21:24]
		u32  	I			: 1;	// Bit[25];
		u32 	bit2726		: 2;	// Bit[26:27]	00
		u32 	Cond		: 4; 	// Bit[28:31]
       } data_pro_imm;

    

       struct
	 {

	        u32     Rm                      : 4;        //Bit[0:3]
	        u32     bit4                    : 1;        //Bit[4]
	        u32     Shift_type              : 2;        // Bit[5:6]
       	        u32     Integer                 : 5;         //Bit[7:11]
	        u32  	Rd 			: 4;	// Bit[12:15]
		u32  	Rn			: 4;	// Bit[16:19]
		u32  	S			: 1;	// Bit[20];
		u32  	OpCode		: 4;	// Bit[21:24]
		u32  	I			: 1;	// Bit[25];
		u32 	bit2726		: 2;	// Bit[26:27]	00
		u32 	Cond		: 4; 	// Bit[28:31]

       } data_pro_reg;

  
	struct
	{
		u32		Rm			: 4;	// Bit[3:0]
		u32 	bit74		: 4;	// Bit[7:4] 	1001
		u32  	Rs		 	: 4;	// Bit[11:8]
		u32  	Rn 			: 4;	// Bit[15:12]
		u32  	Rd			: 4;	// Bit[19:16]
		u32  	S			: 1;	// Bit[20];
		u32  	A			: 1;	// Bit[21]
		u32 	bit2522		: 4;	// Bit[25:22] 	0000
		u32 	bit2726		: 2;	// Bit[27:26]	00
		u32 	Cond		: 4; 	// Bit[31:28]
	} Multiply;
	struct
	{
		u32		Offset 		: 12;	// Bit[11:0]
		u32  	Rd 			: 4;	// Bit[15:12]
		u32  	Rn			: 4;	// Bit[19:16]
		u32  	L			: 1;	// Bit[20];
		u32 	bit2221		: 2;	// Bit[22:21] 	00
		u32  	U			: 1;	// Bit[23];
		u32  	P			: 1;	// Bit[24];
		u32  	I			: 1;	// Bit[25];
		u32 	bit2726		: 2;	// Bit[27:26]	01
		u32 	Cond		: 4; 	// Bit[31:28]
	} SingleDataTransfer;
  
  struct
    {
        u32     Offset      : 23;   // Bit[22:0]
        u32        Sign         : 1;    // Bit[23]
        u32     bit2524        : 2;    // Bit[25:24]
        u32     bit2726        : 2;    // Bit[27:26]    01
        u32     Cond        : 4;     // Bit[31:28]
    } Branch;
  
} instruction;

