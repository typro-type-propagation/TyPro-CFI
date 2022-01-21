# 1 "tree.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 341 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "tree.c" 2
# 36 "tree.c"
# 1 "./config.h" 1

# 1 "./auto-host.h" 1
# 3 "./config.h" 2




struct rtx_def;
typedef struct rtx_def *rtx;
struct rtvec_def;
typedef struct rtvec_def *rtvec;
union tree_node;
typedef union tree_node *tree;

# 1 "./ansidecl.h" 1
# 14 "./config.h" 2
# 1 "./biarch64.h" 1
# 15 "./config.h" 2
# 1 "./i386.h" 1
# 53 "./i386.h"
struct processor_costs {
  const int add;
  const int lea;
  const int shift_var;
  const int shift_const;
  const int mult_init;
  const int mult_bit;
  const int divide;
  int movsx;
  int movzx;
  const int large_insn;
  const int move_ratio;

  const int movzbl_load;
  const int int_load[3];


  const int int_store[3];

  const int fp_move;
  const int fp_load[3];

  const int fp_store[3];

  const int mmx_move;
  const int mmx_load[2];

  const int mmx_store[2];

  const int sse_move;
  const int sse_load[3];

  const int sse_store[3];

  const int mmxsse_to_integer;

  const int prefetch_block;
  const int simultaneous_prefetches;

};

extern const struct processor_costs *ix86_cost;



extern int target_flags;
# 211 "./i386.h"
extern const int x86_use_leave, x86_push_memory, x86_zero_extend_with_and;
extern const int x86_use_bit_test, x86_cmove, x86_deep_branch;
extern const int x86_branch_hints, x86_unroll_strlen;
extern const int x86_double_with_add, x86_partial_reg_stall, x86_movx;
extern const int x86_use_loop, x86_use_fiop, x86_use_mov0;
extern const int x86_use_cltd, x86_read_modify_write;
extern const int x86_read_modify, x86_split_long_moves;
extern const int x86_promote_QImode, x86_single_stringop;
extern const int x86_himode_math, x86_qimode_math, x86_promote_qi_regs;
extern const int x86_promote_hi_regs, x86_integer_DFmode_moves;
extern const int x86_add_esp_4, x86_add_esp_8, x86_sub_esp_4, x86_sub_esp_8;
extern const int x86_partial_reg_dependency, x86_memory_mismatch_stall;
extern const int x86_accumulate_outgoing_args, x86_prologue_using_move;
extern const int x86_epilogue_using_move, x86_decompose_lea;
extern const int x86_arch_always_fancy_math_387;
extern int x86_prefetch_sse;
# 395 "./i386.h"
enum processor_type
{
  PROCESSOR_I386,
  PROCESSOR_I486,
  PROCESSOR_PENTIUM,
  PROCESSOR_PENTIUMPRO,
  PROCESSOR_K6,
  PROCESSOR_ATHLON,
  PROCESSOR_PENTIUM4,
  PROCESSOR_max
};
enum fpmath_unit
{
  FPMATH_387 = 1,
  FPMATH_SSE = 2
};

extern enum processor_type ix86_cpu;
extern enum fpmath_unit ix86_fpmath;

extern int ix86_arch;
# 1202 "./i386.h"
enum reg_class
{
  NO_REGS,
  AREG, DREG, CREG, BREG, SIREG, DIREG,
  AD_REGS,
  Q_REGS,
  NON_Q_REGS,
  INDEX_REGS,
  LEGACY_REGS,
  GENERAL_REGS,
  FP_TOP_REG, FP_SECOND_REG,
  FLOAT_REGS,
  SSE_REGS,
  MMX_REGS,
  FP_TOP_SSE_REGS,
  FP_SECOND_SSE_REGS,
  FLOAT_SSE_REGS,
  FLOAT_INT_REGS,
  INT_SSE_REGS,
  FLOAT_INT_SSE_REGS,
  ALL_REGS, LIM_REG_CLASSES
};
# 1667 "./i386.h"
typedef struct ix86_args {
  int words;
  int nregs;
  int regno;
  int sse_words;
  int sse_nregs;
  int sse_regno;
  int maybe_vaarg;
} CUMULATIVE_ARGS;
# 2029 "./i386.h"
enum ix86_builtins
{
  IX86_BUILTIN_ADDPS,
  IX86_BUILTIN_ADDSS,
  IX86_BUILTIN_DIVPS,
  IX86_BUILTIN_DIVSS,
  IX86_BUILTIN_MULPS,
  IX86_BUILTIN_MULSS,
  IX86_BUILTIN_SUBPS,
  IX86_BUILTIN_SUBSS,

  IX86_BUILTIN_CMPEQPS,
  IX86_BUILTIN_CMPLTPS,
  IX86_BUILTIN_CMPLEPS,
  IX86_BUILTIN_CMPGTPS,
  IX86_BUILTIN_CMPGEPS,
  IX86_BUILTIN_CMPNEQPS,
  IX86_BUILTIN_CMPNLTPS,
  IX86_BUILTIN_CMPNLEPS,
  IX86_BUILTIN_CMPNGTPS,
  IX86_BUILTIN_CMPNGEPS,
  IX86_BUILTIN_CMPORDPS,
  IX86_BUILTIN_CMPUNORDPS,
  IX86_BUILTIN_CMPNEPS,
  IX86_BUILTIN_CMPEQSS,
  IX86_BUILTIN_CMPLTSS,
  IX86_BUILTIN_CMPLESS,
  IX86_BUILTIN_CMPGTSS,
  IX86_BUILTIN_CMPGESS,
  IX86_BUILTIN_CMPNEQSS,
  IX86_BUILTIN_CMPNLTSS,
  IX86_BUILTIN_CMPNLESS,
  IX86_BUILTIN_CMPNGTSS,
  IX86_BUILTIN_CMPNGESS,
  IX86_BUILTIN_CMPORDSS,
  IX86_BUILTIN_CMPUNORDSS,
  IX86_BUILTIN_CMPNESS,

  IX86_BUILTIN_COMIEQSS,
  IX86_BUILTIN_COMILTSS,
  IX86_BUILTIN_COMILESS,
  IX86_BUILTIN_COMIGTSS,
  IX86_BUILTIN_COMIGESS,
  IX86_BUILTIN_COMINEQSS,
  IX86_BUILTIN_UCOMIEQSS,
  IX86_BUILTIN_UCOMILTSS,
  IX86_BUILTIN_UCOMILESS,
  IX86_BUILTIN_UCOMIGTSS,
  IX86_BUILTIN_UCOMIGESS,
  IX86_BUILTIN_UCOMINEQSS,

  IX86_BUILTIN_CVTPI2PS,
  IX86_BUILTIN_CVTPS2PI,
  IX86_BUILTIN_CVTSI2SS,
  IX86_BUILTIN_CVTSS2SI,
  IX86_BUILTIN_CVTTPS2PI,
  IX86_BUILTIN_CVTTSS2SI,

  IX86_BUILTIN_MAXPS,
  IX86_BUILTIN_MAXSS,
  IX86_BUILTIN_MINPS,
  IX86_BUILTIN_MINSS,

  IX86_BUILTIN_LOADAPS,
  IX86_BUILTIN_LOADUPS,
  IX86_BUILTIN_STOREAPS,
  IX86_BUILTIN_STOREUPS,
  IX86_BUILTIN_LOADSS,
  IX86_BUILTIN_STORESS,
  IX86_BUILTIN_MOVSS,

  IX86_BUILTIN_MOVHLPS,
  IX86_BUILTIN_MOVLHPS,
  IX86_BUILTIN_LOADHPS,
  IX86_BUILTIN_LOADLPS,
  IX86_BUILTIN_STOREHPS,
  IX86_BUILTIN_STORELPS,

  IX86_BUILTIN_MASKMOVQ,
  IX86_BUILTIN_MOVMSKPS,
  IX86_BUILTIN_PMOVMSKB,

  IX86_BUILTIN_MOVNTPS,
  IX86_BUILTIN_MOVNTQ,

  IX86_BUILTIN_PACKSSWB,
  IX86_BUILTIN_PACKSSDW,
  IX86_BUILTIN_PACKUSWB,

  IX86_BUILTIN_PADDB,
  IX86_BUILTIN_PADDW,
  IX86_BUILTIN_PADDD,
  IX86_BUILTIN_PADDSB,
  IX86_BUILTIN_PADDSW,
  IX86_BUILTIN_PADDUSB,
  IX86_BUILTIN_PADDUSW,
  IX86_BUILTIN_PSUBB,
  IX86_BUILTIN_PSUBW,
  IX86_BUILTIN_PSUBD,
  IX86_BUILTIN_PSUBSB,
  IX86_BUILTIN_PSUBSW,
  IX86_BUILTIN_PSUBUSB,
  IX86_BUILTIN_PSUBUSW,

  IX86_BUILTIN_PAND,
  IX86_BUILTIN_PANDN,
  IX86_BUILTIN_POR,
  IX86_BUILTIN_PXOR,

  IX86_BUILTIN_PAVGB,
  IX86_BUILTIN_PAVGW,

  IX86_BUILTIN_PCMPEQB,
  IX86_BUILTIN_PCMPEQW,
  IX86_BUILTIN_PCMPEQD,
  IX86_BUILTIN_PCMPGTB,
  IX86_BUILTIN_PCMPGTW,
  IX86_BUILTIN_PCMPGTD,

  IX86_BUILTIN_PEXTRW,
  IX86_BUILTIN_PINSRW,

  IX86_BUILTIN_PMADDWD,

  IX86_BUILTIN_PMAXSW,
  IX86_BUILTIN_PMAXUB,
  IX86_BUILTIN_PMINSW,
  IX86_BUILTIN_PMINUB,

  IX86_BUILTIN_PMULHUW,
  IX86_BUILTIN_PMULHW,
  IX86_BUILTIN_PMULLW,

  IX86_BUILTIN_PSADBW,
  IX86_BUILTIN_PSHUFW,

  IX86_BUILTIN_PSLLW,
  IX86_BUILTIN_PSLLD,
  IX86_BUILTIN_PSLLQ,
  IX86_BUILTIN_PSRAW,
  IX86_BUILTIN_PSRAD,
  IX86_BUILTIN_PSRLW,
  IX86_BUILTIN_PSRLD,
  IX86_BUILTIN_PSRLQ,
  IX86_BUILTIN_PSLLWI,
  IX86_BUILTIN_PSLLDI,
  IX86_BUILTIN_PSLLQI,
  IX86_BUILTIN_PSRAWI,
  IX86_BUILTIN_PSRADI,
  IX86_BUILTIN_PSRLWI,
  IX86_BUILTIN_PSRLDI,
  IX86_BUILTIN_PSRLQI,

  IX86_BUILTIN_PUNPCKHBW,
  IX86_BUILTIN_PUNPCKHWD,
  IX86_BUILTIN_PUNPCKHDQ,
  IX86_BUILTIN_PUNPCKLBW,
  IX86_BUILTIN_PUNPCKLWD,
  IX86_BUILTIN_PUNPCKLDQ,

  IX86_BUILTIN_SHUFPS,

  IX86_BUILTIN_RCPPS,
  IX86_BUILTIN_RCPSS,
  IX86_BUILTIN_RSQRTPS,
  IX86_BUILTIN_RSQRTSS,
  IX86_BUILTIN_SQRTPS,
  IX86_BUILTIN_SQRTSS,

  IX86_BUILTIN_UNPCKHPS,
  IX86_BUILTIN_UNPCKLPS,

  IX86_BUILTIN_ANDPS,
  IX86_BUILTIN_ANDNPS,
  IX86_BUILTIN_ORPS,
  IX86_BUILTIN_XORPS,

  IX86_BUILTIN_EMMS,
  IX86_BUILTIN_LDMXCSR,
  IX86_BUILTIN_STMXCSR,
  IX86_BUILTIN_SFENCE,


  IX86_BUILTIN_FEMMS,
  IX86_BUILTIN_PAVGUSB,
  IX86_BUILTIN_PF2ID,
  IX86_BUILTIN_PFACC,
  IX86_BUILTIN_PFADD,
  IX86_BUILTIN_PFCMPEQ,
  IX86_BUILTIN_PFCMPGE,
  IX86_BUILTIN_PFCMPGT,
  IX86_BUILTIN_PFMAX,
  IX86_BUILTIN_PFMIN,
  IX86_BUILTIN_PFMUL,
  IX86_BUILTIN_PFRCP,
  IX86_BUILTIN_PFRCPIT1,
  IX86_BUILTIN_PFRCPIT2,
  IX86_BUILTIN_PFRSQIT1,
  IX86_BUILTIN_PFRSQRT,
  IX86_BUILTIN_PFSUB,
  IX86_BUILTIN_PFSUBR,
  IX86_BUILTIN_PI2FD,
  IX86_BUILTIN_PMULHRW,


  IX86_BUILTIN_PF2IW,
  IX86_BUILTIN_PFNACC,
  IX86_BUILTIN_PFPNACC,
  IX86_BUILTIN_PI2FW,
  IX86_BUILTIN_PSWAPDSI,
  IX86_BUILTIN_PSWAPDSF,

  IX86_BUILTIN_SSE_ZERO,
  IX86_BUILTIN_MMX_ZERO,

  IX86_BUILTIN_MAX
};
# 2818 "./i386.h"
extern int const dbx_register_map[53];
extern int const dbx64_register_map[53];
extern int const svr4_dbx_register_map[53];
# 3092 "./i386.h"
enum cmodel {
  CM_32,
  CM_SMALL,
  CM_KERNEL,
  CM_MEDIUM,
  CM_LARGE,
  CM_SMALL_PIC
};





extern const char *ix86_debug_arg_string, *ix86_debug_addr_string;

enum asm_dialect {
  ASM_ATT,
  ASM_INTEL
};
extern const char *ix86_asm_string;
extern enum asm_dialect ix86_asm_dialect;

extern const char *ix86_cmodel_string;
extern enum cmodel ix86_cmodel;


extern const char *ix86_cpu_string;
extern const char *ix86_arch_string;
extern const char *ix86_fpmath_string;
extern const char *ix86_regparm_string;
extern const char *ix86_align_loops_string;
extern const char *ix86_align_jumps_string;
extern const char *ix86_align_funcs_string;
extern const char *ix86_preferred_stack_boundary_string;
extern const char *ix86_branch_cost_string;
extern int ix86_regparm;
extern int ix86_preferred_stack_boundary;
extern int ix86_branch_cost;
extern enum reg_class const regclass_map[53];
extern rtx ix86_compare_op0;
extern rtx ix86_compare_op1;
# 3149 "./i386.h"
enum fp_cw_mode {FP_CW_STORED, FP_CW_UNINITIALIZED, FP_CW_ANY};
# 16 "./config.h" 2
# 1 "./att.h" 1
# 22 "./att.h"
# 1 "./unix.h" 1
# 23 "./att.h" 2
# 17 "./config.h" 2
# 1 "./dbxelf.h" 1
# 18 "./config.h" 2
# 1 "./elfos.h" 1
# 19 "./config.h" 2
# 1 "./svr4.h" 1
# 20 "./config.h" 2
# 1 "./linux.h" 1
# 21 "./config.h" 2
# 1 "./x86-64.h" 1
# 22 "./config.h" 2
# 1 "./linux64.h" 1
# 23 "./config.h" 2
# 1 "./defaults.h" 1
# 24 "./config.h" 2




# 1 "./insn-constants.h" 1
# 29 "./config.h" 2
# 1 "./insn-flags.h" 1
# 599 "./insn-flags.h"
struct rtx_def;
extern struct rtx_def *gen_cmpdi_ccno_1_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpdi_1_insn_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpqi_ext_3_insn (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpqi_ext_3_insn_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_x86_fnstsw_1 (struct rtx_def *);
extern struct rtx_def *gen_x86_sahf_1 (struct rtx_def *);
extern struct rtx_def *gen_popsi1 (struct rtx_def *);
extern struct rtx_def *gen_movsi_insv_1 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pushdi2_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_popdi1 (struct rtx_def *);
extern struct rtx_def *gen_swapxf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_swaptf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendhisi2_and (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendsidi2_32 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendsidi2_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendhidi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendqidi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendsidi2_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendhidi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendqidi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendhisi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendqihi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendqisi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_truncdfsf2_3 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_truncdfsf2_sse_only (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncdi_nomemory (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncdi_memory (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncsfdi_sse (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncdfdi_sse (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncsi_nomemory (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncsi_memory (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncsfsi_sse (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncdfsi_sse (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_trunchi_nomemory (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_trunchi_memory (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_x86_fnstcw_1 (struct rtx_def *);
extern struct rtx_def *gen_x86_fldcw_1 (struct rtx_def *);
extern struct rtx_def *gen_floathisf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floathidf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floathixf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floathitf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatsixf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatsitf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatdixf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatditf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addqi3_cc (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addsi_1_zext (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addqi_ext_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subdi3_carry_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subsi3_carry (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subsi3_carry_zext (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_udivqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divmodhi4 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_udivmoddi4 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_udivmodsi4 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_testsi_1 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_andqi_ext_0 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negsf2_memory (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negsf2_ifs (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negdf2_memory (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negdf2_ifs (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_abssf2_memory (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_abssf2_ifs (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_absdf2_memory (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_absdf2_ifs (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashldi3_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_x86_shld_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrdi3_63_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrdi3_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_x86_shrd_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrsi3_31 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_lshrdi3_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_setcc_2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_jump (struct rtx_def *);
extern struct rtx_def *gen_doloop_end_internal (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_blockage (void);
extern struct rtx_def *gen_return_internal (void);
extern struct rtx_def *gen_return_pop_internal (struct rtx_def *);
extern struct rtx_def *gen_return_indirect_internal (struct rtx_def *);
extern struct rtx_def *gen_nop (void);
extern struct rtx_def *gen_prologue_set_got (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_prologue_get_pc (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_eh_return_si (struct rtx_def *);
extern struct rtx_def *gen_eh_return_di (struct rtx_def *);
extern struct rtx_def *gen_leave (void);
extern struct rtx_def *gen_leave_rex64 (void);
extern struct rtx_def *gen_ffssi_1 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtsf2_1 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtsf2_1_sse_only (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtsf2_i387 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtdf2_1 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtdf2_1_sse_only (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtdf2_i387 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtxf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrttf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sindf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sinsf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sinxf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sintf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cosdf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cossf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cosxf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_costf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cld (void);
extern struct rtx_def *gen_strmovdi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovsi_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovsi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovhi_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovhi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovqi_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovqi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_movdi_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_movsi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_movsi_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_movqi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_movqi_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetdi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetsi_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetsi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsethi_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsethi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetqi_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetqi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_stosdi_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_stossi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_stossi_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_stosqi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rep_stosqi_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpstrqi_nz_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpstrqi_nz_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpstrqi_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpstrqi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strlenqi_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strlenqi_rex_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_x86_movdicc_0_m1_rex64 (struct rtx_def *);
extern struct rtx_def *gen_x86_movsicc_0_m1 (struct rtx_def *);
extern struct rtx_def *gen_pro_epilogue_adjust_stack_rex64 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movsfcc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movsfcc_eq (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movdfcc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movdfcc_eq (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_allocate_stack_worker_1 (struct rtx_def *);
extern struct rtx_def *gen_allocate_stack_worker_rex64 (struct rtx_def *);
extern struct rtx_def *gen_trap (void);
extern struct rtx_def *gen_movv4sf_internal (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv4si_internal (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv8qi_internal (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv4hi_internal (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv2si_internal (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv2sf_internal (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movti_internal (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movaps (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movups (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movmskps (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_pmovmskb (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_maskmovq (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_maskmovq_rex (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movntv4sf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movntdi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movhlps (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movlhps (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movhps (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movlps (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_loadss (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_movss (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_storess (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_shufps (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmaddv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmsubv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmmulv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmdivv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rcpv4sf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmrcpv4sf2 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rsqrtv4sf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmrsqrtv4sf2 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtv4sf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmsqrtv4sf2 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_andti3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_nandti3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_iorti3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_xorti3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_clrv4sf (struct rtx_def *);
extern struct rtx_def *gen_maskcmpv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_maskncmpv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmmaskcmpv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmmaskncmpv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_comi (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_ucomi (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_unpckhps (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sse_unpcklps (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_smaxv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmsmaxv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sminv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_vmsminv4sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cvtpi2ps (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cvtps2pi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cvttps2pi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cvtsi2ss (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cvtss2si (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cvttss2si (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addv2si3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ssaddv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ssaddv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_usaddv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_usaddv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subv2si3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sssubv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sssubv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ussubv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ussubv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_smulv4hi3_highpart (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_umulv4hi3_highpart (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_pmaddwd (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_iordi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_xordi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_clrdi (struct rtx_def *);
extern struct rtx_def *gen_mmx_anddi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_nanddi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_uavgv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_uavgv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_psadbw (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_pinsrw (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_pextrw (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_pshufw (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_eqv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_eqv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_eqv2si3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_gtv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_gtv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_gtv2si3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_umaxv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_smaxv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_uminv8qi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sminv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrv2si3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_lshrv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_lshrv2si3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_lshrdi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashlv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashlv2si3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_ashldi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_packsswb (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_packssdw (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_packuswb (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_punpckhbw (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_punpckhwd (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_punpckhdq (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_punpcklbw (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_punpcklwd (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mmx_punpckldq (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_emms (void);
extern struct rtx_def *gen_ldmxcsr (struct rtx_def *);
extern struct rtx_def *gen_stmxcsr (struct rtx_def *);
extern struct rtx_def *gen_addv2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subv2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subrv2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_gtv2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_gev2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_eqv2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfmaxv2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfminv2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulv2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_femms (void);
extern struct rtx_def *gen_pf2id (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pf2iw (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfacc (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfnacc (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfpnacc (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pi2fw (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatv2si2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pavgusb (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfrcpv2sf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfrcpit1v2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfrcpit2v2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfrsqrtv2sf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pfrsqit1v2sf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pmulhrwv4hi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pswapdv2si2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pswapdv2sf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpdi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpsi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmphi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpqi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpdi_1_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpsi_1 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpqi_ext_3 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpxf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmptf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpdf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpsf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movsi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movhi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movstricthi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movqi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_reload_outqi (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movstrictqi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movdi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movsf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movdf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movxf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movtf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendhisi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendqihi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendqisi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_zero_extendsidi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendsidi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendsfdf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendsfxf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extendsftf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extenddfxf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extenddftf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_truncdfsf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_truncxfsf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_trunctfsf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_truncxfdf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_trunctfdf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncxfdi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_trunctfdi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncdfdi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncsfdi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncxfsi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_trunctfsi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncdfsi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncsfsi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncxfhi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_trunctfhi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncdfhi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_fix_truncsfhi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatsisf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatdisf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatsidf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_floatdidf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_adddi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addxf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addtf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_adddf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_addsf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subdi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subxf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subtf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subdf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_subsf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_muldi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_umulqihi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulqihi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_umulditi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_umulsidi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulditi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulsidi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_umuldi3_highpart (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_umulsi3_highpart (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_smuldi3_highpart (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_smulsi3_highpart (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulxf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_multf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_muldf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mulsf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divxf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divtf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divdf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divsf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divmoddi4 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_divmodsi4 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_udivmodhi4 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_testsi_ccno_1 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_testqi_ccz_1 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_testqi_ext_ccno_0 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_anddi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_andsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_andhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_andqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_iordi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_iorsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_iorhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_iorqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_xordi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_xorsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_xorhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_xorqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_xorqi_cc_ext_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negdi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negsi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_neghi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negqi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negsf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negdf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negxf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_negtf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_abssf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_absdf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_absxf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_abstf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_one_cmpldi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_one_cmplsi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_one_cmplhi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_one_cmplqi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashldi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_x86_shift_adj_1 (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_x86_shift_adj_2 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashlsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashlhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashlqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrdi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_x86_shift_adj_3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ashrqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_lshrdi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_lshrsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_lshrhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_lshrqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rotldi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rotlsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rotlhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rotlqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rotrdi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rotrsi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rotrhi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_rotrqi3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extv (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_extzv (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_insv (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_seq (struct rtx_def *);
extern struct rtx_def *gen_sne (struct rtx_def *);
extern struct rtx_def *gen_sgt (struct rtx_def *);
extern struct rtx_def *gen_sgtu (struct rtx_def *);
extern struct rtx_def *gen_slt (struct rtx_def *);
extern struct rtx_def *gen_sltu (struct rtx_def *);
extern struct rtx_def *gen_sge (struct rtx_def *);
extern struct rtx_def *gen_sgeu (struct rtx_def *);
extern struct rtx_def *gen_sle (struct rtx_def *);
extern struct rtx_def *gen_sleu (struct rtx_def *);
extern struct rtx_def *gen_sunordered (struct rtx_def *);
extern struct rtx_def *gen_sordered (struct rtx_def *);
extern struct rtx_def *gen_suneq (struct rtx_def *);
extern struct rtx_def *gen_sunge (struct rtx_def *);
extern struct rtx_def *gen_sungt (struct rtx_def *);
extern struct rtx_def *gen_sunle (struct rtx_def *);
extern struct rtx_def *gen_sunlt (struct rtx_def *);
extern struct rtx_def *gen_sltgt (struct rtx_def *);
extern struct rtx_def *gen_beq (struct rtx_def *);
extern struct rtx_def *gen_bne (struct rtx_def *);
extern struct rtx_def *gen_bgt (struct rtx_def *);
extern struct rtx_def *gen_bgtu (struct rtx_def *);
extern struct rtx_def *gen_blt (struct rtx_def *);
extern struct rtx_def *gen_bltu (struct rtx_def *);
extern struct rtx_def *gen_bge (struct rtx_def *);
extern struct rtx_def *gen_bgeu (struct rtx_def *);
extern struct rtx_def *gen_ble (struct rtx_def *);
extern struct rtx_def *gen_bleu (struct rtx_def *);
extern struct rtx_def *gen_bunordered (struct rtx_def *);
extern struct rtx_def *gen_bordered (struct rtx_def *);
extern struct rtx_def *gen_buneq (struct rtx_def *);
extern struct rtx_def *gen_bunge (struct rtx_def *);
extern struct rtx_def *gen_bungt (struct rtx_def *);
extern struct rtx_def *gen_bunle (struct rtx_def *);
extern struct rtx_def *gen_bunlt (struct rtx_def *);
extern struct rtx_def *gen_bltgt (struct rtx_def *);
extern struct rtx_def *gen_indirect_jump (struct rtx_def *);
extern struct rtx_def *gen_tablejump (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_doloop_end (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);

extern struct rtx_def *gen_call_pop (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);

extern struct rtx_def *gen_call (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_call_exp (struct rtx_def *, struct rtx_def *);

extern struct rtx_def *gen_call_value_pop (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);

extern struct rtx_def *gen_call_value (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_call_value_exp (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_untyped_call (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_return (void);
extern struct rtx_def *gen_prologue (void);
extern struct rtx_def *gen_epilogue (void);
extern struct rtx_def *gen_sibcall_epilogue (void);
extern struct rtx_def *gen_eh_return (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_ffssi2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtsf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sqrtdf2 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movstrsi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movstrdi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovdi_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovsi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovsi_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovhi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovhi_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovqi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strmovqi_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_clrstrsi (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_clrstrdi (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetdi_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetsi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetsi_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsethi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsethi_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetqi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strsetqi_rex64 (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpstrsi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_cmpintqi (struct rtx_def *);
extern struct rtx_def *gen_strlensi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_strlendi (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movdicc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movsicc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movhicc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movsfcc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movdfcc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movxfcc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movtfcc (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_minsf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_mindf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_maxsf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_maxdf3 (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_pro_epilogue_adjust_stack (struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_allocate_stack_worker (struct rtx_def *);
extern struct rtx_def *gen_allocate_stack (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_builtin_setjmp_receiver (struct rtx_def *);
extern struct rtx_def *gen_conditional_trap (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movti (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv4sf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv4si (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv2si (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv4hi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv8qi (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_movv2sf (struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_sfence (void);
extern struct rtx_def *gen_sse_prologue_save (struct rtx_def *, struct rtx_def *, struct rtx_def *, struct rtx_def *);
extern struct rtx_def *gen_prefetch (struct rtx_def *, struct rtx_def *, struct rtx_def *);
# 30 "./config.h" 2
# 37 "tree.c" 2
# 1 "./system.h" 1
# 33 "./system.h"
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stdarg.h" 1 3
# 14 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stdarg.h" 3
typedef __builtin_va_list va_list;
# 32 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stdarg.h" 3
typedef __builtin_va_list __gnuc_va_list;
# 34 "./system.h" 2
# 47 "./system.h"
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3
# 35 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 3
typedef long int ptrdiff_t;
# 46 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 3
typedef long unsigned int size_t;
# 74 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 3
typedef int wchar_t;
# 102 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 3
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/__stddef_max_align_t.h" 1 3
# 19 "/usr/lib/llvm-10/lib/clang/10.0.0/include/__stddef_max_align_t.h" 3
typedef struct {
  long long __clang_max_align_nonce1
      __attribute__((__aligned__(__alignof__(long long))));
  long double __clang_max_align_nonce2
      __attribute__((__aligned__(__alignof__(long double))));
} max_align_t;
# 103 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 2 3
# 48 "./system.h" 2


# 1 "/usr/include/stdio.h" 1 3 4
# 27 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 33 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 439 "/usr/include/features.h" 3 4
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 440 "/usr/include/features.h" 2 3 4
# 461 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 1 3 4
# 452 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 453 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/long-double.h" 1 3 4
# 454 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 462 "/usr/include/features.h" 2 3 4
# 485 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4
# 486 "/usr/include/features.h" 2 3 4
# 34 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 2 3 4
# 28 "/usr/include/stdio.h" 2 3 4





# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 34 "/usr/include/stdio.h" 2 3 4




# 1 "/usr/include/x86_64-linux-gnu/bits/types.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/timesize.h" 1 3 4
# 29 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;






typedef __int8_t __int_least8_t;
typedef __uint8_t __uint_least8_t;
typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;
typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;
typedef __int64_t __int_least64_t;
typedef __uint64_t __uint_least64_t;



typedef long int __quad_t;
typedef unsigned long int __u_quad_t;







typedef long int __intmax_t;
typedef unsigned long int __uintmax_t;
# 141 "/usr/include/x86_64-linux-gnu/bits/types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/typesizes.h" 1 3 4
# 142 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/time64.h" 1 3 4
# 143 "/usr/include/x86_64-linux-gnu/bits/types.h" 2 3 4


typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;

typedef int __daddr_t;
typedef int __key_t;


typedef int __clockid_t;


typedef void * __timer_t;


typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;


typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;


typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;


typedef long int __fsword_t;

typedef long int __ssize_t;


typedef long int __syscall_slong_t;

typedef unsigned long int __syscall_ulong_t;



typedef __off64_t __loff_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;




typedef int __sig_atomic_t;
# 39 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/__fpos_t.h" 1 3 4




# 1 "/usr/include/x86_64-linux-gnu/bits/types/__mbstate_t.h" 1 3 4
# 13 "/usr/include/x86_64-linux-gnu/bits/types/__mbstate_t.h" 3 4
typedef struct
{
  int __count;
  union
  {
    unsigned int __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
# 6 "/usr/include/x86_64-linux-gnu/bits/types/__fpos_t.h" 2 3 4




typedef struct _G_fpos_t
{
  __off_t __pos;
  __mbstate_t __state;
} __fpos_t;
# 40 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/__fpos64_t.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/bits/types/__fpos64_t.h" 3 4
typedef struct _G_fpos64_t
{
  __off64_t __pos;
  __mbstate_t __state;
} __fpos64_t;
# 41 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/__FILE.h" 1 3 4



struct _IO_FILE;
typedef struct _IO_FILE __FILE;
# 42 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/FILE.h" 1 3 4



struct _IO_FILE;


typedef struct _IO_FILE FILE;
# 43 "/usr/include/stdio.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h" 1 3 4
# 35 "/usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h" 3 4
struct _IO_FILE;
struct _IO_marker;
struct _IO_codecvt;
struct _IO_wide_data;




typedef void _IO_lock_t;





struct _IO_FILE
{
  int _flags;


  char *_IO_read_ptr;
  char *_IO_read_end;
  char *_IO_read_base;
  char *_IO_write_base;
  char *_IO_write_ptr;
  char *_IO_write_end;
  char *_IO_buf_base;
  char *_IO_buf_end;


  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
  int _flags2;
  __off_t _old_offset;


  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];

  _IO_lock_t *_lock;







  __off64_t _offset;

  struct _IO_codecvt *_codecvt;
  struct _IO_wide_data *_wide_data;
  struct _IO_FILE *_freeres_list;
  void *_freeres_buf;
  size_t __pad5;
  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];
};
# 44 "/usr/include/stdio.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/types/cookie_io_functions_t.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/types/cookie_io_functions_t.h" 3 4
typedef __ssize_t cookie_read_function_t (void *__cookie, char *__buf,
                                          size_t __nbytes);







typedef __ssize_t cookie_write_function_t (void *__cookie, const char *__buf,
                                           size_t __nbytes);







typedef int cookie_seek_function_t (void *__cookie, __off64_t *__pos, int __w);


typedef int cookie_close_function_t (void *__cookie);






typedef struct _IO_cookie_io_functions_t
{
  cookie_read_function_t *read;
  cookie_write_function_t *write;
  cookie_seek_function_t *seek;
  cookie_close_function_t *close;
} cookie_io_functions_t;
# 47 "/usr/include/stdio.h" 2 3 4





typedef __gnuc_va_list va_list;
# 63 "/usr/include/stdio.h" 3 4
typedef __off_t off_t;






typedef __off64_t off64_t;






typedef __ssize_t ssize_t;






typedef __fpos_t fpos_t;




typedef __fpos64_t fpos64_t;
# 133 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdio_lim.h" 1 3 4
# 134 "/usr/include/stdio.h" 2 3 4



extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;






extern int remove (const char *__filename) __attribute__ ((__nothrow__ ));

extern int rename (const char *__old, const char *__new) __attribute__ ((__nothrow__ ));



extern int renameat (int __oldfd, const char *__old, int __newfd,
       const char *__new) __attribute__ ((__nothrow__ ));
# 164 "/usr/include/stdio.h" 3 4
extern int renameat2 (int __oldfd, const char *__old, int __newfd,
        const char *__new, unsigned int __flags) __attribute__ ((__nothrow__ ));







extern FILE *tmpfile (void) ;
# 183 "/usr/include/stdio.h" 3 4
extern FILE *tmpfile64 (void) ;



extern char *tmpnam (char *__s) __attribute__ ((__nothrow__ )) ;




extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__ )) ;
# 204 "/usr/include/stdio.h" 3 4
extern char *tempnam (const char *__dir, const char *__pfx)
     __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__)) ;







extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);
# 227 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 237 "/usr/include/stdio.h" 3 4
extern int fcloseall (void);
# 246 "/usr/include/stdio.h" 3 4
extern FILE *fopen (const char *__restrict __filename,
      const char *__restrict __modes) ;




extern FILE *freopen (const char *__restrict __filename,
        const char *__restrict __modes,
        FILE *__restrict __stream) ;
# 270 "/usr/include/stdio.h" 3 4
extern FILE *fopen64 (const char *__restrict __filename,
        const char *__restrict __modes) ;
extern FILE *freopen64 (const char *__restrict __filename,
   const char *__restrict __modes,
   FILE *__restrict __stream) ;




extern FILE *fdopen (int __fd, const char *__modes) __attribute__ ((__nothrow__ )) ;





extern FILE *fopencookie (void *__restrict __magic_cookie,
     const char *__restrict __modes,
     cookie_io_functions_t __io_funcs) __attribute__ ((__nothrow__ )) ;




extern FILE *fmemopen (void *__s, size_t __len, const char *__modes)
  __attribute__ ((__nothrow__ )) ;




extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__ )) ;





extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__ ));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__ ));




extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__ ));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__ ));







extern int fprintf (FILE *__restrict __stream,
      const char *__restrict __format, ...);




extern int printf (const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));



extern int snprintf (char *__restrict __s, size_t __maxlen,
       const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));





extern int vasprintf (char **__restrict __ptr, const char *__restrict __f,
        __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 0))) ;
extern int __asprintf (char **__restrict __ptr,
         const char *__restrict __fmt, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3))) ;
extern int asprintf (char **__restrict __ptr,
       const char *__restrict __fmt, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3))) ;




extern int vdprintf (int __fd, const char *__restrict __fmt,
       __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, const char *__restrict __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));







extern int fscanf (FILE *__restrict __stream,
     const char *__restrict __format, ...) ;




extern int scanf (const char *__restrict __format, ...) ;

extern int sscanf (const char *__restrict __s,
     const char *__restrict __format, ...) __attribute__ ((__nothrow__ ));






extern int fscanf (FILE *__restrict __stream, const char *__restrict __format, ...) __asm__ ("" "__isoc99_fscanf") ;


extern int scanf (const char *__restrict __format, ...) __asm__ ("" "__isoc99_scanf") ;

extern int sscanf (const char *__restrict __s, const char *__restrict __format, ...) __asm__ ("" "__isoc99_sscanf") __attribute__ ((__nothrow__ ));
# 432 "/usr/include/stdio.h" 3 4
extern int vfscanf (FILE *__restrict __s, const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;





extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;


extern int vsscanf (const char *__restrict __s,
      const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__ )) __attribute__ ((__format__ (__scanf__, 2, 0)));




extern int vfscanf (FILE *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vfscanf")



     __attribute__ ((__format__ (__scanf__, 2, 0))) ;
extern int vscanf (const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vscanf")

     __attribute__ ((__format__ (__scanf__, 1, 0))) ;
extern int vsscanf (const char *__restrict __s, const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vsscanf") __attribute__ ((__nothrow__ ))



     __attribute__ ((__format__ (__scanf__, 2, 0)));
# 485 "/usr/include/stdio.h" 3 4
extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);






extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 510 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);
# 521 "/usr/include/stdio.h" 3 4
extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);
# 537 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);







extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
          ;
# 587 "/usr/include/stdio.h" 3 4
extern char *fgets_unlocked (char *__restrict __s, int __n,
        FILE *__restrict __stream) ;
# 603 "/usr/include/stdio.h" 3 4
extern __ssize_t __getdelim (char **__restrict __lineptr,
                             size_t *__restrict __n, int __delimiter,
                             FILE *__restrict __stream) ;
extern __ssize_t getdelim (char **__restrict __lineptr,
                           size_t *__restrict __n, int __delimiter,
                           FILE *__restrict __stream) ;







extern __ssize_t getline (char **__restrict __lineptr,
                          size_t *__restrict __n,
                          FILE *__restrict __stream) ;







extern int fputs (const char *__restrict __s, FILE *__restrict __stream);





extern int puts (const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s);
# 662 "/usr/include/stdio.h" 3 4
extern int fputs_unlocked (const char *__restrict __s,
      FILE *__restrict __stream);
# 673 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream);







extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);
# 707 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
# 731 "/usr/include/stdio.h" 3 4
extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, const fpos_t *__pos);
# 750 "/usr/include/stdio.h" 3 4
extern int fseeko64 (FILE *__stream, __off64_t __off, int __whence);
extern __off64_t ftello64 (FILE *__stream) ;
extern int fgetpos64 (FILE *__restrict __stream, fpos64_t *__restrict __pos);
extern int fsetpos64 (FILE *__stream, const fpos64_t *__pos);



extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__ ));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__ )) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__ )) ;



extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__ ));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__ )) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__ )) ;







extern void perror (const char *__s);






# 1 "/usr/include/x86_64-linux-gnu/bits/sys_errlist.h" 1 3 4
# 26 "/usr/include/x86_64-linux-gnu/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern const char *const sys_errlist[];


extern int _sys_nerr;
extern const char *const _sys_errlist[];
# 782 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__ )) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__ )) ;
# 800 "/usr/include/stdio.h" 3 4
extern FILE *popen (const char *__command, const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__ ));





extern char *cuserid (char *__s);




struct obstack;


extern int obstack_printf (struct obstack *__restrict __obstack,
      const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 3)));
extern int obstack_vprintf (struct obstack *__restrict __obstack,
       const char *__restrict __format,
       __gnuc_va_list __args)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 2, 0)));







extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__ ));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__ )) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__ ));
# 858 "/usr/include/stdio.h" 3 4
extern int __uflow (FILE *);
extern int __overflow (FILE *, int);
# 51 "./system.h" 2
# 110 "./system.h"
# 1 "./safe-ctype.h" 1
# 44 "./safe-ctype.h"
enum {

  _sch_isblank = 0x0001,
  _sch_iscntrl = 0x0002,
  _sch_isdigit = 0x0004,
  _sch_islower = 0x0008,
  _sch_isprint = 0x0010,
  _sch_ispunct = 0x0020,
  _sch_isspace = 0x0040,
  _sch_isupper = 0x0080,
  _sch_isxdigit = 0x0100,


  _sch_isidst = 0x0200,
  _sch_isvsp = 0x0400,
  _sch_isnvsp = 0x0800,


  _sch_isalpha = _sch_isupper|_sch_islower,
  _sch_isalnum = _sch_isalpha|_sch_isdigit,
  _sch_isidnum = _sch_isidst|_sch_isdigit,
  _sch_isgraph = _sch_isalnum|_sch_ispunct,
  _sch_iscppsp = _sch_isvsp|_sch_isnvsp,
  _sch_isbasic = _sch_isprint|_sch_iscppsp

};


extern const unsigned short _sch_istable[256];
# 97 "./safe-ctype.h"
extern const unsigned char _sch_toupper[256];
extern const unsigned char _sch_tolower[256];
# 111 "./system.h" 2

# 1 "/usr/include/x86_64-linux-gnu/sys/types.h" 1 3 4
# 33 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;


typedef __loff_t loff_t;




typedef __ino_t ino_t;






typedef __ino64_t ino64_t;




typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;
# 97 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __pid_t pid_t;





typedef __id_t id_t;
# 114 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;





# 1 "/usr/include/x86_64-linux-gnu/bits/types/clock_t.h" 1 3 4






typedef __clock_t clock_t;
# 127 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/types/clockid_t.h" 1 3 4






typedef __clockid_t clockid_t;
# 129 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/time_t.h" 1 3 4






typedef __time_t time_t;
# 130 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/timer_t.h" 1 3 4






typedef __timer_t timer_t;
# 131 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef __useconds_t useconds_t;



typedef __suseconds_t suseconds_t;






# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 145 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;





# 1 "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h" 3 4
typedef __int8_t int8_t;
typedef __int16_t int16_t;
typedef __int32_t int32_t;
typedef __int64_t int64_t;
# 156 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4


typedef __uint8_t u_int8_t;
typedef __uint16_t u_int16_t;
typedef __uint32_t u_int32_t;
typedef __uint64_t u_int64_t;


typedef int register_t __attribute__ ((__mode__ (__word__)));
# 176 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 24 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/endian.h" 1 3 4
# 35 "/usr/include/x86_64-linux-gnu/bits/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/endianness.h" 1 3 4
# 36 "/usr/include/x86_64-linux-gnu/bits/endian.h" 2 3 4
# 25 "/usr/include/endian.h" 2 3 4
# 35 "/usr/include/endian.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 1 3 4
# 33 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
static __inline __uint16_t
__bswap_16 (__uint16_t __bsx)
{



  return ((__uint16_t) ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8)));

}






static __inline __uint32_t
__bswap_32 (__uint32_t __bsx)
{



  return ((((__bsx) & 0xff000000u) >> 24) | (((__bsx) & 0x00ff0000u) >> 8) | (((__bsx) & 0x0000ff00u) << 8) | (((__bsx) & 0x000000ffu) << 24));

}
# 69 "/usr/include/x86_64-linux-gnu/bits/byteswap.h" 3 4
__extension__ static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{



  return ((((__bsx) & 0xff00000000000000ull) >> 56) | (((__bsx) & 0x00ff000000000000ull) >> 40) | (((__bsx) & 0x0000ff0000000000ull) >> 24) | (((__bsx) & 0x000000ff00000000ull) >> 8) | (((__bsx) & 0x00000000ff000000ull) << 8) | (((__bsx) & 0x0000000000ff0000ull) << 24) | (((__bsx) & 0x000000000000ff00ull) << 40) | (((__bsx) & 0x00000000000000ffull) << 56));

}
# 36 "/usr/include/endian.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/uintn-identity.h" 1 3 4
# 32 "/usr/include/x86_64-linux-gnu/bits/uintn-identity.h" 3 4
static __inline __uint16_t
__uint16_identity (__uint16_t __x)
{
  return __x;
}

static __inline __uint32_t
__uint32_identity (__uint32_t __x)
{
  return __x;
}

static __inline __uint64_t
__uint64_identity (__uint64_t __x)
{
  return __x;
}
# 37 "/usr/include/endian.h" 2 3 4
# 177 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/sys/select.h" 1 3 4
# 30 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/select.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/select.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/select.h" 2 3 4
# 31 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/types/sigset_t.h" 1 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/types/__sigset_t.h" 1 3 4




typedef struct
{
  unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
} __sigset_t;
# 5 "/usr/include/x86_64-linux-gnu/bits/types/sigset_t.h" 2 3 4


typedef __sigset_t sigset_t;
# 34 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_timeval.h" 1 3 4







struct timeval
{
  __time_t tv_sec;
  __suseconds_t tv_usec;
};
# 38 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4

# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h" 3 4
struct timespec
{
  __time_t tv_sec;



  __syscall_slong_t tv_nsec;
# 26 "/usr/include/x86_64-linux-gnu/bits/types/struct_timespec.h" 3 4
};
# 40 "/usr/include/x86_64-linux-gnu/sys/select.h" 2 3 4
# 49 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
typedef long int __fd_mask;
# 59 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
typedef struct
  {



    __fd_mask fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];





  } fd_set;






typedef __fd_mask fd_mask;
# 101 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 113 "/usr/include/x86_64-linux-gnu/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);
# 180 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4





typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 219 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
typedef __blkcnt64_t blkcnt64_t;
typedef __fsblkcnt64_t fsblkcnt64_t;
typedef __fsfilcnt64_t fsfilcnt64_t;






# 1 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 1 3 4
# 44 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 1 3 4
# 21 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes-arch.h" 2 3 4
# 45 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 2 3 4




typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;

typedef struct __pthread_internal_slist
{
  struct __pthread_internal_slist *__next;
} __pthread_slist_t;
# 74 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/struct_mutex.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/struct_mutex.h" 3 4
struct __pthread_mutex_s
{
  int __lock;
  unsigned int __count;
  int __owner;

  unsigned int __nusers;



  int __kind;

  short __spins;
  short __elision;
  __pthread_list_t __list;
# 53 "/usr/include/x86_64-linux-gnu/bits/struct_mutex.h" 3 4
};
# 75 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 2 3 4
# 87 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/struct_rwlock.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/struct_rwlock.h" 3 4
struct __pthread_rwlock_arch_t
{
  unsigned int __readers;
  unsigned int __writers;
  unsigned int __wrphase_futex;
  unsigned int __writers_futex;
  unsigned int __pad3;
  unsigned int __pad4;

  int __cur_writer;
  int __shared;
  signed char __rwelision;




  unsigned char __pad1[7];


  unsigned long int __pad2;


  unsigned int __flags;
# 55 "/usr/include/x86_64-linux-gnu/bits/struct_rwlock.h" 3 4
};
# 88 "/usr/include/x86_64-linux-gnu/bits/thread-shared-types.h" 2 3 4




struct __pthread_cond_s
{
  __extension__ union
  {
    __extension__ unsigned long long int __wseq;
    struct
    {
      unsigned int __low;
      unsigned int __high;
    } __wseq32;
  };
  __extension__ union
  {
    __extension__ unsigned long long int __g1_start;
    struct
    {
      unsigned int __low;
      unsigned int __high;
    } __g1_start32;
  };
  unsigned int __g_refs[2] ;
  unsigned int __g_size[2];
  unsigned int __g1_orig_size;
  unsigned int __wrefs;
  unsigned int __g_signals[2];
};
# 24 "/usr/include/x86_64-linux-gnu/bits/pthreadtypes.h" 2 3 4



typedef unsigned long int pthread_t;




typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;




typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;


union pthread_attr_t
{
  char __size[56];
  long int __align;
};

typedef union pthread_attr_t pthread_attr_t;




typedef union
{
  struct __pthread_mutex_s __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;


typedef union
{
  struct __pthread_cond_s __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;





typedef union
{
  struct __pthread_rwlock_arch_t __data;
  char __size[56];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 228 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4
# 113 "./system.h" 2

# 1 "/usr/include/errno.h" 1 3 4
# 28 "/usr/include/errno.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/errno.h" 1 3 4
# 26 "/usr/include/x86_64-linux-gnu/bits/errno.h" 3 4
# 1 "/usr/include/linux/errno.h" 1 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/errno.h" 1 3 4
# 1 "/usr/include/asm-generic/errno.h" 1 3 4




# 1 "/usr/include/asm-generic/errno-base.h" 1 3 4
# 6 "/usr/include/asm-generic/errno.h" 2 3 4
# 2 "/usr/include/x86_64-linux-gnu/asm/errno.h" 2 3 4
# 2 "/usr/include/linux/errno.h" 2 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/errno.h" 2 3 4
# 29 "/usr/include/errno.h" 2 3 4








extern int *__errno_location (void) __attribute__ ((__nothrow__ )) __attribute__ ((__const__));







extern char *program_invocation_name;
extern char *program_invocation_short_name;


# 1 "/usr/include/x86_64-linux-gnu/bits/types/error_t.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/types/error_t.h" 3 4
typedef int error_t;
# 49 "/usr/include/errno.h" 2 3 4
# 115 "./system.h" 2
# 125 "./system.h"
# 1 "/usr/include/string.h" 1 3 4
# 26 "/usr/include/string.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 27 "/usr/include/string.h" 2 3 4






# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 34 "/usr/include/string.h" 2 3 4
# 43 "/usr/include/string.h" 3 4
extern void *memcpy (void *__restrict __dest, const void *__restrict __src,
       size_t __n) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, const void *__src, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));





extern void *memccpy (void *__restrict __dest, const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));




extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));


extern int memcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 91 "/usr/include/string.h" 3 4
extern void *memchr (const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 104 "/usr/include/string.h" 3 4
extern void *rawmemchr (const void *__s, int __c)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 115 "/usr/include/string.h" 3 4
extern void *memrchr (const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (const char *__s1, const char *__s2, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));




# 1 "/usr/include/x86_64-linux-gnu/bits/types/locale_t.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/types/locale_t.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/__locale_t.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/types/__locale_t.h" 3 4
struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
};

typedef struct __locale_struct *__locale_t;
# 23 "/usr/include/x86_64-linux-gnu/bits/types/locale_t.h" 2 3 4

typedef __locale_t locale_t;
# 154 "/usr/include/string.h" 2 3 4


extern int strcoll_l (const char *__s1, const char *__s2, locale_t __l)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));


extern size_t strxfrm_l (char *__dest, const char *__src, size_t __n,
    locale_t __l) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 4)));





extern char *strdup (const char *__s)
     __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (const char *__string, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 226 "/usr/include/string.h" 3 4
extern char *strchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 253 "/usr/include/string.h" 3 4
extern char *strrchr (const char *__s, int __c)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 266 "/usr/include/string.h" 3 4
extern char *strchrnul (const char *__s, int __c)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern size_t strcspn (const char *__s, const char *__reject)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 303 "/usr/include/string.h" 3 4
extern char *strpbrk (const char *__s, const char *__accept)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 330 "/usr/include/string.h" 3 4
extern char *strstr (const char *__haystack, const char *__needle)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strtok (char *__restrict __s, const char *__restrict __delim)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));



extern char *__strtok_r (char *__restrict __s,
    const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));
# 360 "/usr/include/string.h" 3 4
extern char *strcasestr (const char *__haystack, const char *__needle)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));







extern void *memmem (const void *__haystack, size_t __haystacklen,
       const void *__needle, size_t __needlelen)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 3)));



extern void *__mempcpy (void *__restrict __dest,
   const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
extern void *mempcpy (void *__restrict __dest,
        const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));




extern size_t strlen (const char *__s)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern size_t strnlen (const char *__string, size_t __maxlen)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern char *strerror (int __errnum) __attribute__ ((__nothrow__ ));
# 421 "/usr/include/string.h" 3 4
extern char *strerror_r (int __errnum, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2))) ;





extern char *strerror_l (int __errnum, locale_t __l) __attribute__ ((__nothrow__ ));




# 1 "/usr/include/strings.h" 1 3 4
# 23 "/usr/include/strings.h" 3 4
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 24 "/usr/include/strings.h" 2 3 4
# 34 "/usr/include/strings.h" 3 4
extern int bcmp (const void *__s1, const void *__s2, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bcopy (const void *__src, void *__dest, size_t __n)
  __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 68 "/usr/include/strings.h" 3 4
extern char *index (const char *__s, int __c)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 96 "/usr/include/strings.h" 3 4
extern char *rindex (const char *__s, int __c)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));






extern int ffs (int __i) __attribute__ ((__nothrow__ )) __attribute__ ((__const__));





extern int ffsl (long int __l) __attribute__ ((__nothrow__ )) __attribute__ ((__const__));
__extension__ extern int ffsll (long long int __ll)
     __attribute__ ((__nothrow__ )) __attribute__ ((__const__));



extern int strcasecmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (const char *__s1, const char *__s2, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));






extern int strcasecmp_l (const char *__s1, const char *__s2, locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));



extern int strncasecmp_l (const char *__s1, const char *__s2,
     size_t __n, locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 4)));
# 433 "/usr/include/string.h" 2 3 4



extern void explicit_bzero (void *__s, size_t __n) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));



extern char *strsep (char **__restrict __stringp,
       const char *__restrict __delim)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));




extern char *strsignal (int __sig) __attribute__ ((__nothrow__ ));


extern char *__stpcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, const char *__restrict __src)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));




extern int strverscmp (const char *__s1, const char *__s2)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strfry (char *__string) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));


extern void *memfrob (void *__s, size_t __n) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 487 "/usr/include/string.h" 3 4
extern char *basename (const char *__filename) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 126 "./system.h" 2








# 1 "/usr/include/stdlib.h" 1 3 4
# 25 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 26 "/usr/include/stdlib.h" 2 3 4





# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 32 "/usr/include/stdlib.h" 2 3 4







# 1 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 1 3 4
# 52 "/usr/include/x86_64-linux-gnu/bits/waitflags.h" 3 4
typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
# 40 "/usr/include/stdlib.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/waitstatus.h" 1 3 4
# 41 "/usr/include/stdlib.h" 2 3 4
# 55 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 1 3 4
# 120 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/long-double.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 2 3 4
# 214 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 3 4
typedef float _Float32;
# 251 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 3 4
typedef double _Float64;
# 268 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 3 4
typedef double _Float32x;
# 285 "/usr/include/x86_64-linux-gnu/bits/floatn-common.h" 3 4
typedef long double _Float64x;
# 121 "/usr/include/x86_64-linux-gnu/bits/floatn.h" 2 3 4
# 56 "/usr/include/stdlib.h" 2 3 4


typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;





__extension__ typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;
# 97 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__ )) ;



extern double atof (const char *__nptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (const char *__nptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (const char *__nptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;



__extension__ extern long long int atoll (const char *__nptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;



extern double strtod (const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));



extern float strtof (const char *__restrict __nptr,
       char **__restrict __endptr) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));

extern long double strtold (const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 140 "/usr/include/stdlib.h" 3 4
extern _Float32 strtof32 (const char *__restrict __nptr,
     char **__restrict __endptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));



extern _Float64 strtof64 (const char *__restrict __nptr,
     char **__restrict __endptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 158 "/usr/include/stdlib.h" 3 4
extern _Float32x strtof32x (const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));



extern _Float64x strtof64x (const char *__restrict __nptr,
       char **__restrict __endptr)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 176 "/usr/include/stdlib.h" 3 4
extern long int strtol (const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));

extern unsigned long int strtoul (const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));



__extension__
extern long long int strtoq (const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtouq (const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));




__extension__
extern long long int strtoll (const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));

__extension__
extern unsigned long long int strtoull (const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));




extern int strfromd (char *__dest, size_t __size, const char *__format,
       double __f)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));

extern int strfromf (char *__dest, size_t __size, const char *__format,
       float __f)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));

extern int strfroml (char *__dest, size_t __size, const char *__format,
       long double __f)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));
# 232 "/usr/include/stdlib.h" 3 4
extern int strfromf32 (char *__dest, size_t __size, const char * __format,
         _Float32 __f)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));



extern int strfromf64 (char *__dest, size_t __size, const char * __format,
         _Float64 __f)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));
# 250 "/usr/include/stdlib.h" 3 4
extern int strfromf32x (char *__dest, size_t __size, const char * __format,
   _Float32x __f)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));



extern int strfromf64x (char *__dest, size_t __size, const char * __format,
   _Float64x __f)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));
# 274 "/usr/include/stdlib.h" 3 4
extern long int strtol_l (const char *__restrict __nptr,
     char **__restrict __endptr, int __base,
     locale_t __loc) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 4)));

extern unsigned long int strtoul_l (const char *__restrict __nptr,
        char **__restrict __endptr,
        int __base, locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 4)));

__extension__
extern long long int strtoll_l (const char *__restrict __nptr,
    char **__restrict __endptr, int __base,
    locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 4)));

__extension__
extern unsigned long long int strtoull_l (const char *__restrict __nptr,
       char **__restrict __endptr,
       int __base, locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 4)));

extern double strtod_l (const char *__restrict __nptr,
   char **__restrict __endptr, locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 3)));

extern float strtof_l (const char *__restrict __nptr,
         char **__restrict __endptr, locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 3)));

extern long double strtold_l (const char *__restrict __nptr,
         char **__restrict __endptr,
         locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 3)));
# 316 "/usr/include/stdlib.h" 3 4
extern _Float32 strtof32_l (const char *__restrict __nptr,
       char **__restrict __endptr,
       locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 3)));



extern _Float64 strtof64_l (const char *__restrict __nptr,
       char **__restrict __endptr,
       locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 3)));
# 337 "/usr/include/stdlib.h" 3 4
extern _Float32x strtof32x_l (const char *__restrict __nptr,
         char **__restrict __endptr,
         locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 3)));



extern _Float64x strtof64x_l (const char *__restrict __nptr,
         char **__restrict __endptr,
         locale_t __loc)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 3)));
# 385 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__ )) ;


extern long int a64l (const char *__s)
     __attribute__ ((__nothrow__ )) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;
# 401 "/usr/include/stdlib.h" 3 4
extern long int random (void) __attribute__ ((__nothrow__ ));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__ ));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));







struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };

extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));





extern int rand (void) __attribute__ ((__nothrow__ ));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__ ));



extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__ ));







extern double drand48 (void) __attribute__ ((__nothrow__ ));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__ ));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__ ));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__ ));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    __extension__ unsigned long long int __a;

  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));




extern void *malloc (size_t __size) __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__))
                                         ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__)) ;






extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__ )) __attribute__ ((__warn_unused_result__)) ;







extern void *reallocarray (void *__ptr, size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__ )) __attribute__ ((__warn_unused_result__))
                                      ;



extern void free (void *__ptr) __attribute__ ((__nothrow__ ));



# 1 "/usr/include/alloca.h" 1 3 4
# 24 "/usr/include/alloca.h" 3 4
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 25 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__ ));
# 569 "/usr/include/stdlib.h" 2 3 4





extern void *valloc (size_t __size) __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__))
                                         ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;




extern void *aligned_alloc (size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__)) ;



extern void abort (void) __attribute__ ((__nothrow__ )) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));







extern int at_quick_exit (void (*__func) (void)) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));






extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));





extern void exit (int __status) __attribute__ ((__nothrow__ )) __attribute__ ((__noreturn__));





extern void quick_exit (int __status) __attribute__ ((__nothrow__ )) __attribute__ ((__noreturn__));





extern void _Exit (int __status) __attribute__ ((__nothrow__ )) __attribute__ ((__noreturn__));




extern char *getenv (const char *__name) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;




extern char *secure_getenv (const char *__name)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;






extern int putenv (char *__string) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));





extern int setenv (const char *__name, const char *__value, int __replace)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (const char *__name) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));






extern int clearenv (void) __attribute__ ((__nothrow__ ));
# 675 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 688 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 698 "/usr/include/stdlib.h" 3 4
extern int mkstemp64 (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 710 "/usr/include/stdlib.h" 3 4
extern int mkstemps (char *__template, int __suffixlen) __attribute__ ((__nonnull__ (1))) ;
# 720 "/usr/include/stdlib.h" 3 4
extern int mkstemps64 (char *__template, int __suffixlen)
     __attribute__ ((__nonnull__ (1))) ;
# 731 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;
# 742 "/usr/include/stdlib.h" 3 4
extern int mkostemp (char *__template, int __flags) __attribute__ ((__nonnull__ (1))) ;
# 752 "/usr/include/stdlib.h" 3 4
extern int mkostemp64 (char *__template, int __flags) __attribute__ ((__nonnull__ (1))) ;
# 762 "/usr/include/stdlib.h" 3 4
extern int mkostemps (char *__template, int __suffixlen, int __flags)
     __attribute__ ((__nonnull__ (1))) ;
# 774 "/usr/include/stdlib.h" 3 4
extern int mkostemps64 (char *__template, int __suffixlen, int __flags)
     __attribute__ ((__nonnull__ (1))) ;
# 784 "/usr/include/stdlib.h" 3 4
extern int system (const char *__command) ;





extern char *canonicalize_file_name (const char *__name)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;
# 800 "/usr/include/stdlib.h" 3 4
extern char *realpath (const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__ )) ;






typedef int (*__compar_fn_t) (const void *, const void *);


typedef __compar_fn_t comparison_fn_t;



typedef int (*__compar_d_fn_t) (const void *, const void *, void *);




extern void *bsearch (const void *__key, const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;







extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));

extern void qsort_r (void *__base, size_t __nmemb, size_t __size,
       __compar_d_fn_t __compar, void *__arg)
  __attribute__ ((__nonnull__ (1, 4)));




extern int abs (int __x) __attribute__ ((__nothrow__ )) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__ )) __attribute__ ((__const__)) ;


__extension__ extern long long int llabs (long long int __x)
     __attribute__ ((__nothrow__ )) __attribute__ ((__const__)) ;






extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__ )) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__ )) __attribute__ ((__const__)) ;


__extension__ extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     __attribute__ ((__nothrow__ )) __attribute__ ((__const__)) ;
# 872 "/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3))) ;




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3))) ;




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4, 5)));





extern int mblen (const char *__s, size_t __n) __attribute__ ((__nothrow__ ));


extern int mbtowc (wchar_t *__restrict __pwc,
     const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ ));


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__ ));



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__ ));

extern size_t wcstombs (char *__restrict __s,
   const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__ ));







extern int rpmatch (const char *__response) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;
# 957 "/usr/include/stdlib.h" 3 4
extern int getsubopt (char **__restrict __optionp,
        char *const *__restrict __tokens,
        char **__restrict __valuep)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2, 3))) ;







extern int posix_openpt (int __oflag) ;







extern int grantpt (int __fd) __attribute__ ((__nothrow__ ));



extern int unlockpt (int __fd) __attribute__ ((__nothrow__ ));




extern char *ptsname (int __fd) __attribute__ ((__nothrow__ )) ;






extern int ptsname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));


extern int getpt (void);






extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 1013 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stdlib-float.h" 1 3 4
# 1014 "/usr/include/stdlib.h" 2 3 4
# 135 "./system.h" 2
# 157 "./system.h"
# 1 "/usr/include/unistd.h" 1 3 4
# 202 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/posix_opt.h" 1 3 4
# 203 "/usr/include/unistd.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/environments.h" 1 3 4
# 22 "/usr/include/x86_64-linux-gnu/bits/environments.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/environments.h" 2 3 4
# 207 "/usr/include/unistd.h" 2 3 4
# 226 "/usr/include/unistd.h" 3 4
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 227 "/usr/include/unistd.h" 2 3 4
# 267 "/usr/include/unistd.h" 3 4
typedef __intptr_t intptr_t;






typedef __socklen_t socklen_t;
# 287 "/usr/include/unistd.h" 3 4
extern int access (const char *__name, int __type) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));




extern int euidaccess (const char *__name, int __type)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));


extern int eaccess (const char *__name, int __type)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));






extern int faccessat (int __fd, const char *__file, int __type, int __flag)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2))) ;
# 334 "/usr/include/unistd.h" 3 4
extern __off_t lseek (int __fd, __off_t __offset, int __whence) __attribute__ ((__nothrow__ ));
# 345 "/usr/include/unistd.h" 3 4
extern __off64_t lseek64 (int __fd, __off64_t __offset, int __whence)
     __attribute__ ((__nothrow__ ));






extern int close (int __fd);






extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;





extern ssize_t write (int __fd, const void *__buf, size_t __n) ;
# 376 "/usr/include/unistd.h" 3 4
extern ssize_t pread (int __fd, void *__buf, size_t __nbytes,
        __off_t __offset) ;






extern ssize_t pwrite (int __fd, const void *__buf, size_t __n,
         __off_t __offset) ;
# 404 "/usr/include/unistd.h" 3 4
extern ssize_t pread64 (int __fd, void *__buf, size_t __nbytes,
   __off64_t __offset) ;


extern ssize_t pwrite64 (int __fd, const void *__buf, size_t __n,
    __off64_t __offset) ;







extern int pipe (int __pipedes[2]) __attribute__ ((__nothrow__ )) ;




extern int pipe2 (int __pipedes[2], int __flags) __attribute__ ((__nothrow__ )) ;
# 432 "/usr/include/unistd.h" 3 4
extern unsigned int alarm (unsigned int __seconds) __attribute__ ((__nothrow__ ));
# 444 "/usr/include/unistd.h" 3 4
extern unsigned int sleep (unsigned int __seconds);







extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     __attribute__ ((__nothrow__ ));






extern int usleep (__useconds_t __useconds);
# 469 "/usr/include/unistd.h" 3 4
extern int pause (void);



extern int chown (const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;



extern int fchown (int __fd, __uid_t __owner, __gid_t __group) __attribute__ ((__nothrow__ )) ;




extern int lchown (const char *__file, __uid_t __owner, __gid_t __group)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;






extern int fchownat (int __fd, const char *__file, __uid_t __owner,
       __gid_t __group, int __flag)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2))) ;



extern int chdir (const char *__path) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;



extern int fchdir (int __fd) __attribute__ ((__nothrow__ )) ;
# 511 "/usr/include/unistd.h" 3 4
extern char *getcwd (char *__buf, size_t __size) __attribute__ ((__nothrow__ )) ;





extern char *get_current_dir_name (void) __attribute__ ((__nothrow__ ));







extern char *getwd (char *__buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__)) ;




extern int dup (int __fd) __attribute__ ((__nothrow__ )) ;


extern int dup2 (int __fd, int __fd2) __attribute__ ((__nothrow__ ));




extern int dup3 (int __fd, int __fd2, int __flags) __attribute__ ((__nothrow__ ));



extern char **__environ;

extern char **environ;





extern int execve (const char *__path, char *const __argv[],
     char *const __envp[]) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));




extern int fexecve (int __fd, char *const __argv[], char *const __envp[])
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));




extern int execv (const char *__path, char *const __argv[])
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));



extern int execle (const char *__path, const char *__arg, ...)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));



extern int execl (const char *__path, const char *__arg, ...)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));



extern int execvp (const char *__file, char *const __argv[])
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));




extern int execlp (const char *__file, const char *__arg, ...)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));




extern int execvpe (const char *__file, char *const __argv[],
      char *const __envp[])
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));





extern int nice (int __inc) __attribute__ ((__nothrow__ )) ;




extern void _exit (int __status) __attribute__ ((__noreturn__));






# 1 "/usr/include/x86_64-linux-gnu/bits/confname.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/confname.h" 3 4
enum
  {
    _PC_LINK_MAX,

    _PC_MAX_CANON,

    _PC_MAX_INPUT,

    _PC_NAME_MAX,

    _PC_PATH_MAX,

    _PC_PIPE_BUF,

    _PC_CHOWN_RESTRICTED,

    _PC_NO_TRUNC,

    _PC_VDISABLE,

    _PC_SYNC_IO,

    _PC_ASYNC_IO,

    _PC_PRIO_IO,

    _PC_SOCK_MAXBUF,

    _PC_FILESIZEBITS,

    _PC_REC_INCR_XFER_SIZE,

    _PC_REC_MAX_XFER_SIZE,

    _PC_REC_MIN_XFER_SIZE,

    _PC_REC_XFER_ALIGN,

    _PC_ALLOC_SIZE_MIN,

    _PC_SYMLINK_MAX,

    _PC_2_SYMLINKS

  };


enum
  {
    _SC_ARG_MAX,

    _SC_CHILD_MAX,

    _SC_CLK_TCK,

    _SC_NGROUPS_MAX,

    _SC_OPEN_MAX,

    _SC_STREAM_MAX,

    _SC_TZNAME_MAX,

    _SC_JOB_CONTROL,

    _SC_SAVED_IDS,

    _SC_REALTIME_SIGNALS,

    _SC_PRIORITY_SCHEDULING,

    _SC_TIMERS,

    _SC_ASYNCHRONOUS_IO,

    _SC_PRIORITIZED_IO,

    _SC_SYNCHRONIZED_IO,

    _SC_FSYNC,

    _SC_MAPPED_FILES,

    _SC_MEMLOCK,

    _SC_MEMLOCK_RANGE,

    _SC_MEMORY_PROTECTION,

    _SC_MESSAGE_PASSING,

    _SC_SEMAPHORES,

    _SC_SHARED_MEMORY_OBJECTS,

    _SC_AIO_LISTIO_MAX,

    _SC_AIO_MAX,

    _SC_AIO_PRIO_DELTA_MAX,

    _SC_DELAYTIMER_MAX,

    _SC_MQ_OPEN_MAX,

    _SC_MQ_PRIO_MAX,

    _SC_VERSION,

    _SC_PAGESIZE,


    _SC_RTSIG_MAX,

    _SC_SEM_NSEMS_MAX,

    _SC_SEM_VALUE_MAX,

    _SC_SIGQUEUE_MAX,

    _SC_TIMER_MAX,




    _SC_BC_BASE_MAX,

    _SC_BC_DIM_MAX,

    _SC_BC_SCALE_MAX,

    _SC_BC_STRING_MAX,

    _SC_COLL_WEIGHTS_MAX,

    _SC_EQUIV_CLASS_MAX,

    _SC_EXPR_NEST_MAX,

    _SC_LINE_MAX,

    _SC_RE_DUP_MAX,

    _SC_CHARCLASS_NAME_MAX,


    _SC_2_VERSION,

    _SC_2_C_BIND,

    _SC_2_C_DEV,

    _SC_2_FORT_DEV,

    _SC_2_FORT_RUN,

    _SC_2_SW_DEV,

    _SC_2_LOCALEDEF,


    _SC_PII,

    _SC_PII_XTI,

    _SC_PII_SOCKET,

    _SC_PII_INTERNET,

    _SC_PII_OSI,

    _SC_POLL,

    _SC_SELECT,

    _SC_UIO_MAXIOV,

    _SC_IOV_MAX = _SC_UIO_MAXIOV,

    _SC_PII_INTERNET_STREAM,

    _SC_PII_INTERNET_DGRAM,

    _SC_PII_OSI_COTS,

    _SC_PII_OSI_CLTS,

    _SC_PII_OSI_M,

    _SC_T_IOV_MAX,



    _SC_THREADS,

    _SC_THREAD_SAFE_FUNCTIONS,

    _SC_GETGR_R_SIZE_MAX,

    _SC_GETPW_R_SIZE_MAX,

    _SC_LOGIN_NAME_MAX,

    _SC_TTY_NAME_MAX,

    _SC_THREAD_DESTRUCTOR_ITERATIONS,

    _SC_THREAD_KEYS_MAX,

    _SC_THREAD_STACK_MIN,

    _SC_THREAD_THREADS_MAX,

    _SC_THREAD_ATTR_STACKADDR,

    _SC_THREAD_ATTR_STACKSIZE,

    _SC_THREAD_PRIORITY_SCHEDULING,

    _SC_THREAD_PRIO_INHERIT,

    _SC_THREAD_PRIO_PROTECT,

    _SC_THREAD_PROCESS_SHARED,


    _SC_NPROCESSORS_CONF,

    _SC_NPROCESSORS_ONLN,

    _SC_PHYS_PAGES,

    _SC_AVPHYS_PAGES,

    _SC_ATEXIT_MAX,

    _SC_PASS_MAX,


    _SC_XOPEN_VERSION,

    _SC_XOPEN_XCU_VERSION,

    _SC_XOPEN_UNIX,

    _SC_XOPEN_CRYPT,

    _SC_XOPEN_ENH_I18N,

    _SC_XOPEN_SHM,


    _SC_2_CHAR_TERM,

    _SC_2_C_VERSION,

    _SC_2_UPE,


    _SC_XOPEN_XPG2,

    _SC_XOPEN_XPG3,

    _SC_XOPEN_XPG4,


    _SC_CHAR_BIT,

    _SC_CHAR_MAX,

    _SC_CHAR_MIN,

    _SC_INT_MAX,

    _SC_INT_MIN,

    _SC_LONG_BIT,

    _SC_WORD_BIT,

    _SC_MB_LEN_MAX,

    _SC_NZERO,

    _SC_SSIZE_MAX,

    _SC_SCHAR_MAX,

    _SC_SCHAR_MIN,

    _SC_SHRT_MAX,

    _SC_SHRT_MIN,

    _SC_UCHAR_MAX,

    _SC_UINT_MAX,

    _SC_ULONG_MAX,

    _SC_USHRT_MAX,


    _SC_NL_ARGMAX,

    _SC_NL_LANGMAX,

    _SC_NL_MSGMAX,

    _SC_NL_NMAX,

    _SC_NL_SETMAX,

    _SC_NL_TEXTMAX,


    _SC_XBS5_ILP32_OFF32,

    _SC_XBS5_ILP32_OFFBIG,

    _SC_XBS5_LP64_OFF64,

    _SC_XBS5_LPBIG_OFFBIG,


    _SC_XOPEN_LEGACY,

    _SC_XOPEN_REALTIME,

    _SC_XOPEN_REALTIME_THREADS,


    _SC_ADVISORY_INFO,

    _SC_BARRIERS,

    _SC_BASE,

    _SC_C_LANG_SUPPORT,

    _SC_C_LANG_SUPPORT_R,

    _SC_CLOCK_SELECTION,

    _SC_CPUTIME,

    _SC_THREAD_CPUTIME,

    _SC_DEVICE_IO,

    _SC_DEVICE_SPECIFIC,

    _SC_DEVICE_SPECIFIC_R,

    _SC_FD_MGMT,

    _SC_FIFO,

    _SC_PIPE,

    _SC_FILE_ATTRIBUTES,

    _SC_FILE_LOCKING,

    _SC_FILE_SYSTEM,

    _SC_MONOTONIC_CLOCK,

    _SC_MULTI_PROCESS,

    _SC_SINGLE_PROCESS,

    _SC_NETWORKING,

    _SC_READER_WRITER_LOCKS,

    _SC_SPIN_LOCKS,

    _SC_REGEXP,

    _SC_REGEX_VERSION,

    _SC_SHELL,

    _SC_SIGNALS,

    _SC_SPAWN,

    _SC_SPORADIC_SERVER,

    _SC_THREAD_SPORADIC_SERVER,

    _SC_SYSTEM_DATABASE,

    _SC_SYSTEM_DATABASE_R,

    _SC_TIMEOUTS,

    _SC_TYPED_MEMORY_OBJECTS,

    _SC_USER_GROUPS,

    _SC_USER_GROUPS_R,

    _SC_2_PBS,

    _SC_2_PBS_ACCOUNTING,

    _SC_2_PBS_LOCATE,

    _SC_2_PBS_MESSAGE,

    _SC_2_PBS_TRACK,

    _SC_SYMLOOP_MAX,

    _SC_STREAMS,

    _SC_2_PBS_CHECKPOINT,


    _SC_V6_ILP32_OFF32,

    _SC_V6_ILP32_OFFBIG,

    _SC_V6_LP64_OFF64,

    _SC_V6_LPBIG_OFFBIG,


    _SC_HOST_NAME_MAX,

    _SC_TRACE,

    _SC_TRACE_EVENT_FILTER,

    _SC_TRACE_INHERIT,

    _SC_TRACE_LOG,


    _SC_LEVEL1_ICACHE_SIZE,

    _SC_LEVEL1_ICACHE_ASSOC,

    _SC_LEVEL1_ICACHE_LINESIZE,

    _SC_LEVEL1_DCACHE_SIZE,

    _SC_LEVEL1_DCACHE_ASSOC,

    _SC_LEVEL1_DCACHE_LINESIZE,

    _SC_LEVEL2_CACHE_SIZE,

    _SC_LEVEL2_CACHE_ASSOC,

    _SC_LEVEL2_CACHE_LINESIZE,

    _SC_LEVEL3_CACHE_SIZE,

    _SC_LEVEL3_CACHE_ASSOC,

    _SC_LEVEL3_CACHE_LINESIZE,

    _SC_LEVEL4_CACHE_SIZE,

    _SC_LEVEL4_CACHE_ASSOC,

    _SC_LEVEL4_CACHE_LINESIZE,



    _SC_IPV6 = _SC_LEVEL1_ICACHE_SIZE + 50,

    _SC_RAW_SOCKETS,


    _SC_V7_ILP32_OFF32,

    _SC_V7_ILP32_OFFBIG,

    _SC_V7_LP64_OFF64,

    _SC_V7_LPBIG_OFFBIG,


    _SC_SS_REPL_MAX,


    _SC_TRACE_EVENT_NAME_MAX,

    _SC_TRACE_NAME_MAX,

    _SC_TRACE_SYS_MAX,

    _SC_TRACE_USER_EVENT_MAX,


    _SC_XOPEN_STREAMS,


    _SC_THREAD_ROBUST_PRIO_INHERIT,

    _SC_THREAD_ROBUST_PRIO_PROTECT

  };


enum
  {
    _CS_PATH,


    _CS_V6_WIDTH_RESTRICTED_ENVS,



    _CS_GNU_LIBC_VERSION,

    _CS_GNU_LIBPTHREAD_VERSION,


    _CS_V5_WIDTH_RESTRICTED_ENVS,



    _CS_V7_WIDTH_RESTRICTED_ENVS,



    _CS_LFS_CFLAGS = 1000,

    _CS_LFS_LDFLAGS,

    _CS_LFS_LIBS,

    _CS_LFS_LINTFLAGS,

    _CS_LFS64_CFLAGS,

    _CS_LFS64_LDFLAGS,

    _CS_LFS64_LIBS,

    _CS_LFS64_LINTFLAGS,


    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,

    _CS_XBS5_ILP32_OFF32_LDFLAGS,

    _CS_XBS5_ILP32_OFF32_LIBS,

    _CS_XBS5_ILP32_OFF32_LINTFLAGS,

    _CS_XBS5_ILP32_OFFBIG_CFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,

    _CS_XBS5_ILP32_OFFBIG_LIBS,

    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,

    _CS_XBS5_LP64_OFF64_CFLAGS,

    _CS_XBS5_LP64_OFF64_LDFLAGS,

    _CS_XBS5_LP64_OFF64_LIBS,

    _CS_XBS5_LP64_OFF64_LINTFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,

    _CS_XBS5_LPBIG_OFFBIG_LIBS,

    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_LIBS,

    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V6_LP64_OFF64_CFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V6_LP64_OFF64_LIBS,

    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS,


    _CS_POSIX_V7_ILP32_OFF32_CFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_LDFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_LIBS,

    _CS_POSIX_V7_ILP32_OFF32_LINTFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS,

    _CS_POSIX_V7_ILP32_OFFBIG_LIBS,

    _CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS,

    _CS_POSIX_V7_LP64_OFF64_CFLAGS,

    _CS_POSIX_V7_LP64_OFF64_LDFLAGS,

    _CS_POSIX_V7_LP64_OFF64_LIBS,

    _CS_POSIX_V7_LP64_OFF64_LINTFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LIBS,

    _CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS,


    _CS_V6_ENV,

    _CS_V7_ENV

  };
# 610 "/usr/include/unistd.h" 2 3 4


extern long int pathconf (const char *__path, int __name)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));


extern long int fpathconf (int __fd, int __name) __attribute__ ((__nothrow__ ));


extern long int sysconf (int __name) __attribute__ ((__nothrow__ ));



extern size_t confstr (int __name, char *__buf, size_t __len) __attribute__ ((__nothrow__ ));




extern __pid_t getpid (void) __attribute__ ((__nothrow__ ));


extern __pid_t getppid (void) __attribute__ ((__nothrow__ ));


extern __pid_t getpgrp (void) __attribute__ ((__nothrow__ ));


extern __pid_t __getpgid (__pid_t __pid) __attribute__ ((__nothrow__ ));

extern __pid_t getpgid (__pid_t __pid) __attribute__ ((__nothrow__ ));






extern int setpgid (__pid_t __pid, __pid_t __pgid) __attribute__ ((__nothrow__ ));
# 660 "/usr/include/unistd.h" 3 4
extern int setpgrp (void) __attribute__ ((__nothrow__ ));






extern __pid_t setsid (void) __attribute__ ((__nothrow__ ));



extern __pid_t getsid (__pid_t __pid) __attribute__ ((__nothrow__ ));



extern __uid_t getuid (void) __attribute__ ((__nothrow__ ));


extern __uid_t geteuid (void) __attribute__ ((__nothrow__ ));


extern __gid_t getgid (void) __attribute__ ((__nothrow__ ));


extern __gid_t getegid (void) __attribute__ ((__nothrow__ ));




extern int getgroups (int __size, __gid_t __list[]) __attribute__ ((__nothrow__ )) ;



extern int group_member (__gid_t __gid) __attribute__ ((__nothrow__ ));






extern int setuid (__uid_t __uid) __attribute__ ((__nothrow__ )) ;




extern int setreuid (__uid_t __ruid, __uid_t __euid) __attribute__ ((__nothrow__ )) ;




extern int seteuid (__uid_t __uid) __attribute__ ((__nothrow__ )) ;






extern int setgid (__gid_t __gid) __attribute__ ((__nothrow__ )) ;




extern int setregid (__gid_t __rgid, __gid_t __egid) __attribute__ ((__nothrow__ )) ;




extern int setegid (__gid_t __gid) __attribute__ ((__nothrow__ )) ;





extern int getresuid (__uid_t *__ruid, __uid_t *__euid, __uid_t *__suid)
     __attribute__ ((__nothrow__ ));



extern int getresgid (__gid_t *__rgid, __gid_t *__egid, __gid_t *__sgid)
     __attribute__ ((__nothrow__ ));



extern int setresuid (__uid_t __ruid, __uid_t __euid, __uid_t __suid)
     __attribute__ ((__nothrow__ )) ;



extern int setresgid (__gid_t __rgid, __gid_t __egid, __gid_t __sgid)
     __attribute__ ((__nothrow__ )) ;






extern __pid_t fork (void) __attribute__ ((__nothrow__));







extern __pid_t vfork (void) __attribute__ ((__nothrow__ ));





extern char *ttyname (int __fd) __attribute__ ((__nothrow__ ));



extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2))) ;



extern int isatty (int __fd) __attribute__ ((__nothrow__ ));




extern int ttyslot (void) __attribute__ ((__nothrow__ ));




extern int link (const char *__from, const char *__to)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2))) ;




extern int linkat (int __fromfd, const char *__from, int __tofd,
     const char *__to, int __flags)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 4))) ;




extern int symlink (const char *__from, const char *__to)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2))) ;




extern ssize_t readlink (const char *__restrict __path,
    char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2))) ;




extern int symlinkat (const char *__from, int __tofd,
        const char *__to) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 3))) ;


extern ssize_t readlinkat (int __fd, const char *__restrict __path,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3))) ;



extern int unlink (const char *__name) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));



extern int unlinkat (int __fd, const char *__name, int __flag)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));



extern int rmdir (const char *__path) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));



extern __pid_t tcgetpgrp (int __fd) __attribute__ ((__nothrow__ ));


extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) __attribute__ ((__nothrow__ ));






extern char *getlogin (void);







extern int getlogin_r (char *__name, size_t __name_len) __attribute__ ((__nonnull__ (1)));




extern int setlogin (const char *__name) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));








# 1 "/usr/include/x86_64-linux-gnu/bits/getopt_posix.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/getopt_posix.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/getopt_core.h" 1 3 4
# 36 "/usr/include/x86_64-linux-gnu/bits/getopt_core.h" 3 4
extern char *optarg;
# 50 "/usr/include/x86_64-linux-gnu/bits/getopt_core.h" 3 4
extern int optind;




extern int opterr;



extern int optopt;
# 91 "/usr/include/x86_64-linux-gnu/bits/getopt_core.h" 3 4
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));
# 28 "/usr/include/x86_64-linux-gnu/bits/getopt_posix.h" 2 3 4
# 870 "/usr/include/unistd.h" 2 3 4







extern int gethostname (char *__name, size_t __len) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));






extern int sethostname (const char *__name, size_t __len)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;



extern int sethostid (long int __id) __attribute__ ((__nothrow__ )) ;





extern int getdomainname (char *__name, size_t __len)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;
extern int setdomainname (const char *__name, size_t __len)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;





extern int vhangup (void) __attribute__ ((__nothrow__ ));


extern int revoke (const char *__file) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;







extern int profil (unsigned short int *__sample_buffer, size_t __size,
     size_t __offset, unsigned int __scale)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));





extern int acct (const char *__name) __attribute__ ((__nothrow__ ));



extern char *getusershell (void) __attribute__ ((__nothrow__ ));
extern void endusershell (void) __attribute__ ((__nothrow__ ));
extern void setusershell (void) __attribute__ ((__nothrow__ ));





extern int daemon (int __nochdir, int __noclose) __attribute__ ((__nothrow__ )) ;






extern int chroot (const char *__path) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;



extern char *getpass (const char *__prompt) __attribute__ ((__nonnull__ (1)));







extern int fsync (int __fd);





extern int syncfs (int __fd) __attribute__ ((__nothrow__ ));






extern long int gethostid (void);


extern void sync (void) __attribute__ ((__nothrow__ ));





extern int getpagesize (void) __attribute__ ((__nothrow__ )) __attribute__ ((__const__));




extern int getdtablesize (void) __attribute__ ((__nothrow__ ));
# 991 "/usr/include/unistd.h" 3 4
extern int truncate (const char *__file, __off_t __length)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;
# 1003 "/usr/include/unistd.h" 3 4
extern int truncate64 (const char *__file, __off64_t __length)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1))) ;
# 1014 "/usr/include/unistd.h" 3 4
extern int ftruncate (int __fd, __off_t __length) __attribute__ ((__nothrow__ )) ;
# 1024 "/usr/include/unistd.h" 3 4
extern int ftruncate64 (int __fd, __off64_t __length) __attribute__ ((__nothrow__ )) ;
# 1035 "/usr/include/unistd.h" 3 4
extern int brk (void *__addr) __attribute__ ((__nothrow__ )) ;





extern void *sbrk (intptr_t __delta) __attribute__ ((__nothrow__ ));
# 1056 "/usr/include/unistd.h" 3 4
extern long int syscall (long int __sysno, ...) __attribute__ ((__nothrow__ ));
# 1079 "/usr/include/unistd.h" 3 4
extern int lockf (int __fd, int __cmd, __off_t __len) ;
# 1089 "/usr/include/unistd.h" 3 4
extern int lockf64 (int __fd, int __cmd, __off64_t __len) ;
# 1107 "/usr/include/unistd.h" 3 4
ssize_t copy_file_range (int __infd, __off64_t *__pinoff,
    int __outfd, __off64_t *__poutoff,
    size_t __length, unsigned int __flags);





extern int fdatasync (int __fildes);
# 1124 "/usr/include/unistd.h" 3 4
extern char *crypt (const char *__key, const char *__salt)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));







extern void swab (const void *__restrict __from, void *__restrict __to,
    ssize_t __n) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
# 1161 "/usr/include/unistd.h" 3 4
int getentropy (void *__buffer, size_t __length) ;
# 1170 "/usr/include/unistd.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/unistd_ext.h" 1 3 4
# 34 "/usr/include/x86_64-linux-gnu/bits/unistd_ext.h" 3 4
extern __pid_t gettid (void) __attribute__ ((__nothrow__ ));
# 1171 "/usr/include/unistd.h" 2 3 4
# 158 "./system.h" 2
# 167 "./system.h"
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/limits.h" 1 3
# 21 "/usr/lib/llvm-10/lib/clang/10.0.0/include/limits.h" 3
# 1 "/usr/include/limits.h" 1 3 4
# 26 "/usr/include/limits.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 27 "/usr/include/limits.h" 2 3 4
# 183 "/usr/include/limits.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 28 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 2 3 4
# 161 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 1 3 4
# 38 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 3 4
# 1 "/usr/include/linux/limits.h" 1 3 4
# 39 "/usr/include/x86_64-linux-gnu/bits/local_lim.h" 2 3 4
# 162 "/usr/include/x86_64-linux-gnu/bits/posix1_lim.h" 2 3 4
# 184 "/usr/include/limits.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/posix2_lim.h" 1 3 4
# 188 "/usr/include/limits.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/xopen_lim.h" 1 3 4
# 64 "/usr/include/x86_64-linux-gnu/bits/xopen_lim.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/uio_lim.h" 1 3 4
# 65 "/usr/include/x86_64-linux-gnu/bits/xopen_lim.h" 2 3 4
# 192 "/usr/include/limits.h" 2 3 4
# 22 "/usr/lib/llvm-10/lib/clang/10.0.0/include/limits.h" 2 3
# 168 "./system.h" 2



# 1 "./hwint.h" 1
# 172 "./system.h" 2
# 207 "./system.h"
# 1 "/usr/include/time.h" 1 3 4
# 29 "/usr/include/time.h" 3 4
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 30 "/usr/include/time.h" 2 3 4



# 1 "/usr/include/x86_64-linux-gnu/bits/time.h" 1 3 4
# 73 "/usr/include/x86_64-linux-gnu/bits/time.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/timex.h" 1 3 4
# 26 "/usr/include/x86_64-linux-gnu/bits/timex.h" 3 4
struct timex
{
  unsigned int modes;
  __syscall_slong_t offset;
  __syscall_slong_t freq;
  __syscall_slong_t maxerror;
  __syscall_slong_t esterror;
  int status;
  __syscall_slong_t constant;
  __syscall_slong_t precision;
  __syscall_slong_t tolerance;
  struct timeval time;
  __syscall_slong_t tick;
  __syscall_slong_t ppsfreq;
  __syscall_slong_t jitter;
  int shift;
  __syscall_slong_t stabil;
  __syscall_slong_t jitcnt;
  __syscall_slong_t calcnt;
  __syscall_slong_t errcnt;
  __syscall_slong_t stbcnt;

  int tai;


  int :32; int :32; int :32; int :32;
  int :32; int :32; int :32; int :32;
  int :32; int :32; int :32;
};
# 74 "/usr/include/x86_64-linux-gnu/bits/time.h" 2 3 4




extern int clock_adjtime (__clockid_t __clock_id, struct timex *__utx) __attribute__ ((__nothrow__ ));
# 34 "/usr/include/time.h" 2 3 4





# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_tm.h" 1 3 4






struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;


  long int tm_gmtoff;
  const char *tm_zone;




};
# 40 "/usr/include/time.h" 2 3 4








# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_itimerspec.h" 1 3 4







struct itimerspec
  {
    struct timespec it_interval;
    struct timespec it_value;
  };
# 49 "/usr/include/time.h" 2 3 4
struct sigevent;
# 72 "/usr/include/time.h" 3 4
extern clock_t clock (void) __attribute__ ((__nothrow__ ));


extern time_t time (time_t *__timer) __attribute__ ((__nothrow__ ));


extern double difftime (time_t __time1, time_t __time0)
     __attribute__ ((__nothrow__ )) __attribute__ ((__const__));


extern time_t mktime (struct tm *__tp) __attribute__ ((__nothrow__ ));





extern size_t strftime (char *__restrict __s, size_t __maxsize,
   const char *__restrict __format,
   const struct tm *__restrict __tp) __attribute__ ((__nothrow__ ));




extern char *strptime (const char *__restrict __s,
         const char *__restrict __fmt, struct tm *__tp)
     __attribute__ ((__nothrow__ ));






extern size_t strftime_l (char *__restrict __s, size_t __maxsize,
     const char *__restrict __format,
     const struct tm *__restrict __tp,
     locale_t __loc) __attribute__ ((__nothrow__ ));



extern char *strptime_l (const char *__restrict __s,
    const char *__restrict __fmt, struct tm *__tp,
    locale_t __loc) __attribute__ ((__nothrow__ ));





extern struct tm *gmtime (const time_t *__timer) __attribute__ ((__nothrow__ ));



extern struct tm *localtime (const time_t *__timer) __attribute__ ((__nothrow__ ));




extern struct tm *gmtime_r (const time_t *__restrict __timer,
       struct tm *__restrict __tp) __attribute__ ((__nothrow__ ));



extern struct tm *localtime_r (const time_t *__restrict __timer,
          struct tm *__restrict __tp) __attribute__ ((__nothrow__ ));




extern char *asctime (const struct tm *__tp) __attribute__ ((__nothrow__ ));


extern char *ctime (const time_t *__timer) __attribute__ ((__nothrow__ ));






extern char *asctime_r (const struct tm *__restrict __tp,
   char *__restrict __buf) __attribute__ ((__nothrow__ ));


extern char *ctime_r (const time_t *__restrict __timer,
        char *__restrict __buf) __attribute__ ((__nothrow__ ));




extern char *__tzname[2];
extern int __daylight;
extern long int __timezone;




extern char *tzname[2];



extern void tzset (void) __attribute__ ((__nothrow__ ));



extern int daylight;
extern long int timezone;
# 190 "/usr/include/time.h" 3 4
extern time_t timegm (struct tm *__tp) __attribute__ ((__nothrow__ ));


extern time_t timelocal (struct tm *__tp) __attribute__ ((__nothrow__ ));


extern int dysize (int __year) __attribute__ ((__nothrow__ )) __attribute__ ((__const__));
# 205 "/usr/include/time.h" 3 4
extern int nanosleep (const struct timespec *__requested_time,
        struct timespec *__remaining);



extern int clock_getres (clockid_t __clock_id, struct timespec *__res) __attribute__ ((__nothrow__ ));


extern int clock_gettime (clockid_t __clock_id, struct timespec *__tp) __attribute__ ((__nothrow__ ));


extern int clock_settime (clockid_t __clock_id, const struct timespec *__tp)
     __attribute__ ((__nothrow__ ));






extern int clock_nanosleep (clockid_t __clock_id, int __flags,
       const struct timespec *__req,
       struct timespec *__rem);


extern int clock_getcpuclockid (pid_t __pid, clockid_t *__clock_id) __attribute__ ((__nothrow__ ));




extern int timer_create (clockid_t __clock_id,
    struct sigevent *__restrict __evp,
    timer_t *__restrict __timerid) __attribute__ ((__nothrow__ ));


extern int timer_delete (timer_t __timerid) __attribute__ ((__nothrow__ ));


extern int timer_settime (timer_t __timerid, int __flags,
     const struct itimerspec *__restrict __value,
     struct itimerspec *__restrict __ovalue) __attribute__ ((__nothrow__ ));


extern int timer_gettime (timer_t __timerid, struct itimerspec *__value)
     __attribute__ ((__nothrow__ ));


extern int timer_getoverrun (timer_t __timerid) __attribute__ ((__nothrow__ ));





extern int timespec_get (struct timespec *__ts, int __base)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));
# 274 "/usr/include/time.h" 3 4
extern int getdate_err;
# 283 "/usr/include/time.h" 3 4
extern struct tm *getdate (const char *__string);
# 297 "/usr/include/time.h" 3 4
extern int getdate_r (const char *__restrict __string,
        struct tm *__restrict __resbufp);
# 208 "./system.h" 2





# 1 "/usr/include/fcntl.h" 1 3 4
# 35 "/usr/include/fcntl.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/fcntl.h" 1 3 4
# 35 "/usr/include/x86_64-linux-gnu/bits/fcntl.h" 3 4
struct flock
  {
    short int l_type;
    short int l_whence;

    __off_t l_start;
    __off_t l_len;




    __pid_t l_pid;
  };


struct flock64
  {
    short int l_type;
    short int l_whence;
    __off64_t l_start;
    __off64_t l_len;
    __pid_t l_pid;
  };




# 1 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 1 3 4
# 38 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_iovec.h" 1 3 4
# 23 "/usr/include/x86_64-linux-gnu/bits/types/struct_iovec.h" 3 4
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/types/struct_iovec.h" 2 3 4


struct iovec
  {
    void *iov_base;
    size_t iov_len;
  };
# 39 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 2 3 4
# 265 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
enum __pid_type
  {
    F_OWNER_TID = 0,
    F_OWNER_PID,
    F_OWNER_PGRP,
    F_OWNER_GID = F_OWNER_PGRP
  };


struct f_owner_ex
  {
    enum __pid_type type;
    __pid_t pid;
  };
# 353 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
# 1 "/usr/include/linux/falloc.h" 1 3 4
# 354 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 2 3 4



struct file_handle
{
  unsigned int handle_bytes;
  int handle_type;

  unsigned char f_handle[0];
};
# 397 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
extern __ssize_t readahead (int __fd, __off64_t __offset, size_t __count)
    __attribute__ ((__nothrow__ ));






extern int sync_file_range (int __fd, __off64_t __offset, __off64_t __count,
       unsigned int __flags);






extern __ssize_t vmsplice (int __fdout, const struct iovec *__iov,
      size_t __count, unsigned int __flags);





extern __ssize_t splice (int __fdin, __off64_t *__offin, int __fdout,
    __off64_t *__offout, size_t __len,
    unsigned int __flags);





extern __ssize_t tee (int __fdin, int __fdout, size_t __len,
        unsigned int __flags);






extern int fallocate (int __fd, int __mode, __off_t __offset, __off_t __len);
# 447 "/usr/include/x86_64-linux-gnu/bits/fcntl-linux.h" 3 4
extern int fallocate64 (int __fd, int __mode, __off64_t __offset,
   __off64_t __len);




extern int name_to_handle_at (int __dfd, const char *__name,
         struct file_handle *__handle, int *__mnt_id,
         int __flags) __attribute__ ((__nothrow__ ));





extern int open_by_handle_at (int __mountdirfd, struct file_handle *__handle,
         int __flags);
# 62 "/usr/include/x86_64-linux-gnu/bits/fcntl.h" 2 3 4
# 36 "/usr/include/fcntl.h" 2 3 4
# 78 "/usr/include/fcntl.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stat.h" 1 3 4
# 46 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
struct stat
  {
    __dev_t st_dev;




    __ino_t st_ino;







    __nlink_t st_nlink;
    __mode_t st_mode;

    __uid_t st_uid;
    __gid_t st_gid;

    int __pad0;

    __dev_t st_rdev;




    __off_t st_size;



    __blksize_t st_blksize;

    __blkcnt_t st_blocks;
# 91 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
# 106 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
    __syscall_slong_t __glibc_reserved[3];
# 115 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
  };



struct stat64
  {
    __dev_t st_dev;

    __ino64_t st_ino;
    __nlink_t st_nlink;
    __mode_t st_mode;






    __uid_t st_uid;
    __gid_t st_gid;

    int __pad0;
    __dev_t st_rdev;
    __off_t st_size;





    __blksize_t st_blksize;
    __blkcnt64_t st_blocks;







    struct timespec st_atim;
    struct timespec st_mtim;
    struct timespec st_ctim;
# 164 "/usr/include/x86_64-linux-gnu/bits/stat.h" 3 4
    __syscall_slong_t __glibc_reserved[3];



  };
# 79 "/usr/include/fcntl.h" 2 3 4
# 148 "/usr/include/fcntl.h" 3 4
extern int fcntl (int __fd, int __cmd, ...);
# 157 "/usr/include/fcntl.h" 3 4
extern int fcntl64 (int __fd, int __cmd, ...);
# 168 "/usr/include/fcntl.h" 3 4
extern int open (const char *__file, int __oflag, ...) __attribute__ ((__nonnull__ (1)));
# 178 "/usr/include/fcntl.h" 3 4
extern int open64 (const char *__file, int __oflag, ...) __attribute__ ((__nonnull__ (1)));
# 192 "/usr/include/fcntl.h" 3 4
extern int openat (int __fd, const char *__file, int __oflag, ...)
     __attribute__ ((__nonnull__ (2)));
# 203 "/usr/include/fcntl.h" 3 4
extern int openat64 (int __fd, const char *__file, int __oflag, ...)
     __attribute__ ((__nonnull__ (2)));
# 214 "/usr/include/fcntl.h" 3 4
extern int creat (const char *__file, mode_t __mode) __attribute__ ((__nonnull__ (1)));
# 224 "/usr/include/fcntl.h" 3 4
extern int creat64 (const char *__file, mode_t __mode) __attribute__ ((__nonnull__ (1)));
# 260 "/usr/include/fcntl.h" 3 4
extern int posix_fadvise (int __fd, off_t __offset, off_t __len,
     int __advise) __attribute__ ((__nothrow__ ));
# 272 "/usr/include/fcntl.h" 3 4
extern int posix_fadvise64 (int __fd, off64_t __offset, off64_t __len,
       int __advise) __attribute__ ((__nothrow__ ));
# 282 "/usr/include/fcntl.h" 3 4
extern int posix_fallocate (int __fd, off_t __offset, off_t __len);
# 293 "/usr/include/fcntl.h" 3 4
extern int posix_fallocate64 (int __fd, off64_t __offset, off64_t __len);
# 214 "./system.h" 2
# 312 "./system.h"
# 1 "/usr/include/malloc.h" 1 3 4
# 23 "/usr/include/malloc.h" 3 4
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 24 "/usr/include/malloc.h" 2 3 4
# 38 "/usr/include/malloc.h" 3 4
extern void *malloc (size_t __size) __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__))
                                         ;


extern void *calloc (size_t __nmemb, size_t __size)
__attribute__ ((__nothrow__ )) __attribute__ ((__malloc__)) ;






extern void *realloc (void *__ptr, size_t __size)
__attribute__ ((__nothrow__ )) __attribute__ ((__warn_unused_result__)) ;






extern void *reallocarray (void *__ptr, size_t __nmemb, size_t __size)
__attribute__ ((__nothrow__ )) __attribute__ ((__warn_unused_result__)) ;


extern void free (void *__ptr) __attribute__ ((__nothrow__ ));


extern void *memalign (size_t __alignment, size_t __size)
__attribute__ ((__nothrow__ )) __attribute__ ((__malloc__)) ;


extern void *valloc (size_t __size) __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__))
                                         ;



extern void *pvalloc (size_t __size) __attribute__ ((__nothrow__ )) __attribute__ ((__malloc__)) ;



extern void *(*__morecore) (ptrdiff_t __size);


extern void *__default_morecore (ptrdiff_t __size)
__attribute__ ((__nothrow__ )) __attribute__ ((__malloc__));



struct mallinfo
{
  int arena;
  int ordblks;
  int smblks;
  int hblks;
  int hblkhd;
  int usmblks;
  int fsmblks;
  int uordblks;
  int fordblks;
  int keepcost;
};


extern struct mallinfo mallinfo (void) __attribute__ ((__nothrow__ ));
# 128 "/usr/include/malloc.h" 3 4
extern int mallopt (int __param, int __val) __attribute__ ((__nothrow__ ));



extern int malloc_trim (size_t __pad) __attribute__ ((__nothrow__ ));



extern size_t malloc_usable_size (void *__ptr) __attribute__ ((__nothrow__ ));


extern void malloc_stats (void) __attribute__ ((__nothrow__ ));


extern int malloc_info (int __options, FILE *__fp) __attribute__ ((__nothrow__ ));


extern void (*volatile __free_hook) (void *__ptr,
                                                   const void *)
__attribute__ ((__deprecated__));
extern void *(*volatile __malloc_hook)(size_t __size,
                                                     const void *)
__attribute__ ((__deprecated__));
extern void *(*volatile __realloc_hook)(void *__ptr,
                                                      size_t __size,
                                                      const void *)
__attribute__ ((__deprecated__));
extern void *(*volatile __memalign_hook)(size_t __alignment,
                                                       size_t __size,
                                                       const void *)
__attribute__ ((__deprecated__));
extern void (*volatile __after_morecore_hook) (void);
# 313 "./system.h" 2
# 381 "./system.h"
# 1 "/usr/include/x86_64-linux-gnu/sys/stat.h" 1 3 4
# 101 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/stat.h" 1 3 4
# 102 "/usr/include/x86_64-linux-gnu/sys/stat.h" 2 3 4
# 205 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int stat (const char *__restrict __file,
   struct stat *__restrict __buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));



extern int fstat (int __fd, struct stat *__buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));
# 224 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int stat64 (const char *__restrict __file,
     struct stat64 *__restrict __buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
extern int fstat64 (int __fd, struct stat64 *__buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));







extern int fstatat (int __fd, const char *__restrict __file,
      struct stat *__restrict __buf, int __flag)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));
# 249 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int fstatat64 (int __fd, const char *__restrict __file,
        struct stat64 *__restrict __buf, int __flag)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));







extern int lstat (const char *__restrict __file,
    struct stat *__restrict __buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));
# 272 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int lstat64 (const char *__restrict __file,
      struct stat64 *__restrict __buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1, 2)));





extern int chmod (const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));





extern int lchmod (const char *__file, __mode_t __mode)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));




extern int fchmod (int __fd, __mode_t __mode) __attribute__ ((__nothrow__ ));





extern int fchmodat (int __fd, const char *__file, __mode_t __mode,
       int __flag)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2))) ;






extern __mode_t umask (__mode_t __mask) __attribute__ ((__nothrow__ ));




extern __mode_t getumask (void) __attribute__ ((__nothrow__ ));



extern int mkdir (const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));





extern int mkdirat (int __fd, const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));






extern int mknod (const char *__path, __mode_t __mode, __dev_t __dev)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));





extern int mknodat (int __fd, const char *__path, __mode_t __mode,
      __dev_t __dev) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));





extern int mkfifo (const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (1)));





extern int mkfifoat (int __fd, const char *__path, __mode_t __mode)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));





extern int utimensat (int __fd, const char *__path,
        const struct timespec __times[2],
        int __flags)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2)));




extern int futimens (int __fd, const struct timespec __times[2]) __attribute__ ((__nothrow__ ));
# 395 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int __fxstat (int __ver, int __fildes, struct stat *__stat_buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));
extern int __xstat (int __ver, const char *__filename,
      struct stat *__stat_buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat (int __ver, const char *__filename,
       struct stat *__stat_buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstatat (int __ver, int __fildes, const char *__filename,
         struct stat *__stat_buf, int __flag)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4)));
# 428 "/usr/include/x86_64-linux-gnu/sys/stat.h" 3 4
extern int __fxstat64 (int __ver, int __fildes, struct stat64 *__stat_buf)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3)));
extern int __xstat64 (int __ver, const char *__filename,
        struct stat64 *__stat_buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));
extern int __lxstat64 (int __ver, const char *__filename,
         struct stat64 *__stat_buf) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 3)));
extern int __fxstatat64 (int __ver, int __fildes, const char *__filename,
    struct stat64 *__stat_buf, int __flag)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 4)));

extern int __xmknod (int __ver, const char *__path, __mode_t __mode,
       __dev_t *__dev) __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 4)));

extern int __xmknodat (int __ver, int __fd, const char *__path,
         __mode_t __mode, __dev_t *__dev)
     __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (3, 5)));



# 1 "/usr/include/x86_64-linux-gnu/bits/statx.h" 1 3 4
# 31 "/usr/include/x86_64-linux-gnu/bits/statx.h" 3 4
# 1 "/usr/include/linux/stat.h" 1 3 4




# 1 "/usr/include/linux/types.h" 1 3 4




# 1 "/usr/include/x86_64-linux-gnu/asm/types.h" 1 3 4
# 1 "/usr/include/asm-generic/types.h" 1 3 4






# 1 "/usr/include/asm-generic/int-ll64.h" 1 3 4
# 12 "/usr/include/asm-generic/int-ll64.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/bitsperlong.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/asm/bitsperlong.h" 3 4
# 1 "/usr/include/asm-generic/bitsperlong.h" 1 3 4
# 12 "/usr/include/x86_64-linux-gnu/asm/bitsperlong.h" 2 3 4
# 13 "/usr/include/asm-generic/int-ll64.h" 2 3 4







typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;


__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
# 8 "/usr/include/asm-generic/types.h" 2 3 4
# 2 "/usr/include/x86_64-linux-gnu/asm/types.h" 2 3 4
# 6 "/usr/include/linux/types.h" 2 3 4



# 1 "/usr/include/linux/posix_types.h" 1 3 4




# 1 "/usr/include/linux/stddef.h" 1 3 4
# 6 "/usr/include/linux/posix_types.h" 2 3 4
# 25 "/usr/include/linux/posix_types.h" 3 4
typedef struct {
 unsigned long fds_bits[1024 / (8 * sizeof(long))];
} __kernel_fd_set;


typedef void (*__kernel_sighandler_t)(int);


typedef int __kernel_key_t;
typedef int __kernel_mqd_t;


# 1 "/usr/include/x86_64-linux-gnu/asm/posix_types.h" 1 3 4






# 1 "/usr/include/x86_64-linux-gnu/asm/posix_types_64.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/asm/posix_types_64.h" 3 4
typedef unsigned short __kernel_old_uid_t;
typedef unsigned short __kernel_old_gid_t;


typedef unsigned long __kernel_old_dev_t;



# 1 "/usr/include/asm-generic/posix_types.h" 1 3 4
# 15 "/usr/include/asm-generic/posix_types.h" 3 4
typedef long __kernel_long_t;
typedef unsigned long __kernel_ulong_t;



typedef __kernel_ulong_t __kernel_ino_t;



typedef unsigned int __kernel_mode_t;



typedef int __kernel_pid_t;



typedef int __kernel_ipc_pid_t;



typedef unsigned int __kernel_uid_t;
typedef unsigned int __kernel_gid_t;



typedef __kernel_long_t __kernel_suseconds_t;



typedef int __kernel_daddr_t;



typedef unsigned int __kernel_uid32_t;
typedef unsigned int __kernel_gid32_t;
# 72 "/usr/include/asm-generic/posix_types.h" 3 4
typedef __kernel_ulong_t __kernel_size_t;
typedef __kernel_long_t __kernel_ssize_t;
typedef __kernel_long_t __kernel_ptrdiff_t;




typedef struct {
 int val[2];
} __kernel_fsid_t;





typedef __kernel_long_t __kernel_off_t;
typedef long long __kernel_loff_t;
typedef __kernel_long_t __kernel_time_t;
typedef long long __kernel_time64_t;
typedef __kernel_long_t __kernel_clock_t;
typedef int __kernel_timer_t;
typedef int __kernel_clockid_t;
typedef char * __kernel_caddr_t;
typedef unsigned short __kernel_uid16_t;
typedef unsigned short __kernel_gid16_t;
# 19 "/usr/include/x86_64-linux-gnu/asm/posix_types_64.h" 2 3 4
# 8 "/usr/include/x86_64-linux-gnu/asm/posix_types.h" 2 3 4
# 37 "/usr/include/linux/posix_types.h" 2 3 4
# 10 "/usr/include/linux/types.h" 2 3 4
# 24 "/usr/include/linux/types.h" 3 4
typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;

typedef __u16 __sum16;
typedef __u32 __wsum;
# 47 "/usr/include/linux/types.h" 3 4
typedef unsigned __poll_t;
# 6 "/usr/include/linux/stat.h" 2 3 4
# 56 "/usr/include/linux/stat.h" 3 4
struct statx_timestamp {
 __s64 tv_sec;
 __u32 tv_nsec;
 __s32 __reserved;
};
# 99 "/usr/include/linux/stat.h" 3 4
struct statx {

 __u32 stx_mask;
 __u32 stx_blksize;
 __u64 stx_attributes;

 __u32 stx_nlink;
 __u32 stx_uid;
 __u32 stx_gid;
 __u16 stx_mode;
 __u16 __spare0[1];

 __u64 stx_ino;
 __u64 stx_size;
 __u64 stx_blocks;
 __u64 stx_attributes_mask;

 struct statx_timestamp stx_atime;
 struct statx_timestamp stx_btime;
 struct statx_timestamp stx_ctime;
 struct statx_timestamp stx_mtime;

 __u32 stx_rdev_major;
 __u32 stx_rdev_minor;
 __u32 stx_dev_major;
 __u32 stx_dev_minor;

 __u64 __spare2[14];

};
# 32 "/usr/include/x86_64-linux-gnu/bits/statx.h" 2 3 4







# 1 "/usr/include/x86_64-linux-gnu/bits/statx-generic.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/statx-generic.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_statx_timestamp.h" 1 3 4
# 26 "/usr/include/x86_64-linux-gnu/bits/statx-generic.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/types/struct_statx.h" 1 3 4
# 27 "/usr/include/x86_64-linux-gnu/bits/statx-generic.h" 2 3 4
# 56 "/usr/include/x86_64-linux-gnu/bits/statx-generic.h" 3 4
int statx (int __dirfd, const char *__restrict __path, int __flags,
           unsigned int __mask, struct statx *__restrict __buf)
  __attribute__ ((__nothrow__ )) __attribute__ ((__nonnull__ (2, 5)));
# 40 "/usr/include/x86_64-linux-gnu/bits/statx.h" 2 3 4
# 447 "/usr/include/x86_64-linux-gnu/sys/stat.h" 2 3 4
# 382 "./system.h" 2
# 493 "./system.h"
# 1 "./libiberty.h" 1
# 46 "./libiberty.h"
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3
# 47 "./libiberty.h" 2
# 59 "./libiberty.h"
extern char **buildargv (const char *) __attribute__ ((__malloc__));



extern void freeargv (char **);




extern char **dupargv (char **) __attribute__ ((__malloc__));
# 82 "./libiberty.h"
extern char *basename (const char *);
# 91 "./libiberty.h"
extern const char *lbasename (const char *);





extern char *concat (const char *, ...) __attribute__ ((__malloc__));
# 106 "./libiberty.h"
extern char *reconcat (char *, const char *, ...) __attribute__ ((__malloc__));





extern unsigned long concat_length (const char *, ...);






extern char *concat_copy (char *, const char *, ...);






extern char *concat_copy2 (const char *, ...);



extern char *libiberty_concat_ptr;
# 142 "./libiberty.h"
extern int fdmatch (int fd1, int fd2);




extern char * getpwd (void);



extern long get_run_time (void);



extern char *choose_temp_base (void) __attribute__ ((__malloc__));



extern char *make_temp_file (const char *) __attribute__ ((__malloc__));



extern const char *spaces (int count);




extern int errno_max (void);




extern const char *strerrno (int);



extern int strtoerrno (const char *);



extern char *xstrerror (int);




extern int signo_max (void);
# 198 "./libiberty.h"
extern const char *strsigno (int);



extern int strtosigno (const char *);



extern int xatexit (void (*fn) (void));



extern void xexit (int status) __attribute__ ((__noreturn__));



extern void xmalloc_set_program_name (const char *);


extern void xmalloc_failed (size_t) __attribute__ ((__noreturn__));





extern void * xmalloc (size_t) __attribute__ ((__malloc__));





extern void * xrealloc (void *, size_t);




extern void * xcalloc (size_t, size_t) __attribute__ ((__malloc__));



extern char *xstrdup (const char *) __attribute__ ((__malloc__));



extern void * xmemdup (const void *, size_t, size_t) __attribute__ ((__malloc__));


extern void specqsort (void *, int, int, int (*)() );





extern const char _hex_value[256];
extern void hex_init (void);
# 268 "./libiberty.h"
extern int pexecute (const char *, char * const *, const char *, const char *, char **, char **, int);




extern int pwait (int, int *, int);




extern int asprintf (char **, const char *, ...) __attribute__ ((__format__ (__printf__, 2, 3)));




extern int vasprintf (char **, const char *, va_list)
  __attribute__ ((__format__ (__printf__, 2, 0)));
# 494 "./system.h" 2
# 1 "./symcat.h" 1
# 495 "./system.h" 2
# 38 "tree.c" 2
# 1 "./flags.h" 1
# 26 "./flags.h"
extern const char *main_input_filename;

enum debug_info_type
{
  NO_DEBUG,
  DBX_DEBUG,
  SDB_DEBUG,
  DWARF_DEBUG,
  DWARF2_DEBUG,
  XCOFF_DEBUG,
  VMS_DEBUG,
  VMS_AND_DWARF2_DEBUG

};


extern enum debug_info_type write_symbols;

enum debug_info_level
{
  DINFO_LEVEL_NONE,
  DINFO_LEVEL_TERSE,
  DINFO_LEVEL_NORMAL,
  DINFO_LEVEL_VERBOSE
};


extern enum debug_info_level debug_info_level;



extern int use_gnu_debug_info_extensions;



extern int optimize;



extern int optimize_size;




extern int quiet_flag;



extern int time_report;




extern int mem_report;



extern int inhibit_warnings;



extern int warn_system_headers;



extern int extra_warnings;





extern void set_Wunused (int setting);

extern int warn_unused_function;
extern int warn_unused_label;
extern int warn_unused_parameter;
extern int warn_unused_variable;
extern int warn_unused_value;



extern int warn_notreached;



extern int warn_inline;



extern int warn_uninitialized;







extern int warn_unknown_pragmas;



extern int warn_shadow;



extern int warn_switch;




extern int warn_return_type;



extern int warn_missing_noreturn;





extern int warn_cast_align;





extern int warn_larger_than;
extern long larger_than_size;




extern int warn_aggregate_return;



extern int warn_packed;



extern int warn_padded;



extern int warn_disabled_optimization;




extern int warn_deprecated_decl;



extern int profile_flag;



extern int profile_arc_flag;



extern int flag_test_coverage;



extern int flag_branch_probabilities;



extern int flag_reorder_blocks;



extern int flag_rename_registers;




extern int pedantic;




extern int in_system_header;




extern int flag_print_asm_name;





extern int flag_signed_char;



extern int flag_short_enums;





extern int flag_caller_saves;



extern int flag_pcc_struct_return;




extern int flag_force_mem;




extern int flag_force_addr;




extern int flag_defer_pop;




extern int flag_float_store;



extern int flag_strength_reduce;






extern int flag_unroll_loops;




extern int flag_unroll_all_loops;




extern int flag_move_all_movables;



extern int flag_prefetch_loop_arrays;




extern int flag_reduce_all_givs;




extern int flag_cse_follow_jumps;




extern int flag_cse_skip_blocks;



extern int flag_expensive_optimizations;




extern int flag_writable_strings;





extern int flag_no_function_cse;




extern int flag_omit_frame_pointer;



extern int flag_no_peephole;



extern int flag_volatile;



extern int flag_volatile_global;



extern int flag_volatile_static;



extern int flag_optimize_sibling_calls;




extern int flag_errno_math;






extern int flag_unsafe_math_optimizations;





extern int flag_trapping_math;





extern int flag_complex_divide_method;



extern int flag_rerun_loop_opt;




extern int flag_inline_functions;





extern int flag_keep_inline_functions;







extern int flag_no_inline;




extern int flag_really_no_inline;



extern int flag_syntax_only;



extern int flag_gen_aux_info;



extern int flag_shared_data;






extern int flag_schedule_insns;
extern int flag_schedule_insns_after_reload;
# 418 "./flags.h"
extern int flag_schedule_interblock;
extern int flag_schedule_speculative;
extern int flag_schedule_speculative_load;
extern int flag_schedule_speculative_load_dangerous;



extern int flag_branch_on_count_reg;





extern int flag_single_precision_constant;



extern int flag_delayed_branch;




extern int flag_dump_unnumbered;





extern int flag_pretend_float;




extern int flag_pedantic_errors;




extern int flag_pic;




extern int flag_exceptions;



extern int flag_unwind_tables;



extern int flag_asynchronous_unwind_tables;




extern int flag_no_common;





extern int flag_inhibit_size_directive;




extern int flag_function_sections;



extern int flag_data_sections;
# 498 "./flags.h"
extern int flag_verbose_asm;
# 507 "./flags.h"
extern int flag_debug_asm;

extern int flag_dump_rtl_in_asm;



extern int flag_gnu_linker;


extern int flag_pack_struct;
# 525 "./flags.h"
extern int flag_argument_noalias;





extern int flag_strict_aliasing;



extern int flag_stack_check;


extern int flag_regmove;


extern int flag_instrument_function_entry_exit;


extern int flag_peephole2;


extern int flag_guess_branch_prob;






extern int flag_bounded_pointers;







extern int flag_bounds_check;




extern int flag_merge_constants;




extern int flag_renumber_insns;







extern int frame_pointer_needed;



extern int flag_trapv;


extern int g_switch_value;
extern int g_switch_set;






extern int align_loops;
extern int align_loops_log;
extern int align_loops_max_skip;
extern int align_jumps;
extern int align_jumps_log;
extern int align_jumps_max_skip;
extern int align_labels;
extern int align_labels_log;
extern int align_labels_max_skip;
extern int align_functions;
extern int align_functions_log;


extern int dump_for_graph;


enum graph_dump_types
{
  no_graph = 0,
  vcg
};
extern enum graph_dump_types graph_dump_format;





extern int flag_no_ident;



extern int flag_gcse_lm;



extern int flag_gcse_sm;




extern int flag_eliminate_dwarf2_dups;



extern int flag_detailed_statistics;


extern int flag_non_call_exceptions;
# 39 "tree.c" 2
# 1 "./tree.h" 1
# 22 "./tree.h"
# 1 "./machmode.h" 1
# 29 "./machmode.h"
enum machine_mode {

# 1 "./machmode.def" 1
# 74 "./machmode.def"
VOIDmode,

BImode,
QImode,
HImode,
SImode,
DImode,
TImode,
OImode,




PQImode,
PHImode,
PSImode,
PDImode,

QFmode,
HFmode,
TQFmode,
SFmode,
DFmode,
XFmode,
TFmode,


QCmode,
HCmode,
SCmode,
DCmode,
XCmode,
TCmode,

CQImode,
CHImode,
CSImode,
CDImode,
CTImode,
COImode,







V2QImode,
V2HImode,
V2SImode,
V2DImode,

V4QImode,
V4HImode,
V4SImode,
V4DImode,

V8QImode,
V8HImode,
V8SImode,
V8DImode,

V16QImode,

V2SFmode,
V2DFmode,

V4SFmode,
V4DFmode,

V8SFmode,
V8DFmode,
V16SFmode,



BLKmode,
# 159 "./machmode.def"
CCmode,


CCGCmode, CCGOCmode, CCNOmode, CCZmode, CCFPmode, CCFPUmode,
# 31 "./machmode.h" 2
MAX_MACHINE_MODE };
# 41 "./machmode.h"
extern const char * const mode_name[(int) MAX_MACHINE_MODE];


enum mode_class { MODE_RANDOM, MODE_INT, MODE_FLOAT, MODE_PARTIAL_INT, MODE_CC,
    MODE_COMPLEX_INT, MODE_COMPLEX_FLOAT,
    MODE_VECTOR_INT, MODE_VECTOR_FLOAT,
    MAX_MODE_CLASS};




extern const enum mode_class mode_class[(int) MAX_MACHINE_MODE];
# 80 "./machmode.h"
extern const unsigned char mode_size[(int) MAX_MACHINE_MODE];




extern const unsigned char mode_unit_size[(int) MAX_MACHINE_MODE];
# 96 "./machmode.h"
extern const unsigned short mode_bitsize[(int) MAX_MACHINE_MODE];
# 106 "./machmode.h"
extern const unsigned long mode_mask_array[(int) MAX_MACHINE_MODE];



extern const enum machine_mode inner_mode_array[(int) MAX_MACHINE_MODE];
# 123 "./machmode.h"
extern const unsigned char mode_wider_mode[(int) MAX_MACHINE_MODE];






extern enum machine_mode mode_for_size (unsigned int, enum mode_class, int);




extern enum machine_mode smallest_mode_for_size
    (unsigned int, enum mode_class);





extern enum machine_mode int_mode_for_mode (enum machine_mode);



extern enum machine_mode get_best_mode (int, int, unsigned int, enum machine_mode, int);




extern unsigned get_mode_alignment (enum machine_mode);





extern const enum machine_mode class_narrowest_mode[(int) MAX_MODE_CLASS];





extern enum machine_mode byte_mode;
extern enum machine_mode word_mode;
extern enum machine_mode ptr_mode;
# 23 "./tree.h" 2
# 1 "./version.h" 1


extern const char *const version_string;
# 24 "./tree.h" 2





enum tree_code {

# 1 "./tree.def" 1
# 46 "./tree.def"
ERROR_MARK,





IDENTIFIER_NODE,






TREE_LIST,


TREE_VEC,
# 84 "./tree.def"
BLOCK,
# 129 "./tree.def"
VOID_TYPE,
# 139 "./tree.def"
INTEGER_TYPE,



REAL_TYPE,



COMPLEX_TYPE,



VECTOR_TYPE,
# 164 "./tree.def"
ENUMERAL_TYPE,



BOOLEAN_TYPE,



CHAR_TYPE,



POINTER_TYPE,





OFFSET_TYPE,



REFERENCE_TYPE,






METHOD_TYPE,


FILE_TYPE,
# 209 "./tree.def"
ARRAY_TYPE,





SET_TYPE,
# 225 "./tree.def"
RECORD_TYPE,





UNION_TYPE,





QUAL_UNION_TYPE,







FUNCTION_TYPE,





LANG_TYPE,
# 262 "./tree.def"
INTEGER_CST,


REAL_CST,




COMPLEX_CST,


VECTOR_CST,



STRING_CST,
# 336 "./tree.def"
FUNCTION_DECL,
LABEL_DECL,
CONST_DECL,
TYPE_DECL,
VAR_DECL,
PARM_DECL,
RESULT_DECL,
FIELD_DECL,



NAMESPACE_DECL,






COMPONENT_REF,
# 363 "./tree.def"
BIT_FIELD_REF,


INDIRECT_REF,


BUFFER_REF,



ARRAY_REF,




ARRAY_RANGE_REF,






VTABLE_REF,
# 409 "./tree.def"
CONSTRUCTOR,
# 419 "./tree.def"
COMPOUND_EXPR,


MODIFY_EXPR,



INIT_EXPR,






TARGET_EXPR,
# 443 "./tree.def"
COND_EXPR,
# 466 "./tree.def"
BIND_EXPR,




CALL_EXPR,




METHOD_CALL_EXPR,
# 490 "./tree.def"
WITH_CLEANUP_EXPR,
# 507 "./tree.def"
CLEANUP_POINT_EXPR,
# 556 "./tree.def"
PLACEHOLDER_EXPR,





WITH_RECORD_EXPR,


PLUS_EXPR,
MINUS_EXPR,
MULT_EXPR,


TRUNC_DIV_EXPR,


CEIL_DIV_EXPR,


FLOOR_DIV_EXPR,


ROUND_DIV_EXPR,


TRUNC_MOD_EXPR,
CEIL_MOD_EXPR,
FLOOR_MOD_EXPR,
ROUND_MOD_EXPR,


RDIV_EXPR,



EXACT_DIV_EXPR,






FIX_TRUNC_EXPR,
FIX_CEIL_EXPR,
FIX_FLOOR_EXPR,
FIX_ROUND_EXPR,


FLOAT_EXPR,


NEGATE_EXPR,

MIN_EXPR,
MAX_EXPR,





ABS_EXPR,

FFS_EXPR,
# 628 "./tree.def"
LSHIFT_EXPR,
RSHIFT_EXPR,
LROTATE_EXPR,
RROTATE_EXPR,


BIT_IOR_EXPR,
BIT_XOR_EXPR,
BIT_AND_EXPR,
BIT_ANDTC_EXPR,
BIT_NOT_EXPR,
# 649 "./tree.def"
TRUTH_ANDIF_EXPR,
TRUTH_ORIF_EXPR,
TRUTH_AND_EXPR,
TRUTH_OR_EXPR,
TRUTH_XOR_EXPR,
TRUTH_NOT_EXPR,







LT_EXPR,
LE_EXPR,
GT_EXPR,
GE_EXPR,
EQ_EXPR,
NE_EXPR,


UNORDERED_EXPR,
ORDERED_EXPR,


UNLT_EXPR,
UNLE_EXPR,
UNGT_EXPR,
UNGE_EXPR,
UNEQ_EXPR,


IN_EXPR,
SET_LE_EXPR,
CARD_EXPR,
RANGE_EXPR,




CONVERT_EXPR,


NOP_EXPR,


NON_LVALUE_EXPR,
# 707 "./tree.def"
VIEW_CONVERT_EXPR,





SAVE_EXPR,
# 722 "./tree.def"
UNSAVE_EXPR,
# 731 "./tree.def"
RTL_EXPR,



ADDR_EXPR,


REFERENCE_EXPR,



ENTRY_VALUE_EXPR,



FDESC_EXPR,



COMPLEX_EXPR,


CONJ_EXPR,



REALPART_EXPR,
IMAGPART_EXPR,




PREDECREMENT_EXPR,
PREINCREMENT_EXPR,
POSTDECREMENT_EXPR,
POSTINCREMENT_EXPR,


VA_ARG_EXPR,






TRY_CATCH_EXPR,
# 790 "./tree.def"
TRY_FINALLY_EXPR,






GOTO_SUBROUTINE_EXPR,







LABEL_EXPR,



GOTO_EXPR,






RETURN_EXPR,



EXIT_EXPR,




LOOP_EXPR,




LABELED_BLOCK_EXPR,




EXIT_BLOCK_EXPR,
# 844 "./tree.def"
EXPR_WITH_FILE_LOCATION,





SWITCH_EXPR,


EXC_PTR_EXPR,
# 31 "./tree.h" 2

  LAST_AND_UNUSED_TREE_CODE

};
# 49 "./tree.h"
extern char tree_code_type[256];
# 60 "./tree.h"
extern int tree_code_length[256];




extern const char *tree_code_name[256];



enum built_in_class
{
  NOT_BUILT_IN = 0,
  BUILT_IN_FRONTEND,
  BUILT_IN_MD,
  BUILT_IN_NORMAL
};


extern const char *const built_in_class_names[4];





enum built_in_function
{

# 1 "./builtins.def" 1
# 145 "./builtins.def"
BUILT_IN_ALLOCA,




BUILT_IN_ABS,


BUILT_IN_LABS,



BUILT_IN_FABS,


BUILT_IN_FABSF,


BUILT_IN_FABSL,



BUILT_IN_LLABS,


BUILT_IN_IMAXABS,


BUILT_IN_CONJ,


BUILT_IN_CONJF,


BUILT_IN_CONJL,


BUILT_IN_CREAL,


BUILT_IN_CREALF,


BUILT_IN_CREALL,


BUILT_IN_CIMAG,


BUILT_IN_CIMAGF,


BUILT_IN_CIMAGL,
# 214 "./builtins.def"
BUILT_IN_BZERO,





BUILT_IN_BCMP,






BUILT_IN_FFS,


BUILT_IN_INDEX,


BUILT_IN_RINDEX,



BUILT_IN_MEMCPY,


BUILT_IN_MEMCMP,


BUILT_IN_MEMSET,



BUILT_IN_STRCAT,


BUILT_IN_STRNCAT,


BUILT_IN_STRCPY,


BUILT_IN_STRNCPY,


BUILT_IN_STRCMP,


BUILT_IN_STRNCMP,


BUILT_IN_STRLEN,


BUILT_IN_STRSTR,


BUILT_IN_STRPBRK,


BUILT_IN_STRSPN,


BUILT_IN_STRCSPN,


BUILT_IN_STRCHR,


BUILT_IN_STRRCHR,



BUILT_IN_SQRT,


BUILT_IN_SIN,


BUILT_IN_COS,


BUILT_IN_SQRTF,


BUILT_IN_SINF,


BUILT_IN_COSF,


BUILT_IN_SQRTL,


BUILT_IN_SINL,


BUILT_IN_COSL,
# 321 "./builtins.def"
BUILT_IN_SAVEREGS,


BUILT_IN_CLASSIFY_TYPE,


BUILT_IN_NEXT_ARG,


BUILT_IN_ARGS_INFO,


BUILT_IN_CONSTANT_P,


BUILT_IN_FRAME_ADDRESS,


BUILT_IN_RETURN_ADDRESS,


BUILT_IN_AGGREGATE_INCOMING_ADDRESS,


BUILT_IN_APPLY_ARGS,


BUILT_IN_APPLY,


BUILT_IN_RETURN,


BUILT_IN_SETJMP,


BUILT_IN_LONGJMP,


BUILT_IN_TRAP,


BUILT_IN_PREFETCH,




BUILT_IN_PUTCHAR,


BUILT_IN_PUTS,


BUILT_IN_PRINTF,


BUILT_IN_FPUTC,






BUILT_IN_FPUTS,





BUILT_IN_FWRITE,


BUILT_IN_FPRINTF,





BUILT_IN_PUTCHAR_UNLOCKED,


BUILT_IN_PUTS_UNLOCKED,


BUILT_IN_PRINTF_UNLOCKED,


BUILT_IN_FPUTC_UNLOCKED,






BUILT_IN_FPUTS_UNLOCKED,





BUILT_IN_FWRITE_UNLOCKED,


BUILT_IN_FPRINTF_UNLOCKED,




BUILT_IN_ISGREATER,


BUILT_IN_ISGREATEREQUAL,


BUILT_IN_ISLESS,


BUILT_IN_ISLESSEQUAL,


BUILT_IN_ISLESSGREATER,


BUILT_IN_ISUNORDERED,




BUILT_IN_UNWIND_INIT,


BUILT_IN_DWARF_CFA,


BUILT_IN_DWARF_FP_REGNUM,


BUILT_IN_INIT_DWARF_REG_SIZES,


BUILT_IN_FROB_RETURN_ADDR,


BUILT_IN_EXTRACT_RETURN_ADDR,


BUILT_IN_EH_RETURN,


BUILT_IN_EH_RETURN_DATA_REGNO,



BUILT_IN_VARARGS_START,


BUILT_IN_STDARG_START,


BUILT_IN_VA_END,


BUILT_IN_VA_COPY,


BUILT_IN_EXPECT,
# 87 "./tree.h" 2


  END_BUILTINS
};



extern const char *const built_in_names[(int) END_BUILTINS];


extern tree built_in_decls[(int) END_BUILTINS];
# 121 "./tree.h"
struct tree_common
{
  tree chain;
  tree type;

  unsigned int code : 8;

  unsigned side_effects_flag : 1;
  unsigned constant_flag : 1;
  unsigned addressable_flag : 1;
  unsigned volatile_flag : 1;
  unsigned readonly_flag : 1;
  unsigned unsigned_flag : 1;
  unsigned asm_written_flag: 1;
  unsigned unused_0 : 1;

  unsigned used_flag : 1;
  unsigned nothrow_flag : 1;
  unsigned static_flag : 1;
  unsigned public_flag : 1;
  unsigned private_flag : 1;
  unsigned protected_flag : 1;
  unsigned bounded_flag : 1;
  unsigned deprecated_flag : 1;

  unsigned lang_flag_0 : 1;
  unsigned lang_flag_1 : 1;
  unsigned lang_flag_2 : 1;
  unsigned lang_flag_3 : 1;
  unsigned lang_flag_4 : 1;
  unsigned lang_flag_5 : 1;
  unsigned lang_flag_6 : 1;
  unsigned unused_1 : 1;
};
# 332 "./tree.h"
# 1 "./tree-check.h" 1
# 333 "./tree.h" 2
# 705 "./tree.h"
struct tree_int_cst
{
  struct tree_common common;
  rtx rtl;




  struct {
    unsigned long low;
    long high;
  } int_cst;
};
# 732 "./tree.h"
# 1 "./real.h" 1
# 107 "./real.h"
typedef struct {
  long r[(19 + sizeof (long))/(sizeof (long))];
} realvaluetype;
# 131 "./real.h"
extern unsigned int significand_size (enum machine_mode);
# 144 "./real.h"
extern void earith (realvaluetype *, int, realvaluetype *, realvaluetype *);

extern realvaluetype etrunci (realvaluetype);
extern realvaluetype etruncui (realvaluetype);
extern realvaluetype ereal_negate (realvaluetype);
extern long efixi (realvaluetype);
extern unsigned long efixui (realvaluetype);
extern void ereal_from_int (realvaluetype *, long, long, enum machine_mode);


extern void ereal_from_uint (realvaluetype *, unsigned long, unsigned long, enum machine_mode);



extern void ereal_to_int (long *, long *, realvaluetype);

extern realvaluetype ereal_ldexp (realvaluetype, int);

extern void etartdouble (realvaluetype, long *);
extern void etarldouble (realvaluetype, long *);
extern void etardouble (realvaluetype, long *);
extern long etarsingle (realvaluetype);
extern void ereal_to_decimal (realvaluetype, char *);
extern int ereal_cmp (realvaluetype, realvaluetype);
extern int ereal_isneg (realvaluetype);
extern realvaluetype ereal_unto_float (long);
extern realvaluetype ereal_unto_double (long *);
extern realvaluetype ereal_from_float (long);
extern realvaluetype ereal_from_double (long *);
# 406 "./real.h"
extern realvaluetype real_value_truncate (enum machine_mode, realvaluetype);
# 437 "./real.h"
extern realvaluetype dconst0;
extern realvaluetype dconst1;
extern realvaluetype dconst2;
extern realvaluetype dconstm1;




union real_extract
{
  realvaluetype d;
  long i[sizeof (realvaluetype) / sizeof (long)];
};




union tree_node;
realvaluetype real_value_from_int_cst (union tree_node *, union tree_node *);
# 466 "./real.h"
extern struct rtx_def *immed_real_const_1 (realvaluetype, enum machine_mode);
# 478 "./real.h"
extern int exact_real_inverse (enum machine_mode, realvaluetype *);
extern int target_isnan (realvaluetype);
extern int target_isinf (realvaluetype);
extern int target_negative (realvaluetype);
extern void debug_real (realvaluetype);
extern realvaluetype ereal_atof (const char *, enum machine_mode);
# 733 "./tree.h" 2

struct tree_real_cst
{
  struct tree_common common;
  rtx rtl;
  realvaluetype real_cst;
};





struct tree_string
{
  struct tree_common common;
  rtx rtl;
  int length;
  const char *pointer;
};





struct tree_complex
{
  struct tree_common common;
  rtx rtl;
  tree real;
  tree imag;
};




struct tree_vector
{
  struct tree_common common;
  rtx rtl;
  tree elements;
};


# 1 "./hashtable.h" 1
# 21 "./hashtable.h"
# 1 "./obstack.h" 1
# 161 "./obstack.h"
struct _obstack_chunk
{
  char *limit;
  struct _obstack_chunk *prev;
  char contents[4];
};

struct obstack
{
  long chunk_size;
  struct _obstack_chunk *chunk;
  char *object_base;
  char *next_free;
  char *chunk_limit;
  long int temp;
  int alignment_mask;




  struct _obstack_chunk *(*chunkfun) (void *, long);
  void (*freefun) (void *, struct _obstack_chunk *);
  void *extra_arg;





  unsigned use_extra_arg:1;
  unsigned maybe_empty_object:1;



  unsigned alloc_failed:1;


};




extern void _obstack_newchunk (struct obstack *, int);
extern void _obstack_free (struct obstack *, void *);
extern int _obstack_begin (struct obstack *, int, int,
       void *(*) (long), void (*) (void *));
extern int _obstack_begin_1 (struct obstack *, int, int,
        void *(*) (void *, long),
        void (*) (void *, void *), void *);
extern int _obstack_memory_used (struct obstack *);
# 223 "./obstack.h"
void obstack_init (struct obstack *obstack);

void * obstack_alloc (struct obstack *obstack, int size);

void * obstack_copy (struct obstack *obstack, void *address, int size);
void * obstack_copy0 (struct obstack *obstack, void *address, int size);

void obstack_free (struct obstack *obstack, void *block);

void obstack_blank (struct obstack *obstack, int size);

void obstack_grow (struct obstack *obstack, void *data, int size);
void obstack_grow0 (struct obstack *obstack, void *data, int size);

void obstack_1grow (struct obstack *obstack, int data_char);
void obstack_ptr_grow (struct obstack *obstack, void *data);
void obstack_int_grow (struct obstack *obstack, int data);

void * obstack_finish (struct obstack *obstack);

int obstack_object_size (struct obstack *obstack);

int obstack_room (struct obstack *obstack);
void obstack_make_room (struct obstack *obstack, int size);
void obstack_1grow_fast (struct obstack *obstack, int data_char);
void obstack_ptr_grow_fast (struct obstack *obstack, void *data);
void obstack_int_grow_fast (struct obstack *obstack, int data);
void obstack_blank_fast (struct obstack *obstack, int size);

void * obstack_base (struct obstack *obstack);
void * obstack_next_free (struct obstack *obstack);
int obstack_alignment_mask (struct obstack *obstack);
int obstack_chunk_size (struct obstack *obstack);
int obstack_memory_used (struct obstack *obstack);
# 267 "./obstack.h"
extern void (*obstack_alloc_failed_handler) (void);





extern int obstack_exit_failure;
# 22 "./hashtable.h" 2



typedef struct ht_identifier ht_identifier;
struct ht_identifier
{
  unsigned int len;
  const unsigned char *str;
};
# 39 "./hashtable.h"
struct cpp_reader;
typedef struct ht hash_table;
typedef struct ht_identifier *hashnode;

enum ht_lookup_option {HT_NO_INSERT = 0, HT_ALLOC, HT_ALLOCED};


struct ht
{

  struct obstack stack;

  hashnode *entries;

  hashnode (*alloc_node) (hash_table *);

  unsigned int nslots;
  unsigned int nelements;


  struct cpp_reader *pfile;


  unsigned int searches;
  unsigned int collisions;
};

extern void gcc_obstack_init (struct obstack *);


extern hash_table *ht_create (unsigned int order);


extern void ht_destroy (hash_table *);

extern hashnode ht_lookup (hash_table *, const unsigned char *, unsigned int, enum ht_lookup_option);





typedef int (*ht_cb) (struct cpp_reader *, hashnode, const void *);
extern void ht_forall (hash_table *, ht_cb, const void *);


extern void ht_dump_statistics (hash_table *);



extern double approx_sqrt (double);
# 776 "./tree.h" 2
# 791 "./tree.h"
struct tree_identifier
{
  struct tree_common common;
  struct ht_identifier id;
};





struct tree_list
{
  struct tree_common common;
  tree purpose;
  tree value;
};







struct tree_vec
{
  struct tree_common common;
  int length;
  tree a[1];
};
# 879 "./tree.h"
struct tree_exp
{
  struct tree_common common;
  int complexity;
  tree operands[1];
};
# 931 "./tree.h"
struct tree_block
{
  struct tree_common common;

  unsigned handler_block_flag : 1;
  unsigned abstract_flag : 1;
  unsigned block_num : 30;

  tree vars;
  tree subblocks;
  tree supercontext;
  tree abstract_origin;
  tree fragment_origin;
  tree fragment_chain;
};
# 1208 "./tree.h"
struct tree_type
{
  struct tree_common common;
  tree values;
  tree size;
  tree size_unit;
  tree attributes;
  unsigned int uid;

  unsigned int precision : 9;
  unsigned int mode : 7;

  unsigned string_flag : 1;
  unsigned no_force_blk_flag : 1;
  unsigned needs_constructing_flag : 1;
  unsigned transparent_union_flag : 1;
  unsigned packed_flag : 1;
  unsigned restrict_flag : 1;
  unsigned pointer_depth : 2;

  unsigned lang_flag_0 : 1;
  unsigned lang_flag_1 : 1;
  unsigned lang_flag_2 : 1;
  unsigned lang_flag_3 : 1;
  unsigned lang_flag_4 : 1;
  unsigned lang_flag_5 : 1;
  unsigned lang_flag_6 : 1;
  unsigned user_align : 1;

  unsigned int align;
  tree pointer_to;
  tree reference_to;
  union {int address; char *pointer; } symtab;
  tree name;
  tree minval;
  tree maxval;
  tree next_variant;
  tree main_variant;
  tree binfo;
  tree context;
  long alias_set;

  struct lang_type *lang_specific;
};
# 1754 "./tree.h"
struct function;

struct tree_decl
{
  struct tree_common common;
  const char *filename;
  int linenum;
  unsigned int uid;
  tree size;
  unsigned int mode : 8;

  unsigned external_flag : 1;
  unsigned nonlocal_flag : 1;
  unsigned regdecl_flag : 1;
  unsigned inline_flag : 1;
  unsigned bit_field_flag : 1;
  unsigned virtual_flag : 1;
  unsigned ignored_flag : 1;
  unsigned abstract_flag : 1;

  unsigned in_system_header_flag : 1;
  unsigned common_flag : 1;
  unsigned defer_output : 1;
  unsigned transparent_union : 1;
  unsigned static_ctor_flag : 1;
  unsigned static_dtor_flag : 1;
  unsigned artificial_flag : 1;
  unsigned weak_flag : 1;

  unsigned non_addr_const_p : 1;
  unsigned no_instrument_function_entry_exit : 1;
  unsigned comdat_flag : 1;
  unsigned malloc_flag : 1;
  unsigned no_limit_stack : 1;
  unsigned int built_in_class : 2;
  unsigned pure_flag : 1;

  unsigned pointer_depth : 2;
  unsigned non_addressable : 1;
  unsigned user_align : 1;
  unsigned uninlinable : 1;


  unsigned lang_flag_0 : 1;
  unsigned lang_flag_1 : 1;
  unsigned lang_flag_2 : 1;
  unsigned lang_flag_3 : 1;
  unsigned lang_flag_4 : 1;
  unsigned lang_flag_5 : 1;
  unsigned lang_flag_6 : 1;
  unsigned lang_flag_7 : 1;

  union {


    enum built_in_function f;


    long i;


    struct {unsigned int align : 24; unsigned int off_align : 8;} a;
  } u1;

  tree size_unit;
  tree name;
  tree context;
  tree arguments;
  tree result;
  tree initial;
  tree abstract_origin;
  tree assembler_name;
  tree section_name;
  tree attributes;
  rtx rtl;
  rtx live_range_rtl;






  union {
    struct function *f;
    rtx r;
    tree t;
    int i;
  } u2;


  tree saved_tree;



  tree inlined_fns;

  tree vindex;
  long pointer_alias_set;

  struct lang_decl *lang_specific;
};





union tree_node
{
  struct tree_common common;
  struct tree_int_cst int_cst;
  struct tree_real_cst real_cst;
  struct tree_vector vector;
  struct tree_string string;
  struct tree_complex complex;
  struct tree_identifier identifier;
  struct tree_decl decl;
  struct tree_type type;
  struct tree_list list;
  struct tree_vec vec;
  struct tree_exp exp;
  struct tree_block block;
 };



enum tree_index
{
  TI_ERROR_MARK,
  TI_INTQI_TYPE,
  TI_INTHI_TYPE,
  TI_INTSI_TYPE,
  TI_INTDI_TYPE,
  TI_INTTI_TYPE,

  TI_UINTQI_TYPE,
  TI_UINTHI_TYPE,
  TI_UINTSI_TYPE,
  TI_UINTDI_TYPE,
  TI_UINTTI_TYPE,

  TI_INTEGER_ZERO,
  TI_INTEGER_ONE,
  TI_INTEGER_MINUS_ONE,
  TI_NULL_POINTER,

  TI_SIZE_ZERO,
  TI_SIZE_ONE,

  TI_BITSIZE_ZERO,
  TI_BITSIZE_ONE,
  TI_BITSIZE_UNIT,

  TI_COMPLEX_INTEGER_TYPE,
  TI_COMPLEX_FLOAT_TYPE,
  TI_COMPLEX_DOUBLE_TYPE,
  TI_COMPLEX_LONG_DOUBLE_TYPE,

  TI_FLOAT_TYPE,
  TI_DOUBLE_TYPE,
  TI_LONG_DOUBLE_TYPE,

  TI_VOID_TYPE,
  TI_PTR_TYPE,
  TI_CONST_PTR_TYPE,
  TI_PTRDIFF_TYPE,
  TI_VA_LIST_TYPE,

  TI_VOID_LIST_NODE,

  TI_UV4SF_TYPE,
  TI_UV4SI_TYPE,
  TI_UV8HI_TYPE,
  TI_UV8QI_TYPE,
  TI_UV4HI_TYPE,
  TI_UV2SI_TYPE,
  TI_UV2SF_TYPE,
  TI_UV16QI_TYPE,

  TI_V4SF_TYPE,
  TI_V16SF_TYPE,
  TI_V4SI_TYPE,
  TI_V8HI_TYPE,
  TI_V8QI_TYPE,
  TI_V4HI_TYPE,
  TI_V2SI_TYPE,
  TI_V2SF_TYPE,
  TI_V16QI_TYPE,

  TI_MAIN_IDENTIFIER,

  TI_MAX
};

extern tree global_trees[TI_MAX];
# 2020 "./tree.h"
enum integer_type_kind
{
  itk_char,
  itk_signed_char,
  itk_unsigned_char,
  itk_short,
  itk_unsigned_short,
  itk_int,
  itk_unsigned_int,
  itk_long,
  itk_unsigned_long,
  itk_long_long,
  itk_unsigned_long_long,
  itk_none
};

typedef enum integer_type_kind integer_type_kind;



extern tree integer_types[itk_none];
# 2059 "./tree.h"
extern double approx_sqrt (double);

extern char *permalloc (int);
extern char *expralloc (int);




extern size_t tree_size (tree);





extern tree make_node (enum tree_code);
extern tree make_lang_type (enum tree_code);
extern tree (*make_lang_type_fn) (enum tree_code);





extern tree copy_node (tree);



extern tree copy_list (tree);



extern tree make_tree_vec (int);




extern tree get_identifier (const char *);




extern tree get_identifier_with_length (const char *, unsigned int);





extern tree maybe_get_identifier (const char *);






extern tree build (enum tree_code, tree, ...);
extern tree build_nt (enum tree_code, ...);

extern tree build_int_2_wide (unsigned long, long);
extern tree build_vector (tree, tree);
extern tree build_real (tree, realvaluetype);
extern tree build_real_from_int_cst (tree, tree);
extern tree build_complex (tree, tree, tree);
extern tree build_string (int, const char *);
extern tree build1 (enum tree_code, tree, tree);
extern tree build_tree_list (tree, tree);
extern tree build_decl (enum tree_code, tree, tree);
extern tree build_block (tree, tree, tree, tree, tree);
extern tree build_expr_wfl (tree, const char *, int, int);



extern tree make_signed_type (int);
extern tree make_unsigned_type (int);
extern void initialize_sizetypes (void);
extern void set_sizetype (tree);
extern tree signed_or_unsigned_type (int, tree);
extern void fixup_unsigned_type (tree);
extern tree build_pointer_type (tree);
extern tree build_reference_type (tree);
extern tree build_type_no_quals (tree);
extern tree build_index_type (tree);
extern tree build_index_2_type (tree, tree);
extern tree build_array_type (tree, tree);
extern tree build_function_type (tree, tree);
extern tree build_method_type (tree, tree);
extern tree build_offset_type (tree, tree);
extern tree build_complex_type (tree);
extern tree array_type_nelts (tree);

extern tree value_member (tree, tree);
extern tree purpose_member (tree, tree);
extern tree binfo_member (tree, tree);
extern unsigned int attribute_hash_list (tree);
extern int attribute_list_equal (tree, tree);
extern int attribute_list_contained (tree, tree);
extern int tree_int_cst_equal (tree, tree);
extern int tree_int_cst_lt (tree, tree);
extern int tree_int_cst_compare (tree, tree);
extern int host_integerp (tree, int);
extern long tree_low_cst (tree, int);
extern int tree_int_cst_msb (tree);
extern int tree_int_cst_sgn (tree);
extern int tree_expr_nonnegative_p (tree);
extern int rtl_expr_nonnegative_p (rtx);
extern int index_type_equal (tree, tree);
extern tree get_inner_array_type (tree);





extern tree make_tree (tree, rtx);







extern tree build_type_attribute_variant (tree, tree);
extern tree build_decl_attribute_variant (tree, tree);


struct attribute_spec
{


  const char *const name;

  const int min_length;


  const int max_length;







  const _Bool decl_required;


  const _Bool type_required;




  const _Bool function_type_required;
# 2221 "./tree.h"
  tree (*const handler) (tree *node, tree name, tree args, int flags, _Bool *no_add_attrs);

};

extern const struct attribute_spec default_target_attribute_table[];



enum attribute_flags
{



  ATTR_FLAG_DECL_NEXT = 1,



  ATTR_FLAG_FUNCTION_NEXT = 2,



  ATTR_FLAG_ARRAY_NEXT = 4,


  ATTR_FLAG_TYPE_IN_PLACE = 8,



  ATTR_FLAG_BUILT_IN = 16
};



extern tree merge_decl_attributes (tree, tree);
extern tree merge_type_attributes (tree, tree);
extern int default_comp_type_attributes (tree, tree);
extern void default_set_default_type_attributes (tree);
extern void default_insert_attributes (tree, tree *);
extern _Bool default_function_attribute_inlinable_p (tree);
extern _Bool default_ms_bitfield_layout_p (tree);



extern void split_specs_attrs (tree, tree *, tree *);



extern tree strip_attrs (tree);



extern int valid_machine_attribute (tree, tree, tree, tree);




extern int is_attribute_p (const char *, tree);




extern tree lookup_attribute (const char *, tree);



extern tree merge_attributes (tree, tree);
# 2298 "./tree.h"
extern tree get_qualified_type (tree, int);




extern tree build_qualified_type (tree, int);
# 2317 "./tree.h"
extern tree build_type_copy (tree);





extern void layout_type (tree);
# 2332 "./tree.h"
typedef struct record_layout_info_s
{

  tree t;


  tree offset;

  unsigned int offset_align;

  tree bitpos;

  unsigned int record_align;

  unsigned int unpacked_align;


  unsigned int unpadded_align;

  tree prev_field;


  tree pending_statics;
  int packed_maybe_necessary;
} *record_layout_info;

extern void set_lang_adjust_rli (void (*) (record_layout_info));

extern record_layout_info start_record_layout (tree);
extern tree bit_from_pos (tree, tree);
extern tree byte_from_pos (tree, tree);
extern void pos_from_byte (tree *, tree *, unsigned int, tree);

extern void pos_from_bit (tree *, tree *, unsigned int, tree);

extern void normalize_offset (tree *, tree *, unsigned int);

extern tree rli_size_unit_so_far (record_layout_info);
extern tree rli_size_so_far (record_layout_info);
extern void normalize_rli (record_layout_info);
extern void place_field (record_layout_info, tree);
extern void compute_record_mode (tree);
extern void finish_record_layout (record_layout_info);






extern tree type_hash_canon (unsigned int, tree);
# 2391 "./tree.h"
extern void layout_decl (tree, unsigned);






extern enum machine_mode mode_for_size_tree (tree, enum mode_class, int);




extern tree non_lvalue (tree);
extern tree pedantic_non_lvalue (tree);

extern tree convert (tree, tree);
extern unsigned int expr_align (tree);
extern tree size_in_bytes (tree);
extern long int_size_in_bytes (tree);
extern tree bit_position (tree);
extern long int_bit_position (tree);
extern tree byte_position (tree);
extern long int_byte_position (tree);




enum size_type_kind
{
  SIZETYPE,
  SSIZETYPE,
  USIZETYPE,
  BITSIZETYPE,
  SBITSIZETYPE,
  UBITSIZETYPE,
  TYPE_KIND_LAST};

extern tree sizetype_tab[(int) TYPE_KIND_LAST];
# 2437 "./tree.h"
extern tree size_binop (enum tree_code, tree, tree);
extern tree size_diffop (tree, tree);
extern tree size_int_wide (long, enum size_type_kind);

extern tree size_int_type_wide (long, tree);







extern tree round_up (tree, int);
extern tree round_down (tree, int);
extern tree get_pending_sizes (void);
extern int is_pending_size (tree);
extern void put_pending_size (tree);
extern void put_pending_sizes (tree);
# 2464 "./tree.h"
extern unsigned int maximum_field_alignment;


extern unsigned int set_alignment;





extern tree chainon (tree, tree);



extern tree tree_cons (tree, tree, tree);



extern tree tree_last (tree);



extern tree nreverse (tree);




extern int list_length (tree);



extern int fields_length (tree);



extern int integer_zerop (tree);



extern int integer_onep (tree);




extern int integer_all_onesp (tree);




extern int integer_pow2p (tree);




extern int staticp (tree);




extern int lvalue_or_else (tree, const char *);





extern tree save_expr (tree);




extern int first_rtl_op (enum tree_code);





extern tree unsave_expr (tree);




extern void unsave_expr_1 (tree);



extern tree unsave_expr_now (tree);






extern void (*lang_unsave) (tree *);
extern void (*lang_unsave_expr_now) (tree);




extern int unsafe_for_reeval (tree);



extern int (*lang_unsafe_for_reeval) (tree);







extern int contains_placeholder_p (tree);




extern int has_cleanups (tree);






extern tree substitute_in_expr (tree, tree, tree);







extern tree variable_size (tree);





extern tree stabilize_reference (tree);





extern tree stabilize_reference_1 (tree);






extern tree get_unwidened (tree, tree);






extern tree get_narrower (tree, int *);






extern tree type_for_mode (enum machine_mode, int);






extern tree type_for_size (unsigned, int);






extern tree unsigned_type (tree);






extern tree signed_type (tree);





extern tree maybe_build_cleanup (tree);





extern tree get_inner_reference (tree, long *, long *, tree *, enum machine_mode *, int *, int *);






extern int handled_component_p (tree);




extern tree get_containing_scope (tree);



extern tree decl_function_context (tree);



extern tree decl_type_context (tree);






extern const char *function_cannot_inline_p (tree);


extern int real_zerop (tree);





extern const char *input_filename;


extern int lineno;




extern int pedantic_lvalues;




extern int immediate_size_expand;



extern tree current_function_decl;


extern tree current_function_func_begin_label;



extern int all_types_permanent;
# 2729 "./tree.h"
extern const char *(*decl_printable_name) (tree, int);




extern void (*incomplete_decl_finalize_hook) (tree);



extern tree builtin_function (const char *, tree, int, enum built_in_class, const char *);




extern char *perm_calloc (int, long);
extern void clean_symbol_name (char *);
extern tree get_file_function_name_long (const char *);
extern tree get_set_constructor_bits (tree, char *, int);
extern tree get_set_constructor_bytes (tree, unsigned char *, int);

extern tree get_callee_fndecl (tree);
extern void set_decl_assembler_name (tree);
extern int type_num_arguments (tree);



extern int in_control_zone_p (void);
extern void expand_fixups (rtx);
extern tree expand_start_stmt_expr (int);
extern tree expand_end_stmt_expr (tree);
extern void expand_expr_stmt (tree);
extern void expand_expr_stmt_value (tree, int, int);
extern int warn_if_unused_value (tree);
extern void expand_decl_init (tree);
extern void clear_last_expr (void);
extern void expand_label (tree);
extern void expand_goto (tree);
extern void expand_asm (tree);
extern void expand_start_cond (tree, int);
extern void expand_end_cond (void);
extern void expand_start_else (void);
extern void expand_start_elseif (tree);
extern struct nesting *expand_start_loop (int);
extern struct nesting *expand_start_loop_continue_elsewhere (int);
extern struct nesting *expand_start_null_loop (void);
extern void expand_loop_continue_here (void);
extern void expand_end_loop (void);
extern void expand_end_null_loop (void);
extern int expand_continue_loop (struct nesting *);
extern int expand_exit_loop (struct nesting *);
extern int expand_exit_loop_if_false (struct nesting *, tree);

extern int expand_exit_loop_top_cond (struct nesting *, tree);

extern int expand_exit_something (void);

extern void expand_return (tree);
extern int optimize_tail_recursion (tree, rtx);
extern void expand_start_bindings_and_block (int, tree);


extern void expand_end_bindings (tree, int, int);
extern void warn_about_unused_variables (tree);
extern void start_cleanup_deferral (void);
extern void end_cleanup_deferral (void);
extern int is_body_block (tree);

extern int conditional_context (void);
extern struct nesting * current_nesting_level (void);
extern tree last_cleanup_this_contour (void);
extern void expand_start_case (int, tree, tree, const char *);

extern void expand_end_case_type (tree, tree);

extern int add_case_node (tree, tree, tree, tree *);

extern int pushcase (tree, tree (*) (tree, tree), tree, tree *);


extern int pushcase_range (tree, tree, tree (*) (tree, tree), tree, tree *);


extern void using_eh_for_cleanups (void);
extern int stmt_loop_nest_empty (void);
# 2822 "./tree.h"
extern tree fold (tree);

extern int force_fit_type (tree, int);
extern int add_double (unsigned long, long, unsigned long, long, unsigned long *, long *);



extern int neg_double (unsigned long, long, unsigned long *, long *);


extern int mul_double (unsigned long, long, unsigned long, long, unsigned long *, long *);




extern void lshift_double (unsigned long, long, long, unsigned int, unsigned long *, long *, int);



extern void rshift_double (unsigned long, long, long, unsigned int, unsigned long *, long *, int);



extern void lrotate_double (unsigned long, long, long, unsigned int, unsigned long *, long *);



extern void rrotate_double (unsigned long, long, long, unsigned int, unsigned long *, long *);



extern int operand_equal_p (tree, tree, int);
extern tree invert_truthvalue (tree);





extern tree (*lang_type_promotes_to) (tree);
extern tree fold_builtin (tree);




extern void copy_lang_decl (tree);


extern int yyparse (void);



extern void pushlevel (int);





extern tree poplevel (int, int, int);

extern void set_block (tree);




extern tree pushdecl (tree);

extern tree getdecls (void);

extern tree gettags (void);

extern tree build_range_type (tree, tree, tree);


extern void record_component_aliases (tree);
extern long get_alias_set (tree);
extern int alias_sets_conflict_p (long, long);

extern int readonly_fields_p (tree);
extern int objects_must_conflict_p (tree, tree);






extern void (*lang_set_decl_assembler_name) (tree);

struct obstack;


extern int really_constant_p (tree);
extern int int_fits_type_p (tree, tree);
extern int tree_log2 (tree);
extern int tree_floor_log2 (tree);
extern void preserve_data (void);
extern int object_permanent_p (tree);
extern int type_precision (tree);
extern int simple_cst_equal (tree, tree);
extern int compare_tree_int (tree, unsigned long);

extern int type_list_equal (tree, tree);
extern int chain_member (tree, tree);
extern int chain_member_purpose (tree, tree);
extern int chain_member_value (tree, tree);
extern tree listify (tree);
extern tree type_hash_lookup (unsigned int, tree);
extern void type_hash_add (unsigned int, tree);
extern unsigned int type_hash_list (tree);
extern int simple_cst_list_equal (tree, tree);
extern void dump_tree_statistics (void);
extern void print_obstack_statistics (const char *, struct obstack *);


extern void print_obstack_name (char *, FILE *, const char *);


extern void expand_function_end (const char *, int, int);
extern void expand_function_start (tree, int);
extern void expand_pending_sizes (tree);

extern int real_onep (tree);
extern int real_twop (tree);
extern void gcc_obstack_init (struct obstack *);
extern void init_obstacks (void);
extern void build_common_tree_nodes (int);
extern void build_common_tree_nodes_2 (int);
extern void mark_tree_hashtable (void *);


extern void setjmp_protect_args (void);
extern void setjmp_protect (tree);
extern void expand_main_function (void);
extern void mark_varargs (void);
extern void init_dummy_function_start (void);
extern void expand_dummy_function_end (void);
extern void init_function_for_compilation (void);
extern void init_function_start (tree, const char *, int);
extern void assign_parms (tree);
extern void put_var_into_stack (tree);
extern void flush_addressof (tree);
extern void uninitialized_vars_warning (tree);
extern void setjmp_args_warning (void);
extern void mark_all_temps_used (void);
extern void init_temp_slots (void);
extern void combine_temp_slots (void);
extern void free_temp_slots (void);
extern void pop_temp_slots (void);
extern void push_temp_slots (void);
extern void preserve_temp_slots (rtx);
extern void preserve_rtl_expr_temps (tree);
extern int aggregate_value_p (tree);
extern void free_temps_for_rtl_expr (tree);
extern void instantiate_virtual_regs (tree, rtx);
extern void unshare_all_rtl (tree, rtx);
extern int max_parm_reg_num (void);
extern void push_function_context (void);
extern void pop_function_context (void);
extern void push_function_context_to (tree);
extern void pop_function_context_from (tree);
extern void ggc_mark_struct_function (struct function *);



extern void print_rtl (FILE *, rtx);



extern void debug_tree (tree);

extern void print_node (FILE *, const char *, tree, int);

extern void print_node_brief (FILE *, const char *, tree, int);

extern void indent_to (FILE *, int);



extern int apply_args_register_offset (int);
extern rtx expand_builtin_return_addr
 (enum built_in_function, int, rtx);
extern void check_max_integer_computation_mode (tree);


extern void start_sequence_for_rtl_expr (tree);
extern rtx emit_line_note (const char *, int);



extern int setjmp_call_p (tree);
# 3022 "./tree.h"
extern tree decl_attributes (tree *, tree, int);






extern void insert_default_attributes (tree);


extern const struct attribute_spec *format_attribute_table;


extern const struct attribute_spec *lang_attribute_table;


extern int lang_attribute_common;



extern int mark_addressable (tree);
extern void incomplete_type_error (tree, tree);
extern tree truthvalue_conversion (tree);
extern int global_bindings_p (void);
extern void insert_block (tree);


extern void save_for_inline (tree);
extern void set_decl_abstract_flags (tree, int);
extern void output_inline_function (tree);
extern void set_decl_origin_self (tree);


extern void fixup_signed_type (tree);
extern void internal_reference_types (void);


extern void make_decl_rtl (tree, const char *);
extern void make_decl_one_only (tree);
extern int supports_one_only (void);
extern void variable_section (tree, int);


extern int div_and_round_double (enum tree_code, int, unsigned long, long, unsigned long, long, unsigned long *, long *, unsigned long *, long *);
# 3076 "./tree.h"
extern void emit_nop (void);
extern void expand_computed_goto (tree);
extern _Bool parse_output_constraint (const char **, int, int, int, _Bool *, _Bool *, _Bool *);


extern void expand_asm_operands (tree, tree, tree, tree, int, const char *, int);

extern int any_pending_cleanups (int);
extern void init_stmt (void);
extern void init_stmt_for_function (void);
extern int drop_through_at_end_p (void);
extern void expand_start_target_temps (void);
extern void expand_end_target_temps (void);
extern void expand_elseif (tree);
extern void save_stack_pointer (void);
extern void expand_decl (tree);
extern int expand_decl_cleanup (tree, tree);
extern int expand_decl_cleanup_eh (tree, tree, int);
extern void expand_anon_union_decl (tree, tree, tree);
extern void move_cleanups_up (void);
extern void expand_start_case_dummy (void);
extern void expand_end_case_dummy (void);
extern tree case_index_expr_type (void);
extern long all_cases_count (tree, int *);
extern void check_for_full_enumeration_handling (tree);
extern void declare_nonlocal_label (tree);



extern tree get_file_function_name (int);





extern char *dwarf2out_cfi_label (void);



extern void dwarf2out_def_cfa (const char *, unsigned, long);



extern void dwarf2out_window_save (const char *);




extern void dwarf2out_args_size (const char *, long);



extern void dwarf2out_reg_save (const char *, unsigned, long);



extern void dwarf2out_return_save (const char *, long);



extern void dwarf2out_return_reg (const char *, unsigned);



typedef tree (*walk_tree_fn) (tree *, int *, void *);





enum tree_dump_index
{
  TDI_all,
  TDI_class,
  TDI_original,
  TDI_optimized,
  TDI_inlined,

  TDI_end
};







typedef struct dump_info *dump_info_p;

extern int dump_flag (dump_info_p, int, tree);
extern int dump_enabled_p (enum tree_dump_index);
extern FILE *dump_begin (enum tree_dump_index, int *);
extern void dump_end (enum tree_dump_index, FILE *);
extern void dump_node (tree, int, FILE *);
extern int dump_switch_p (const char *);
extern const char *dump_flag_name (enum tree_dump_index);
# 3180 "./tree.h"
extern void fancy_abort (const char *, int, const char *)
    __attribute__ ((__noreturn__));
# 40 "tree.c" 2
# 1 "./tm_p.h" 1
# 1 "./i386-protos.h" 1
# 23 "./i386-protos.h"
extern void override_options (void);
extern void optimization_options (int, int);

extern int ix86_can_use_return_insn_p (void);
extern int ix86_frame_pointer_required (void);
extern void ix86_setup_frame_addresses (void);

extern void ix86_asm_file_end (FILE *);
extern void load_pic_register (void);
extern long ix86_initial_elimination_offset (int, int);
extern void ix86_expand_prologue (void);
extern void ix86_expand_epilogue (int);

extern void ix86_output_addr_vec_elt (FILE *, int);
extern void ix86_output_addr_diff_elt (FILE *, int, int);
# 185 "./i386-protos.h"
extern int ix86_return_pops_args (tree, tree, int);
extern tree ix86_build_va_list (void);

extern int ix86_data_alignment (tree, int);
extern int ix86_local_alignment (tree, int);
extern int ix86_constant_alignment (tree, int);
extern tree ix86_handle_dll_attribute (tree *, tree, tree, int, _Bool *);
extern tree ix86_handle_shared_attribute (tree *, tree, tree, int, _Bool *);

extern unsigned int i386_pe_section_type_flags (tree, const char *, int);

extern void i386_pe_asm_named_section (const char *, unsigned int);
extern void x86_output_mi_thunk (FILE *, int, tree);
extern int x86_field_alignment (tree, int);
# 2 "./tm_p.h" 2
# 1 "./tm-preds.h" 1
# 3 "./tm_p.h" 2
# 41 "tree.c" 2
# 1 "./function.h" 1
# 22 "./function.h"
struct var_refs_queue
{
  rtx modified;
  enum machine_mode promoted_mode;
  int unsignedp;
  struct var_refs_queue *next;
};






struct sequence_stack
{

  rtx first, last;
  tree sequence_rtl_expr;
  struct sequence_stack *next;
};

extern struct sequence_stack *sequence_stack;



struct simple_obstack_stack
{
  struct obstack *obstack;
  struct simple_obstack_stack *next;
};

struct emit_status
{


  int x_reg_rtx_no;


  int x_first_label_num;






  rtx x_first_insn;
  rtx x_last_insn;




  tree sequence_rtl_expr;





  struct sequence_stack *sequence_stack;



  int x_cur_insn_uid;



  int x_last_linenum;
  const char *x_last_filename;





  int regno_pointer_align_length;




  unsigned char *regno_pointer_align;



  tree *regno_decl;



  rtx *x_regno_reg_rtx;
};
# 119 "./function.h"
struct expr_status
{


  int x_pending_stack_adjust;
# 140 "./function.h"
  int x_inhibit_defer_pop;





  int x_stack_pointer_delta;




  rtx x_saveregs_value;


  rtx x_apply_args_value;


  rtx x_forced_labels;


  rtx x_pending_chain;
};
# 174 "./function.h"
struct function
{
  struct eh_status *eh;
  struct stmt_status *stmt;
  struct expr_status *expr;
  struct emit_status *emit;
  struct varasm_status *varasm;




  const char *name;


  tree decl;


  struct function *outer;




  int pops_args;




  int args_size;




  int pretend_args_size;



  int outgoing_args_size;



  rtx arg_offset_rtx;



  CUMULATIVE_ARGS args_info;





  rtx return_rtx;


  rtx internal_arg_pointer;



  const char *cannot_inline;



  struct initial_value_struct *hard_reg_initial_vals;


  int x_function_call_count;




  tree x_nonlocal_labels;





  rtx x_nonlocal_goto_handler_slots;



  rtx x_nonlocal_goto_handler_labels;




  rtx x_nonlocal_goto_stack_level;





  rtx x_cleanup_label;




  rtx x_return_label;



  rtx x_save_expr_regs;



  rtx x_stack_slot_list;


  tree x_rtl_expr_chain;



  rtx x_tail_recursion_label;


  rtx x_tail_recursion_reentry;





  rtx x_arg_pointer_save_area;




  rtx x_clobber_return_insn;




  long x_frame_offset;




  tree x_context_display;
# 317 "./function.h"
  tree x_trampoline_list;


  rtx x_parm_birth_insn;



  rtx x_last_parm_insn;



  unsigned int x_max_parm_reg;





  rtx *x_parm_reg_stack_loc;


  struct temp_slot *x_temp_slots;


  int x_temp_slot_level;


  int x_var_temp_slot_level;





  int x_target_temp_slot_level;



  struct var_refs_queue *fixup_var_refs_queue;


  int inlinable;
  int no_debugging_symbols;

  void *original_arg_vector;
  tree original_decl_initial;


  rtx inl_last_parm_insn;

  int inl_max_label_num;


  int profile_label_no;




  struct machine_function *machine;

  int stack_alignment_needed;

  int preferred_stack_boundary;


  struct language_function *language;





  rtx epilogue_delay_list;





  unsigned int returns_struct : 1;



  unsigned int returns_pcc_struct : 1;


  unsigned int returns_pointer : 1;


  unsigned int needs_context : 1;


  unsigned int calls_setjmp : 1;


  unsigned int calls_longjmp : 1;



  unsigned int calls_alloca : 1;


  unsigned int calls_eh_return : 1;



  unsigned int has_nonlocal_label : 1;



  unsigned int has_nonlocal_goto : 1;


  unsigned int contains_functions : 1;


  unsigned int has_computed_jump : 1;




  unsigned int is_thunk : 1;



  unsigned int instrument_entry_exit : 1;


  unsigned int profile : 1;



  unsigned int limit_stack : 1;



  unsigned int varargs : 1;



  unsigned int stdarg : 1;





  unsigned int x_whole_function_mode_p : 1;
# 468 "./function.h"
  unsigned int x_dont_save_pending_sizes_p : 1;


  unsigned int uses_const_pool : 1;


  unsigned int uses_pic_offset_table : 1;


  unsigned int uses_eh_lsda : 1;


  unsigned int arg_pointer_save_area_init : 1;
};


extern struct function *cfun;


extern int virtuals_instantiated;
# 549 "./function.h"
extern tree inline_function_decl;



struct function *find_function_data (tree);


extern void identify_blocks (void);



extern void reorder_blocks (void);


extern void number_blocks (tree);




extern long get_frame_size (void);

extern long get_func_frame_size (struct function *);



extern void (*init_machine_status) (struct function *);
extern void (*free_machine_status) (struct function *);



extern void (*mark_machine_status) (struct function *);


extern void (*init_lang_status) (struct function *);
extern void (*mark_lang_status) (struct function *);
extern void (*save_lang_status) (struct function *);
extern void (*restore_lang_status) (struct function *);
extern void (*free_lang_status) (struct function *);


extern void restore_emit_status (struct function *);
extern void free_after_parsing (struct function *);
extern void free_after_compilation (struct function *);

extern void init_varasm_status (struct function *);
extern void free_varasm_status (struct function *);
extern void free_emit_status (struct function *);
extern void free_stmt_status (struct function *);
extern void free_eh_status (struct function *);
extern void free_expr_status (struct function *);

extern rtx get_first_block_beg (void);







extern rtx get_arg_pointer_save_area (struct function *);

extern void init_virtual_regs (struct emit_status *);


extern void init_function_once (void);
# 42 "tree.c" 2

# 1 "./toplev.h" 1
# 29 "./toplev.h"
extern int toplev_main (int, char **);
extern int read_integral_parameter (const char *, const char *, const int);

extern int count_error (int);
extern void strip_off_ending (char *, int);
extern void print_time (const char *, long);
extern const char *trim_filename (const char *);
extern void internal_error (const char *, ...)
            __attribute__ ((__noreturn__));
extern void fatal_io_error (const char *, ...)
            __attribute__ ((__noreturn__));
extern void _fatal_insn_not_found (struct rtx_def *, const char *, int, const char *)


            __attribute__ ((__noreturn__));
extern void _fatal_insn (const char *, struct rtx_def *, const char *, int, const char *)



           __attribute__ ((__noreturn__));
# 58 "./toplev.h"
extern void warning (const char *, ...);
extern void error (const char *, ...);
extern void fatal_error (const char *, ...)
            __attribute__ ((__noreturn__));
extern void pedwarn (const char *, ...);
extern void pedwarn_with_file_and_line (const char *, int, const char *, ...);

extern void warning_with_file_and_line (const char *, int, const char *, ...);

extern void error_with_file_and_line (const char *, int, const char *, ...);

extern void sorry (const char *, ...);
extern void report_error_function (const char *);

extern void rest_of_decl_compilation (union tree_node *, const char *, int, int);

extern void rest_of_type_compilation (union tree_node *, int);
extern void rest_of_compilation (union tree_node *);

extern void pedwarn_with_decl (union tree_node *, const char *, ...);

extern void warning_with_decl (union tree_node *, const char *, ...);

extern void error_with_decl (union tree_node *, const char *, ...);


extern void announce_function (union tree_node *);

extern void error_for_asm (struct rtx_def *, const char *, ...);

extern void warning_for_asm (struct rtx_def *, const char *, ...);

extern void warn_deprecated_use (union tree_node *);
extern int do_float_handler (void (*) (void *), void *);


extern void output_quoted_string (FILE *, const char *);
extern void output_file_directive (FILE *, const char *);

extern void do_abort (void) __attribute__ ((__noreturn__));
extern void botch (const char *)
  __attribute__ ((__noreturn__));




extern void fnotice (FILE *, const char *, ...)
            __attribute__ ((__format__ (__printf__, 2, 3)));


extern int wrapup_global_declarations (union tree_node **, int);
extern void check_global_declarations (union tree_node **, int);

extern const char *progname;
extern const char *dump_base_name;


extern struct ht *ident_hash;




extern void set_fast_math_flags (void);
extern void set_no_fast_math_flags (void);
# 130 "./toplev.h"
extern int exact_log2_wide (unsigned long);
extern int floor_log2_wide (unsigned long);
# 44 "tree.c" 2
# 1 "./ggc.h" 1
# 21 "./ggc.h"
# 1 "./varray.h" 1
# 36 "./varray.h"
struct const_equiv_data {
# 50 "./varray.h"
  struct rtx_def *rtx;



  unsigned age;
};


typedef union varray_data_tag {
  char c[1];
  unsigned char uc[1];
  short s[1];
  unsigned short us[1];
  int i[1];
  unsigned int u[1];
  long l[1];
  unsigned long ul[1];
  long hint[1];
  unsigned long uhint[1];
  void * generic[1];
  char *cptr[1];
  struct rtx_def *rtx[1];
  struct rtvec_def *rtvec[1];
  union tree_node *tree[1];
  struct bitmap_head_def *bitmap[1];
  struct sched_info_tag *sched[1];
  struct reg_info_def *reg[1];
  struct const_equiv_data const_equiv[1];
  struct basic_block_def *bb[1];
  struct elt_list *te[1];
} varray_data;


typedef struct varray_head_tag {
  size_t num_elements;
  size_t elements_used;

  size_t element_size;
  const char *name;
  varray_data data;
} *varray_type;



extern varray_type varray_init (size_t, size_t, const char *);
# 165 "./varray.h"
extern varray_type varray_grow (varray_type, size_t);
# 22 "./ggc.h" 2








struct eh_status;
struct emit_status;
struct expr_status;
struct hash_table;
struct label_node;
struct rtx_def;
struct rtvec_def;
struct stmt_status;
union tree_node;
struct varasm_status;


extern const char empty_string[];
extern const char digit_vector[];



extern varray_type ggc_pending_trees;


extern void ggc_add_root (void *base, int nelt, int size, void (*)(void *));

extern void ggc_add_rtx_root (struct rtx_def **, int nelt);
extern void ggc_add_tree_root (union tree_node **, int nelt);

extern void ggc_add_rtx_varray_root (struct varray_head_tag **, int nelt);

extern void ggc_add_tree_varray_root (struct varray_head_tag **, int nelt);

extern void ggc_add_tree_hash_table_root (struct hash_table **, int nelt);

extern void ggc_del_root (void *base);



typedef int (*ggc_htab_marked_p) (const void *);
typedef void (*ggc_htab_mark) (const void *);




extern void ggc_add_deletable_htab (void *, ggc_htab_marked_p, ggc_htab_mark);




extern void ggc_mark_rtx_varray (struct varray_head_tag *);
extern void ggc_mark_tree_varray (struct varray_head_tag *);
extern void ggc_mark_tree_hash_table (struct hash_table *);
extern void ggc_mark_roots (void);

extern void ggc_mark_rtx_children (struct rtx_def *);
extern void ggc_mark_rtvec_children (struct rtvec_def *);
# 127 "./ggc.h"
extern void init_ggc (void);
extern void init_stringpool (void);



extern void ggc_push_context (void);



extern void ggc_pop_context (void);




extern void *ggc_alloc (size_t);

extern void *ggc_alloc_cleared (size_t);
# 158 "./ggc.h"
extern const char *ggc_alloc_string (const char *contents, int length);







extern void ggc_collect (void);






extern int ggc_set_mark (const void *);




extern int ggc_marked_p (const void *);





extern void lang_mark_tree (union tree_node *);





extern void (*lang_mark_false_label_stack) (struct label_node *);



void mark_eh_status (struct eh_status *);
void mark_emit_status (struct emit_status *);
void mark_expr_status (struct expr_status *);
void mark_stmt_status (struct stmt_status *);
void mark_varasm_status (struct varasm_status *);
void mark_optab (void *);





typedef struct ggc_statistics
{

  unsigned num_trees[256];


  size_t size_trees[256];

  unsigned num_rtxs[256];


  size_t size_rtxs[256];

  size_t total_size_trees;

  size_t total_size_rtxs;

  unsigned total_num_trees;

  unsigned total_num_rtxs;
} ggc_statistics;


extern size_t ggc_get_size (const void *);



extern void ggc_print_common_statistics (FILE *, ggc_statistics *);


extern void ggc_print_statistics (void);
extern void stringpool_statistics (void);
# 45 "tree.c" 2
# 1 "./hashtab.h" 1
# 42 "./hashtab.h"
typedef unsigned int hashval_t;




typedef hashval_t (*htab_hash) (const void *);






typedef int (*htab_eq) (const void *, const void *);



typedef void (*htab_del) (void *);





typedef int (*htab_trav) (void **, void *);






struct htab
{

  htab_hash hash_f;


  htab_eq eq_f;


  htab_del del_f;


  void * *entries;


  size_t size;


  size_t n_elements;


  size_t n_deleted;



  unsigned int searches;



  unsigned int collisions;



  int return_allocation_failure;
};

typedef struct htab *htab_t;


enum insert_option {NO_INSERT, INSERT};



extern htab_t htab_create (size_t, htab_hash, htab_eq, htab_del);





extern htab_t htab_try_create (size_t, htab_hash, htab_eq, htab_del);

extern void htab_delete (htab_t);
extern void htab_empty (htab_t);

extern void * htab_find (htab_t, const void *);
extern void * *htab_find_slot (htab_t, const void *, enum insert_option);

extern void * htab_find_with_hash (htab_t, const void *, hashval_t);

extern void * *htab_find_slot_with_hash (htab_t, const void *, hashval_t, enum insert_option);


extern void htab_clear_slot (htab_t, void **);
extern void htab_remove_elt (htab_t, void *);

extern void htab_traverse (htab_t, htab_trav, void *);

extern size_t htab_size (htab_t);
extern size_t htab_elements (htab_t);
extern double htab_collisions (htab_t);


extern htab_hash htab_hash_pointer;


extern htab_eq htab_eq_pointer;


extern hashval_t htab_hash_string (const void *);
# 46 "tree.c" 2
# 1 "./output.h" 1
# 24 "./output.h"
extern void compute_alignments (void);


extern void init_final (const char *);



extern void end_final (const char *);



extern void app_enable (void);



extern void app_disable (void);




extern int dbr_sequence_length (void);


extern void init_insn_lengths (void);
# 154 "./output.h"
extern void text_section (void);


extern void data_section (void);


extern void force_data_section (void);



extern void readonly_data_section (void);


extern int in_text_section (void);
# 178 "./output.h"
extern void bss_section (void);



extern void const_section (void);



extern void init_section (void);



extern void fini_section (void);
# 218 "./output.h"
extern void named_section (tree, const char *, int);


extern void function_section (tree);


extern void mergeable_string_section (tree, unsigned long, unsigned int);



extern void mergeable_constant_section (enum machine_mode, unsigned long, unsigned int);




extern void declare_weak (tree);

extern void merge_weak (tree, tree);



extern void weak_finish (void);
# 248 "./output.h"
extern int decode_reg_name (const char *);




extern void make_var_volatile (tree);


extern void assemble_constant_align (tree);

extern void assemble_alias (tree, tree);



extern void assemble_asm (tree);





extern void assemble_start_function (tree, const char *);



extern void assemble_end_function (tree, const char *);
# 283 "./output.h"
extern void assemble_variable (tree, int, int, int);




extern void assemble_external (tree);



extern void assemble_zeros (int);


extern void assemble_align (int);
extern void assemble_eh_align (int);


extern void assemble_string (const char *, int);







extern void assemble_global (const char *);


extern void assemble_label (const char *);
extern void assemble_eh_label (const char *);






extern void assemble_name (FILE *, const char *);
# 327 "./output.h"
extern const char *integer_asm_op (int, int);
# 360 "./output.h"
extern void clear_const_double_mem (void);


extern void defer_addressed_constants (void);



extern void output_deferred_addressed_constants (void);


extern int get_pool_size (void);







extern void output_constant_pool (const char *, tree);
# 389 "./output.h"
extern tree initializer_constant_valid_p (tree, tree);
# 399 "./output.h"
extern void output_constant (tree, long, unsigned int);
# 423 "./output.h"
extern FILE *asm_out_file;



extern const char *first_global_object_name;


extern const char *weak_global_object_name;






extern int current_function_is_leaf;




extern int current_function_nothrow;





extern int current_function_sp_is_unchanging;





extern int current_function_uses_only_leaf_regs;




extern FILE *rtl_dump_file;



extern struct rtx_def *current_insn_predicate;


extern struct rtx_def *current_output_insn;
# 480 "./output.h"
extern const char *user_label_prefix;
# 491 "./output.h"
extern void default_function_pro_epilogue (FILE *, long);


extern void default_exception_section (void);


extern void default_eh_frame_section (void);


extern void no_asm_to_stream (FILE *);
# 517 "./output.h"
extern unsigned int get_named_section_flags (const char *);
extern _Bool set_named_section_flags (const char *, unsigned int);
extern void named_section_flags (const char *, unsigned int);
extern _Bool named_section_first_declaration (const char *);

union tree_node;
extern unsigned int default_section_type_flags (union tree_node *, const char *, int);


extern void default_no_named_section (const char *, unsigned int);
extern void default_elf_asm_named_section (const char *, unsigned int);
extern void default_coff_asm_named_section (const char *, unsigned int);

extern void default_pe_asm_named_section (const char *, unsigned int);

extern void default_stabs_asm_out_destructor (struct rtx_def *, int);
extern void default_named_section_asm_out_destructor (struct rtx_def *, int);

extern void default_dtor_section_asm_out_destructor (struct rtx_def *, int);

extern void default_stabs_asm_out_constructor (struct rtx_def *, int);
extern void default_named_section_asm_out_constructor (struct rtx_def *, int);

extern void default_ctor_section_asm_out_constructor (struct rtx_def *, int);



extern void assemble_vtable_entry (struct rtx_def *, long);
extern void assemble_vtable_inherit (struct rtx_def *, struct rtx_def *);
# 47 "tree.c" 2
# 1 "./target.h" 1
# 47 "./target.h"
struct gcc_target
{

  struct asm_out
  {

    const char *open_paren, *close_paren;


    const char *byte_op;
    struct asm_int_op
    {
      const char *hi;
      const char *si;
      const char *di;
      const char *ti;
    } aligned_op, unaligned_op;






    _Bool (* integer) (rtx x, unsigned int size, int aligned_p);


    void (* function_prologue) (FILE *, long);


    void (* function_end_prologue) (FILE *);


    void (* function_begin_epilogue) (FILE *);


    void (* function_epilogue) (FILE *, long);



    void (* named_section) (const char *, unsigned int);


    void (* exception_section) (void);


    void (* eh_frame_section) (void);


    void (* constructor) (rtx, int);


    void (* destructor) (rtx, int);
  } asm_out;


  struct sched
  {



    int (* adjust_cost) (rtx insn, rtx link, rtx def_insn, int cost);



    int (* adjust_priority) (rtx, int);




    int (* issue_rate) (void);



    int (* variable_issue) (FILE *, int, rtx, int);


    void (* md_init) (FILE *, int, int);


    void (* md_finish) (FILE *, int);



    int (* reorder) (FILE *, int, rtx *, int *, int);
    int (* reorder2) (FILE *, int, rtx *, int *, int);






    rtx (* cycle_display) (int clock, rtx last);
  } sched;


  tree (* merge_decl_attributes) (tree, tree);


  tree (* merge_type_attributes) (tree, tree);


  const struct attribute_spec *attribute_table;




  int (* comp_type_attributes) (tree type1, tree type2);


  void (* set_default_type_attributes) (tree type);


  void (* insert_attributes) (tree decl, tree *attributes);



  _Bool (* function_attribute_inlinable_p) (tree fndecl);



  _Bool (* ms_bitfield_layout_p) (tree record_type);


  void (* init_builtins) (void);


  rtx (* expand_builtin) (tree exp, rtx target, rtx subtarget, enum machine_mode mode, int ignore);





  unsigned int (* section_type_flags) (tree, const char *, int);


  _Bool have_named_sections;



  _Bool have_ctors_dtors;



  _Bool (* cannot_modify_jumps_p) (void);
};

extern struct gcc_target targetm;
# 48 "tree.c" 2
# 1 "./langhooks.h" 1
# 25 "./langhooks.h"
typedef void (*lang_print_tree_hook) (FILE *, tree, int indent);




struct lang_hooks_for_tree_inlining
{
  union tree_node *(*walk_subtrees) (union tree_node **, int *, union tree_node *(*) (union tree_node **, int *, void *), void *, void *);




  int (*cannot_inline_tree_fn) (union tree_node **);
  int (*disregard_inline_limits) (union tree_node *);
  union tree_node *(*add_pending_fn_decls) (void *, union tree_node *);

  int (*tree_chain_matters_p) (union tree_node *);
  int (*auto_var_in_fn_p) (union tree_node *, union tree_node *);
  union tree_node *(*copy_res_decl_for_inlining) (union tree_node *, union tree_node *, union tree_node *, void *, int *, void *);




  int (*anon_aggr_type_p) (union tree_node *);
  int (*start_inlining) (union tree_node *);
  void (*end_inlining) (union tree_node *);
  union tree_node *(*convert_parm_for_inlining) (union tree_node *, union tree_node *, union tree_node *);


};



struct lang_hooks_for_tree_dump
{


  int (*dump_tree) (void *, tree);


  int (*type_quals) (tree);
};



struct lang_hooks
{

  const char *name;



  size_t identifier_size;



  void (*init_options) (void);
# 91 "./langhooks.h"
  int (*decode_option) (int, char **);
# 100 "./langhooks.h"
  void (*post_options) (void);







  const char * (*init) (const char *);


  void (*finish) (void);


  void (*clear_binding_stack) (void);



  long (*get_alias_set) (tree);




  tree (*expand_constant) (tree);
# 132 "./langhooks.h"
  int (*safe_from_p) (rtx, tree);


  int (*staticp) (tree);


  _Bool honor_readonly;



  void (*print_statistics) (void);



  lang_print_tree_hook print_xnode;



  lang_print_tree_hook print_decl;
  lang_print_tree_hook print_type;
  lang_print_tree_hook print_identifier;




  void (*set_yydebug) (int);

  struct lang_hooks_for_tree_inlining tree_inlining;

  struct lang_hooks_for_tree_dump tree_dump;



};


extern const struct lang_hooks lang_hooks;
# 49 "tree.c" 2




extern int _obstack_allocated_p (struct obstack *h, void * obj);

static void unsave_expr_now_r (tree);



struct obstack permanent_obstack;







char tree_code_type[256] = {

# 1 "./tree.def" 1
# 46 "./tree.def"
'x',





'x',






'x',


'x',
# 84 "./tree.def"
'b',
# 129 "./tree.def"
't',
# 139 "./tree.def"
't',



't',



't',



't',
# 164 "./tree.def"
't',



't',



't',



't',





't',



't',






't',


't',
# 209 "./tree.def"
't',





't',
# 225 "./tree.def"
't',





't',





't',







't',





't',
# 262 "./tree.def"
'c',


'c',




'c',


'c',



'c',
# 336 "./tree.def"
'd',
'd',
'd',
'd',
'd',
'd',
'd',
'd',



'd',






'r',
# 363 "./tree.def"
'r',


'r',


'r',



'r',




'r',






'r',
# 409 "./tree.def"
'e',
# 419 "./tree.def"
'e',


'e',



'e',






'e',
# 443 "./tree.def"
'e',
# 466 "./tree.def"
'e',




'e',




'e',
# 490 "./tree.def"
'e',
# 507 "./tree.def"
'e',
# 556 "./tree.def"
'x',





'e',


'2',
'2',
'2',


'2',


'2',


'2',


'2',


'2',
'2',
'2',
'2',


'2',



'2',






'1',
'1',
'1',
'1',


'1',


'1',

'2',
'2',





'1',

'1',
# 628 "./tree.def"
'2',
'2',
'2',
'2',


'2',
'2',
'2',
'2',
'1',
# 649 "./tree.def"
'e',
'e',
'e',
'e',
'e',
'e',







'<',
'<',
'<',
'<',
'<',
'<',


'<',
'<',


'<',
'<',
'<',
'<',
'<',


'2',
'<',
'1',
'2',




'1',


'1',


'1',
# 707 "./tree.def"
'1',





'e',
# 722 "./tree.def"
'e',
# 731 "./tree.def"
'e',



'e',


'e',



'e',



'e',



'2',


'1',



'1',
'1',




'e',
'e',
'e',
'e',


'e',






'e',
# 790 "./tree.def"
'e',






'e',







's',



's',






's',



's',




's',




'e',




'e',
# 844 "./tree.def"
'e',





'e',


'e',
# 69 "tree.c" 2
};
# 78 "tree.c"
int tree_code_length[256] = {

# 1 "./tree.def" 1
# 46 "./tree.def"
0,





-1,






2,


2,
# 84 "./tree.def"
0,
# 129 "./tree.def"
0,
# 139 "./tree.def"
0,



0,



0,



0,
# 164 "./tree.def"
0,



0,



0,



0,





0,



0,






0,


0,
# 209 "./tree.def"
0,





0,
# 225 "./tree.def"
0,





0,





0,







0,





0,
# 262 "./tree.def"
2,


3,




3,


3,



3,
# 336 "./tree.def"
0,
0,
0,
0,
0,
0,
0,
0,



0,






2,
# 363 "./tree.def"
3,


1,


1,



2,




2,






3,
# 409 "./tree.def"
2,
# 419 "./tree.def"
2,


2,



2,






4,
# 443 "./tree.def"
3,
# 466 "./tree.def"
3,




2,




4,
# 490 "./tree.def"
3,
# 507 "./tree.def"
1,
# 556 "./tree.def"
0,





2,


2,
2,
2,


2,


2,


2,


2,


2,
2,
2,
2,


2,



2,






1,
1,
1,
1,


1,


1,

2,
2,





1,

1,
# 628 "./tree.def"
2,
2,
2,
2,


2,
2,
2,
2,
1,
# 649 "./tree.def"
2,
2,
2,
2,
2,
1,







2,
2,
2,
2,
2,
2,


2,
2,


2,
2,
2,
2,
2,


2,
2,
1,
2,




1,


1,


1,
# 707 "./tree.def"
1,





3,
# 722 "./tree.def"
1,
# 731 "./tree.def"
2,



1,


1,



1,



2,



2,


1,



1,
1,




2,
2,
2,
2,


1,






2,
# 790 "./tree.def"
2,






2,







1,



1,






1,



1,




1,




2,




2,
# 844 "./tree.def"
3,





2,


0,
# 80 "tree.c" 2
};






const char *tree_code_name[256] = {

# 1 "./tree.def" 1
# 46 "./tree.def"
"error_mark",





"identifier_node",






"tree_list",


"tree_vec",
# 84 "./tree.def"
"block",
# 129 "./tree.def"
"void_type",
# 139 "./tree.def"
"integer_type",



"real_type",



"complex_type",



"vector_type",
# 164 "./tree.def"
"enumeral_type",



"boolean_type",



"char_type",



"pointer_type",





"offset_type",



"reference_type",






"method_type",


"file_type",
# 209 "./tree.def"
"array_type",





"set_type",
# 225 "./tree.def"
"record_type",





"union_type",





"qual_union_type",







"function_type",





"lang_type",
# 262 "./tree.def"
"integer_cst",


"real_cst",




"complex_cst",


"vector_cst",



"string_cst",
# 336 "./tree.def"
"function_decl",
"label_decl",
"const_decl",
"type_decl",
"var_decl",
"parm_decl",
"result_decl",
"field_decl",



"namespace_decl",






"component_ref",
# 363 "./tree.def"
"bit_field_ref",


"indirect_ref",


"buffer_ref",



"array_ref",




"array_range_ref",






"vtable_ref",
# 409 "./tree.def"
"constructor",
# 419 "./tree.def"
"compound_expr",


"modify_expr",



"init_expr",






"target_expr",
# 443 "./tree.def"
"cond_expr",
# 466 "./tree.def"
"bind_expr",




"call_expr",




"method_call_expr",
# 490 "./tree.def"
"with_cleanup_expr",
# 507 "./tree.def"
"cleanup_point_expr",
# 556 "./tree.def"
"placeholder_expr",





"with_record_expr",


"plus_expr",
"minus_expr",
"mult_expr",


"trunc_div_expr",


"ceil_div_expr",


"floor_div_expr",


"round_div_expr",


"trunc_mod_expr",
"ceil_mod_expr",
"floor_mod_expr",
"round_mod_expr",


"rdiv_expr",



"exact_div_expr",






"fix_trunc_expr",
"fix_ceil_expr",
"fix_floor_expr",
"fix_round_expr",


"float_expr",


"negate_expr",

"min_expr",
"max_expr",





"abs_expr",

"ffs_expr",
# 628 "./tree.def"
"lshift_expr",
"rshift_expr",
"lrotate_expr",
"rrotate_expr",


"bit_ior_expr",
"bit_xor_expr",
"bit_and_expr",
"bit_andtc_expr",
"bit_not_expr",
# 649 "./tree.def"
"truth_andif_expr",
"truth_orif_expr",
"truth_and_expr",
"truth_or_expr",
"truth_xor_expr",
"truth_not_expr",







"lt_expr",
"le_expr",
"gt_expr",
"ge_expr",
"eq_expr",
"ne_expr",


"unordered_expr",
"ordered_expr",


"unlt_expr",
"unle_expr",
"ungt_expr",
"unge_expr",
"uneq_expr",


"in_expr",
"set_le_expr",
"card_expr",
"range_expr",




"convert_expr",


"nop_expr",


"non_lvalue_expr",
# 707 "./tree.def"
"view_convert_expr",





"save_expr",
# 722 "./tree.def"
"unsave_expr",
# 731 "./tree.def"
"rtl_expr",



"addr_expr",


"reference_expr",



"entry_value_expr",



"fdesc_expr",



"complex_expr",


"conj_expr",



"realpart_expr",
"imagpart_expr",




"predecrement_expr",
"preincrement_expr",
"postdecrement_expr",
"postincrement_expr",


"va_arg_expr",






"try_catch_expr",
# 790 "./tree.def"
"try_finally",






"goto_subroutine",







"label_expr",



"goto_expr",






"return_expr",



"exit_expr",




"loop_expr",




"labeled_block_expr",




"exit_block_expr",
# 844 "./tree.def"
"expr_with_file_location",





"switch_expr",


"exc_ptr_expr",
# 89 "tree.c" 2
};



typedef enum
{
  d_kind,
  t_kind,
  b_kind,
  s_kind,
  r_kind,
  e_kind,
  c_kind,
  id_kind,
  perm_list_kind,
  temp_list_kind,
  vec_kind,
  x_kind,
  lang_decl,
  lang_type,
  all_kinds
} tree_node_kind;

int tree_node_counts[(int) all_kinds];
int tree_node_sizes[(int) all_kinds];

static const char * const tree_node_kind_names[] = {
  "decls",
  "types",
  "blocks",
  "stmts",
  "refs",
  "exprs",
  "constants",
  "identifiers",
  "perm_tree_lists",
  "temp_tree_lists",
  "vecs",
  "random kinds",
  "lang_decl kinds",
  "lang_type kinds"
};


static int next_decl_uid;

static int next_type_uid = 1;




struct type_hash
{
  unsigned long hash;
  tree type;
};
# 156 "tree.c"
htab_t type_hash_table;

static void build_real_from_int_cst_1 (void *);
static void set_type_quals (tree, int);
static void append_random_chars (char *);
static int type_hash_eq (const void*, const void*);
static unsigned int type_hash_hash (const void*);
static void print_type_hash_statistics (void);
static void finish_vector_type (tree);
static tree make_vector (enum machine_mode, tree, int);
static int type_hash_marked_p (const void *);
static void type_hash_mark (const void *);
static int mark_tree_hashtable_entry (void **, void *);






void (*lang_unsave) (tree *);
void (*lang_unsave_expr_now) (tree);



int (*lang_unsafe_for_reeval) (tree);






void (*lang_set_decl_assembler_name) (tree);

tree global_trees[TI_MAX];
tree integer_types[itk_none];


void
set_decl_assembler_name (decl)
     tree decl;
{




  if (((enum tree_code) (decl)->common.code) == FUNCTION_DECL
      || (((enum tree_code) (decl)->common.code) == VAR_DECL
   && (((decl)->common.static_flag)
       || ((decl)->decl.external_flag)
       || ((decl)->common.public_flag))))





    ((decl)->decl.assembler_name = (((decl)->decl.name)));
  else



    fancy_abort ("tree.c", 216, __FUNCTION__);
}



void
init_obstacks ()
{
  gcc_obstack_init (&permanent_obstack);


  type_hash_table = htab_create (1000, type_hash_hash,
     type_hash_eq, 0);
  ggc_add_deletable_htab (type_hash_table, type_hash_marked_p,
     type_hash_mark);
  ggc_add_tree_root (global_trees, TI_MAX);
  ggc_add_tree_root (integer_types, itk_none);


  lang_set_decl_assembler_name = set_decl_assembler_name;
}





char *
permalloc (size)
     int size;
{
  return (char *) __extension__ ({ struct obstack *__h = (&permanent_obstack); __extension__ ({ struct obstack *__o = (__h); int __len = ((size)); if (__o->chunk_limit - __o->next_free < __len) _obstack_newchunk (__o, __len); __o->next_free += __len; (void) 0; }); __extension__ ({ struct obstack *__o1 = (__h); void *value; value = (void *) __o1->object_base; if (__o1->next_free == value) __o1->maybe_empty_object = 1; __o1->next_free = (((((__o1->next_free) - (char *) 0)+__o1->alignment_mask) & ~ (__o1->alignment_mask)) + (char *) 0); if (__o1->next_free - (char *)__o1->chunk > __o1->chunk_limit - (char *)__o1->chunk) __o1->next_free = __o1->chunk_limit; __o1->object_base = __o1->next_free; value; }); });
}





char *
perm_calloc (nelem, size)
     int nelem;
     long size;
{
  char *rval = (char *) __extension__ ({ struct obstack *__h = (&permanent_obstack); __extension__ ({ struct obstack *__o = (__h); int __len = ((nelem * size)); if (__o->chunk_limit - __o->next_free < __len) _obstack_newchunk (__o, __len); __o->next_free += __len; (void) 0; }); __extension__ ({ struct obstack *__o1 = (__h); void *value; value = (void *) __o1->object_base; if (__o1->next_free == value) __o1->maybe_empty_object = 1; __o1->next_free = (((((__o1->next_free) - (char *) 0)+__o1->alignment_mask) & ~ (__o1->alignment_mask)) + (char *) 0); if (__o1->next_free - (char *)__o1->chunk > __o1->chunk_limit - (char *)__o1->chunk) __o1->next_free = __o1->chunk_limit; __o1->object_base = __o1->next_free; value; }); });
  memset (rval, 0, nelem * size);
  return rval;
}


