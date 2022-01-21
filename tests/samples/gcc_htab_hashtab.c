# 1 "hashtab.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 341 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "hashtab.c" 2
# 34 "hashtab.c"
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
# 35 "hashtab.c" 2

# 1 "/usr/include/x86_64-linux-gnu/sys/types.h" 1 3 4
# 25 "/usr/include/x86_64-linux-gnu/sys/types.h" 3 4
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
# 26 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



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
# 30 "/usr/include/x86_64-linux-gnu/sys/types.h" 2 3 4



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





typedef __off_t off_t;






typedef __off64_t off64_t;




typedef __pid_t pid_t;





typedef __id_t id_t;




typedef __ssize_t ssize_t;





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
# 46 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 3 4
typedef long unsigned int size_t;
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
# 37 "hashtab.c" 2


# 1 "/usr/include/stdlib.h" 1 3 4
# 25 "/usr/include/stdlib.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 26 "/usr/include/stdlib.h" 2 3 4





# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 74 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 3 4
typedef int wchar_t;
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
# 272 "/usr/include/stdlib.h" 3 4
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
# 273 "/usr/include/stdlib.h" 2 3 4

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
# 40 "hashtab.c" 2



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
# 44 "hashtab.c" 2


# 1 "/usr/include/stdio.h" 1 3 4
# 27 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/libc-header-start.h" 1 3 4
# 28 "/usr/include/stdio.h" 2 3 4





# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3 4
# 34 "/usr/include/stdio.h" 2 3 4


# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stdarg.h" 1 3 4
# 14 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stdarg.h" 3 4
typedef __builtin_va_list va_list;
# 32 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 37 "/usr/include/stdio.h" 2 3 4


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
# 84 "/usr/include/stdio.h" 3 4
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
# 47 "hashtab.c" 2

# 1 "./libiberty.h" 1
# 46 "./libiberty.h"
# 1 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 1 3
# 35 "/usr/lib/llvm-10/lib/clang/10.0.0/include/stddef.h" 3
typedef long int ptrdiff_t;
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
# 49 "hashtab.c" 2
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
# 50 "hashtab.c" 2
# 60 "hashtab.c"
static unsigned long higher_prime_number (unsigned long);
static hashval_t hash_pointer (const void *);
static int eq_pointer (const void *, const void *);
static int htab_expand (htab_t);
static void * *find_empty_slot_for_expand (htab_t, hashval_t);




htab_hash htab_hash_pointer = hash_pointer;
htab_eq htab_eq_pointer = eq_pointer;








void
htab_traverse (htab, callback, info)
     htab_t htab;
     htab_trav callback;
     void * info;
{
  void * *slot = htab->entries;
  void * *limit = slot + htab->size;

  do
    {
      void * x = *slot;

      if (x != ((void *) 0) && x != ((void *) 1))
 if (!(*callback) (slot, info))
   break;
    }
  while (++slot < limit);
}

