; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -amdgpu-scalarize-global-loads=false -mtriple=amdgcn-- -mcpu=tahiti -verify-machineinstrs  | FileCheck %s -check-prefixes=GCN,SI
; RUN: llc < %s -amdgpu-scalarize-global-loads=false -mtriple=amdgcn-- -mcpu=fiji -mattr=-flat-for-global -verify-machineinstrs | FileCheck %s -check-prefixes=GCN,VI

define amdgpu_kernel void @select_f16(
; SI-LABEL: select_f16:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[4:11], s[0:1], 0x9
; SI-NEXT:    s_load_dwordx2 s[0:1], s[0:1], 0x11
; SI-NEXT:    s_mov_b32 s15, 0xf000
; SI-NEXT:    s_mov_b32 s14, -1
; SI-NEXT:    s_mov_b32 s22, s14
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s16, s10
; SI-NEXT:    s_mov_b32 s17, s11
; SI-NEXT:    s_mov_b32 s10, s14
; SI-NEXT:    s_mov_b32 s11, s15
; SI-NEXT:    s_mov_b32 s20, s6
; SI-NEXT:    s_mov_b32 s21, s7
; SI-NEXT:    s_mov_b32 s23, s15
; SI-NEXT:    s_mov_b32 s2, s14
; SI-NEXT:    s_mov_b32 s3, s15
; SI-NEXT:    s_mov_b32 s18, s14
; SI-NEXT:    s_mov_b32 s19, s15
; SI-NEXT:    buffer_load_ushort v0, off, s[20:23], 0
; SI-NEXT:    buffer_load_ushort v1, off, s[8:11], 0
; SI-NEXT:    buffer_load_ushort v2, off, s[16:19], 0
; SI-NEXT:    buffer_load_ushort v3, off, s[0:3], 0
; SI-NEXT:    s_mov_b32 s12, s4
; SI-NEXT:    s_mov_b32 s13, s5
; SI-NEXT:    s_waitcnt vmcnt(3)
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_cvt_f32_f16_e32 v2, v2
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_cvt_f32_f16_e32 v3, v3
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, v0, v1
; SI-NEXT:    v_cndmask_b32_e32 v0, v3, v2, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    buffer_store_short v0, off, s[12:15], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_f16:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[4:11], s[0:1], 0x24
; VI-NEXT:    s_load_dwordx2 s[12:13], s[0:1], 0x44
; VI-NEXT:    s_mov_b32 s3, 0xf000
; VI-NEXT:    s_mov_b32 s2, -1
; VI-NEXT:    s_mov_b32 s14, s2
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s0, s4
; VI-NEXT:    s_mov_b32 s1, s5
; VI-NEXT:    s_mov_b32 s16, s10
; VI-NEXT:    s_mov_b32 s17, s11
; VI-NEXT:    s_mov_b32 s4, s6
; VI-NEXT:    s_mov_b32 s5, s7
; VI-NEXT:    s_mov_b32 s10, s2
; VI-NEXT:    s_mov_b32 s11, s3
; VI-NEXT:    s_mov_b32 s6, s2
; VI-NEXT:    s_mov_b32 s7, s3
; VI-NEXT:    s_mov_b32 s15, s3
; VI-NEXT:    s_mov_b32 s18, s2
; VI-NEXT:    s_mov_b32 s19, s3
; VI-NEXT:    buffer_load_ushort v0, off, s[4:7], 0
; VI-NEXT:    buffer_load_ushort v1, off, s[8:11], 0
; VI-NEXT:    buffer_load_ushort v2, off, s[16:19], 0
; VI-NEXT:    buffer_load_ushort v3, off, s[12:15], 0
; VI-NEXT:    s_waitcnt vmcnt(2)
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, v0, v1
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v3, v2, vcc
; VI-NEXT:    buffer_store_short v0, off, s[0:3], 0
; VI-NEXT:    s_endpgm
    half addrspace(1)* %r,
    half addrspace(1)* %a,
    half addrspace(1)* %b,
    half addrspace(1)* %c,
    half addrspace(1)* %d) {
entry:
  %a.val = load volatile half, half addrspace(1)* %a
  %b.val = load volatile half, half addrspace(1)* %b
  %c.val = load volatile half, half addrspace(1)* %c
  %d.val = load volatile half, half addrspace(1)* %d
  %fcmp = fcmp olt half %a.val, %b.val
  %r.val = select i1 %fcmp, half %c.val, half %d.val
  store half %r.val, half addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_f16_imm_a(
; SI-LABEL: select_f16_imm_a:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s11, 0xf000
; SI-NEXT:    s_mov_b32 s10, -1
; SI-NEXT:    s_mov_b32 s18, s10
; SI-NEXT:    s_mov_b32 s19, s11
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s16, s2
; SI-NEXT:    s_mov_b32 s17, s3
; SI-NEXT:    s_mov_b32 s12, s6
; SI-NEXT:    s_mov_b32 s13, s7
; SI-NEXT:    s_mov_b32 s14, s10
; SI-NEXT:    s_mov_b32 s15, s11
; SI-NEXT:    s_mov_b32 s6, s10
; SI-NEXT:    s_mov_b32 s7, s11
; SI-NEXT:    buffer_load_ushort v0, off, s[16:19], 0
; SI-NEXT:    buffer_load_ushort v1, off, s[4:7], 0
; SI-NEXT:    buffer_load_ushort v2, off, s[12:15], 0
; SI-NEXT:    s_mov_b32 s8, s0
; SI-NEXT:    s_mov_b32 s9, s1
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_cvt_f32_f16_e32 v2, v2
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, 0.5, v0
; SI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    buffer_store_short v0, off, s[8:11], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_f16_imm_a:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x24
; VI-NEXT:    s_mov_b32 s11, 0xf000
; VI-NEXT:    s_mov_b32 s10, -1
; VI-NEXT:    s_mov_b32 s14, s10
; VI-NEXT:    s_mov_b32 s15, s11
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s8, s0
; VI-NEXT:    s_mov_b32 s9, s1
; VI-NEXT:    s_mov_b32 s0, s2
; VI-NEXT:    s_mov_b32 s1, s3
; VI-NEXT:    s_mov_b32 s2, s10
; VI-NEXT:    s_mov_b32 s3, s11
; VI-NEXT:    s_mov_b32 s12, s6
; VI-NEXT:    s_mov_b32 s13, s7
; VI-NEXT:    s_mov_b32 s6, s10
; VI-NEXT:    s_mov_b32 s7, s11
; VI-NEXT:    buffer_load_ushort v0, off, s[0:3], 0
; VI-NEXT:    buffer_load_ushort v1, off, s[4:7], 0
; VI-NEXT:    buffer_load_ushort v2, off, s[12:15], 0
; VI-NEXT:    s_waitcnt vmcnt(2)
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, 0.5, v0
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; VI-NEXT:    buffer_store_short v0, off, s[8:11], 0
; VI-NEXT:    s_endpgm
    half addrspace(1)* %r,
    half addrspace(1)* %b,
    half addrspace(1)* %c,
    half addrspace(1)* %d) {
entry:
  %b.val = load volatile half, half addrspace(1)* %b
  %c.val = load volatile half, half addrspace(1)* %c
  %d.val = load volatile half, half addrspace(1)* %d
  %fcmp = fcmp olt half 0xH3800, %b.val
  %r.val = select i1 %fcmp, half %c.val, half %d.val
  store half %r.val, half addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_f16_imm_b(
; SI-LABEL: select_f16_imm_b:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s11, 0xf000
; SI-NEXT:    s_mov_b32 s10, -1
; SI-NEXT:    s_mov_b32 s18, s10
; SI-NEXT:    s_mov_b32 s19, s11
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s16, s2
; SI-NEXT:    s_mov_b32 s17, s3
; SI-NEXT:    s_mov_b32 s12, s6
; SI-NEXT:    s_mov_b32 s13, s7
; SI-NEXT:    s_mov_b32 s14, s10
; SI-NEXT:    s_mov_b32 s15, s11
; SI-NEXT:    s_mov_b32 s6, s10
; SI-NEXT:    s_mov_b32 s7, s11
; SI-NEXT:    buffer_load_ushort v0, off, s[16:19], 0
; SI-NEXT:    buffer_load_ushort v1, off, s[4:7], 0
; SI-NEXT:    buffer_load_ushort v2, off, s[12:15], 0
; SI-NEXT:    s_mov_b32 s8, s0
; SI-NEXT:    s_mov_b32 s9, s1
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_cvt_f32_f16_e32 v2, v2
; SI-NEXT:    v_cmp_gt_f32_e32 vcc, 0.5, v0
; SI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    buffer_store_short v0, off, s[8:11], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_f16_imm_b:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x24
; VI-NEXT:    s_mov_b32 s11, 0xf000
; VI-NEXT:    s_mov_b32 s10, -1
; VI-NEXT:    s_mov_b32 s14, s10
; VI-NEXT:    s_mov_b32 s15, s11
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s8, s0
; VI-NEXT:    s_mov_b32 s9, s1
; VI-NEXT:    s_mov_b32 s0, s2
; VI-NEXT:    s_mov_b32 s1, s3
; VI-NEXT:    s_mov_b32 s2, s10
; VI-NEXT:    s_mov_b32 s3, s11
; VI-NEXT:    s_mov_b32 s12, s6
; VI-NEXT:    s_mov_b32 s13, s7
; VI-NEXT:    s_mov_b32 s6, s10
; VI-NEXT:    s_mov_b32 s7, s11
; VI-NEXT:    buffer_load_ushort v0, off, s[0:3], 0
; VI-NEXT:    buffer_load_ushort v1, off, s[4:7], 0
; VI-NEXT:    buffer_load_ushort v2, off, s[12:15], 0
; VI-NEXT:    s_waitcnt vmcnt(2)
; VI-NEXT:    v_cmp_gt_f16_e32 vcc, 0.5, v0
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; VI-NEXT:    buffer_store_short v0, off, s[8:11], 0
; VI-NEXT:    s_endpgm
    half addrspace(1)* %r,
    half addrspace(1)* %a,
    half addrspace(1)* %c,
    half addrspace(1)* %d) {
entry:
  %a.val = load volatile half, half addrspace(1)* %a
  %c.val = load volatile half, half addrspace(1)* %c
  %d.val = load volatile half, half addrspace(1)* %d
  %fcmp = fcmp olt half %a.val, 0xH3800
  %r.val = select i1 %fcmp, half %c.val, half %d.val
  store half %r.val, half addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_f16_imm_c(
; SI-LABEL: select_f16_imm_c:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s11, 0xf000
; SI-NEXT:    s_mov_b32 s10, -1
; SI-NEXT:    s_mov_b32 s18, s10
; SI-NEXT:    s_mov_b32 s19, s11
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s12, s6
; SI-NEXT:    s_mov_b32 s13, s7
; SI-NEXT:    s_mov_b32 s6, s10
; SI-NEXT:    s_mov_b32 s7, s11
; SI-NEXT:    s_mov_b32 s16, s2
; SI-NEXT:    s_mov_b32 s17, s3
; SI-NEXT:    s_mov_b32 s14, s10
; SI-NEXT:    s_mov_b32 s15, s11
; SI-NEXT:    buffer_load_ushort v0, off, s[16:19], 0
; SI-NEXT:    buffer_load_ushort v1, off, s[4:7], 0
; SI-NEXT:    buffer_load_ushort v2, off, s[12:15], 0
; SI-NEXT:    s_mov_b32 s8, s0
; SI-NEXT:    s_mov_b32 s9, s1
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_cvt_f32_f16_e32 v2, v2
; SI-NEXT:    v_cmp_nlt_f32_e32 vcc, v0, v1
; SI-NEXT:    v_cndmask_b32_e32 v0, 0.5, v2, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    buffer_store_short v0, off, s[8:11], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_f16_imm_c:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x24
; VI-NEXT:    s_mov_b32 s11, 0xf000
; VI-NEXT:    s_mov_b32 s10, -1
; VI-NEXT:    s_mov_b32 s14, s10
; VI-NEXT:    s_mov_b32 s15, s11
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s8, s0
; VI-NEXT:    s_mov_b32 s9, s1
; VI-NEXT:    s_mov_b32 s12, s6
; VI-NEXT:    s_mov_b32 s13, s7
; VI-NEXT:    s_mov_b32 s0, s2
; VI-NEXT:    s_mov_b32 s1, s3
; VI-NEXT:    s_mov_b32 s6, s10
; VI-NEXT:    s_mov_b32 s7, s11
; VI-NEXT:    s_mov_b32 s2, s10
; VI-NEXT:    s_mov_b32 s3, s11
; VI-NEXT:    buffer_load_ushort v0, off, s[0:3], 0
; VI-NEXT:    buffer_load_ushort v1, off, s[4:7], 0
; VI-NEXT:    buffer_load_ushort v3, off, s[12:15], 0
; VI-NEXT:    v_mov_b32_e32 v2, 0x3800
; VI-NEXT:    s_waitcnt vmcnt(1)
; VI-NEXT:    v_cmp_nlt_f16_e32 vcc, v0, v1
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v3, vcc
; VI-NEXT:    buffer_store_short v0, off, s[8:11], 0
; VI-NEXT:    s_endpgm
    half addrspace(1)* %r,
    half addrspace(1)* %a,
    half addrspace(1)* %b,
    half addrspace(1)* %d) {
entry:
  %a.val = load volatile half, half addrspace(1)* %a
  %b.val = load volatile half, half addrspace(1)* %b
  %d.val = load volatile half, half addrspace(1)* %d
  %fcmp = fcmp olt half %a.val, %b.val
  %r.val = select i1 %fcmp, half 0xH3800, half %d.val
  store half %r.val, half addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_f16_imm_d(
; SI-LABEL: select_f16_imm_d:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s11, 0xf000
; SI-NEXT:    s_mov_b32 s10, -1
; SI-NEXT:    s_mov_b32 s18, s10
; SI-NEXT:    s_mov_b32 s19, s11
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s12, s6
; SI-NEXT:    s_mov_b32 s13, s7
; SI-NEXT:    s_mov_b32 s6, s10
; SI-NEXT:    s_mov_b32 s7, s11
; SI-NEXT:    s_mov_b32 s16, s2
; SI-NEXT:    s_mov_b32 s17, s3
; SI-NEXT:    s_mov_b32 s14, s10
; SI-NEXT:    s_mov_b32 s15, s11
; SI-NEXT:    buffer_load_ushort v0, off, s[16:19], 0
; SI-NEXT:    buffer_load_ushort v1, off, s[4:7], 0
; SI-NEXT:    buffer_load_ushort v2, off, s[12:15], 0
; SI-NEXT:    s_mov_b32 s8, s0
; SI-NEXT:    s_mov_b32 s9, s1
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_cvt_f32_f16_e32 v2, v2
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, v0, v1
; SI-NEXT:    v_cndmask_b32_e32 v0, 0.5, v2, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    buffer_store_short v0, off, s[8:11], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_f16_imm_d:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x24
; VI-NEXT:    s_mov_b32 s11, 0xf000
; VI-NEXT:    s_mov_b32 s10, -1
; VI-NEXT:    s_mov_b32 s14, s10
; VI-NEXT:    s_mov_b32 s15, s11
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s8, s0
; VI-NEXT:    s_mov_b32 s9, s1
; VI-NEXT:    s_mov_b32 s12, s6
; VI-NEXT:    s_mov_b32 s13, s7
; VI-NEXT:    s_mov_b32 s0, s2
; VI-NEXT:    s_mov_b32 s1, s3
; VI-NEXT:    s_mov_b32 s6, s10
; VI-NEXT:    s_mov_b32 s7, s11
; VI-NEXT:    s_mov_b32 s2, s10
; VI-NEXT:    s_mov_b32 s3, s11
; VI-NEXT:    buffer_load_ushort v0, off, s[0:3], 0
; VI-NEXT:    buffer_load_ushort v1, off, s[4:7], 0
; VI-NEXT:    buffer_load_ushort v3, off, s[12:15], 0
; VI-NEXT:    v_mov_b32_e32 v2, 0x3800
; VI-NEXT:    s_waitcnt vmcnt(1)
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, v0, v1
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v3, vcc
; VI-NEXT:    buffer_store_short v0, off, s[8:11], 0
; VI-NEXT:    s_endpgm
    half addrspace(1)* %r,
    half addrspace(1)* %a,
    half addrspace(1)* %b,
    half addrspace(1)* %c) {
entry:
  %a.val = load volatile half, half addrspace(1)* %a
  %b.val = load volatile half, half addrspace(1)* %b
  %c.val = load volatile half, half addrspace(1)* %c
  %fcmp = fcmp olt half %a.val, %b.val
  %r.val = select i1 %fcmp, half %c.val, half 0xH3800
  store half %r.val, half addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_v2f16(
; SI-LABEL: select_v2f16:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[4:11], s[0:1], 0x9
; SI-NEXT:    s_load_dwordx2 s[12:13], s[0:1], 0x11
; SI-NEXT:    s_mov_b32 s3, 0xf000
; SI-NEXT:    s_mov_b32 s2, -1
; SI-NEXT:    s_mov_b32 s22, s2
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s16, s10
; SI-NEXT:    s_mov_b32 s17, s11
; SI-NEXT:    s_mov_b32 s10, s2
; SI-NEXT:    s_mov_b32 s11, s3
; SI-NEXT:    s_mov_b32 s20, s6
; SI-NEXT:    s_mov_b32 s21, s7
; SI-NEXT:    s_mov_b32 s23, s3
; SI-NEXT:    s_mov_b32 s14, s2
; SI-NEXT:    s_mov_b32 s15, s3
; SI-NEXT:    buffer_load_dword v0, off, s[20:23], 0
; SI-NEXT:    s_mov_b32 s18, s2
; SI-NEXT:    s_mov_b32 s19, s3
; SI-NEXT:    buffer_load_dword v1, off, s[8:11], 0
; SI-NEXT:    buffer_load_dword v2, off, s[12:15], 0
; SI-NEXT:    buffer_load_dword v3, off, s[16:19], 0
; SI-NEXT:    s_mov_b32 s0, s4
; SI-NEXT:    s_mov_b32 s1, s5
; SI-NEXT:    s_waitcnt vmcnt(3)
; SI-NEXT:    v_lshrrev_b32_e32 v5, 16, v0
; SI-NEXT:    v_cvt_f32_f16_e32 v5, v5
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_lshrrev_b32_e32 v6, 16, v1
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_cvt_f32_f16_e32 v4, v2
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_lshrrev_b32_e32 v7, 16, v3
; SI-NEXT:    v_lshrrev_b32_e32 v2, 16, v2
; SI-NEXT:    v_cvt_f32_f16_e32 v6, v6
; SI-NEXT:    v_cvt_f32_f16_e32 v2, v2
; SI-NEXT:    v_cvt_f32_f16_e32 v7, v7
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    v_cvt_f32_f16_e32 v3, v3
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, v5, v6
; SI-NEXT:    v_cndmask_b32_e32 v2, v2, v7, vcc
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, v0, v1
; SI-NEXT:    v_cndmask_b32_e32 v0, v4, v3, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v2, v2
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    v_lshlrev_b32_e32 v1, 16, v2
; SI-NEXT:    v_or_b32_e32 v0, v0, v1
; SI-NEXT:    buffer_store_dword v0, off, s[0:3], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_v2f16:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[4:11], s[0:1], 0x24
; VI-NEXT:    s_load_dwordx2 s[12:13], s[0:1], 0x44
; VI-NEXT:    s_mov_b32 s3, 0xf000
; VI-NEXT:    s_mov_b32 s2, -1
; VI-NEXT:    s_mov_b32 s14, s2
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s0, s4
; VI-NEXT:    s_mov_b32 s1, s5
; VI-NEXT:    s_mov_b32 s16, s10
; VI-NEXT:    s_mov_b32 s17, s11
; VI-NEXT:    s_mov_b32 s4, s6
; VI-NEXT:    s_mov_b32 s5, s7
; VI-NEXT:    s_mov_b32 s10, s2
; VI-NEXT:    s_mov_b32 s11, s3
; VI-NEXT:    s_mov_b32 s6, s2
; VI-NEXT:    s_mov_b32 s7, s3
; VI-NEXT:    s_mov_b32 s15, s3
; VI-NEXT:    buffer_load_dword v0, off, s[4:7], 0
; VI-NEXT:    s_mov_b32 s18, s2
; VI-NEXT:    s_mov_b32 s19, s3
; VI-NEXT:    buffer_load_dword v1, off, s[8:11], 0
; VI-NEXT:    buffer_load_dword v2, off, s[12:15], 0
; VI-NEXT:    buffer_load_dword v3, off, s[16:19], 0
; VI-NEXT:    s_waitcnt vmcnt(3)
; VI-NEXT:    v_lshrrev_b32_e32 v6, 16, v0
; VI-NEXT:    s_waitcnt vmcnt(2)
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, v0, v1
; VI-NEXT:    v_lshrrev_b32_e32 v5, 16, v1
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v3, vcc
; VI-NEXT:    v_lshrrev_b32_e32 v4, 16, v2
; VI-NEXT:    v_lshrrev_b32_e32 v1, 16, v3
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, v6, v5
; VI-NEXT:    v_cndmask_b32_e32 v1, v4, v1, vcc
; VI-NEXT:    v_lshlrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_or_b32_sdwa v0, v0, v1 dst_sel:DWORD dst_unused:UNUSED_PAD src0_sel:WORD_0 src1_sel:DWORD
; VI-NEXT:    buffer_store_dword v0, off, s[0:3], 0
; VI-NEXT:    s_endpgm
    <2 x half> addrspace(1)* %r,
    <2 x half> addrspace(1)* %a,
    <2 x half> addrspace(1)* %b,
    <2 x half> addrspace(1)* %c,
    <2 x half> addrspace(1)* %d) {
entry:
  %a.val = load <2 x half>, <2 x half> addrspace(1)* %a
  %b.val = load <2 x half>, <2 x half> addrspace(1)* %b
  %c.val = load <2 x half>, <2 x half> addrspace(1)* %c
  %d.val = load <2 x half>, <2 x half> addrspace(1)* %d
  %fcmp = fcmp olt <2 x half> %a.val, %b.val
  %r.val = select <2 x i1> %fcmp, <2 x half> %c.val, <2 x half> %d.val
  store <2 x half> %r.val, <2 x half> addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_v2f16_imm_a(
; SI-LABEL: select_v2f16_imm_a:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s11, 0xf000
; SI-NEXT:    s_mov_b32 s10, -1
; SI-NEXT:    s_mov_b32 s18, s10
; SI-NEXT:    s_mov_b32 s19, s11
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s16, s2
; SI-NEXT:    s_mov_b32 s17, s3
; SI-NEXT:    s_mov_b32 s12, s6
; SI-NEXT:    s_mov_b32 s13, s7
; SI-NEXT:    s_mov_b32 s14, s10
; SI-NEXT:    s_mov_b32 s15, s11
; SI-NEXT:    s_mov_b32 s6, s10
; SI-NEXT:    s_mov_b32 s7, s11
; SI-NEXT:    buffer_load_dword v0, off, s[16:19], 0
; SI-NEXT:    buffer_load_dword v1, off, s[4:7], 0
; SI-NEXT:    buffer_load_dword v2, off, s[12:15], 0
; SI-NEXT:    s_mov_b32 s2, 0x3f200000
; SI-NEXT:    s_mov_b32 s8, s0
; SI-NEXT:    s_mov_b32 s9, s1
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_lshrrev_b32_e32 v3, 16, v0
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_lshrrev_b32_e32 v4, 16, v1
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_lshrrev_b32_e32 v5, 16, v2
; SI-NEXT:    v_cvt_f32_f16_e32 v3, v3
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    v_cvt_f32_f16_e32 v4, v4
; SI-NEXT:    v_cvt_f32_f16_e32 v5, v5
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    v_cvt_f32_f16_e32 v2, v2
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, s2, v3
; SI-NEXT:    v_cndmask_b32_e32 v3, v5, v4, vcc
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, 0.5, v0
; SI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v3, v3
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    v_lshlrev_b32_e32 v1, 16, v3
; SI-NEXT:    v_or_b32_e32 v0, v0, v1
; SI-NEXT:    buffer_store_dword v0, off, s[8:11], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_v2f16_imm_a:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x24
; VI-NEXT:    s_mov_b32 s11, 0xf000
; VI-NEXT:    s_mov_b32 s10, -1
; VI-NEXT:    s_mov_b32 s14, s10
; VI-NEXT:    s_mov_b32 s15, s11
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s8, s0
; VI-NEXT:    s_mov_b32 s9, s1
; VI-NEXT:    s_mov_b32 s0, s2
; VI-NEXT:    s_mov_b32 s1, s3
; VI-NEXT:    s_mov_b32 s2, s10
; VI-NEXT:    s_mov_b32 s3, s11
; VI-NEXT:    s_mov_b32 s12, s6
; VI-NEXT:    s_mov_b32 s13, s7
; VI-NEXT:    s_mov_b32 s6, s10
; VI-NEXT:    s_mov_b32 s7, s11
; VI-NEXT:    buffer_load_dword v0, off, s[0:3], 0
; VI-NEXT:    buffer_load_dword v1, off, s[4:7], 0
; VI-NEXT:    buffer_load_dword v2, off, s[12:15], 0
; VI-NEXT:    s_movk_i32 s0, 0x3900
; VI-NEXT:    s_waitcnt vmcnt(2)
; VI-NEXT:    v_lshrrev_b32_e32 v3, 16, v0
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, 0.5, v0
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; VI-NEXT:    v_lshrrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_lshrrev_b32_e32 v2, 16, v2
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, s0, v3
; VI-NEXT:    v_cndmask_b32_e32 v1, v2, v1, vcc
; VI-NEXT:    v_lshlrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_or_b32_sdwa v0, v0, v1 dst_sel:DWORD dst_unused:UNUSED_PAD src0_sel:WORD_0 src1_sel:DWORD
; VI-NEXT:    buffer_store_dword v0, off, s[8:11], 0
; VI-NEXT:    s_endpgm
    <2 x half> addrspace(1)* %r,
    <2 x half> addrspace(1)* %b,
    <2 x half> addrspace(1)* %c,
    <2 x half> addrspace(1)* %d) {
entry:
  %b.val = load <2 x half>, <2 x half> addrspace(1)* %b
  %c.val = load <2 x half>, <2 x half> addrspace(1)* %c
  %d.val = load <2 x half>, <2 x half> addrspace(1)* %d
  %fcmp = fcmp olt <2 x half> <half 0xH3800, half 0xH3900>, %b.val
  %r.val = select <2 x i1> %fcmp, <2 x half> %c.val, <2 x half> %d.val
  store <2 x half> %r.val, <2 x half> addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_v2f16_imm_b(
; SI-LABEL: select_v2f16_imm_b:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s11, 0xf000
; SI-NEXT:    s_mov_b32 s10, -1
; SI-NEXT:    s_mov_b32 s18, s10
; SI-NEXT:    s_mov_b32 s19, s11
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s16, s2
; SI-NEXT:    s_mov_b32 s17, s3
; SI-NEXT:    s_mov_b32 s12, s6
; SI-NEXT:    s_mov_b32 s13, s7
; SI-NEXT:    s_mov_b32 s14, s10
; SI-NEXT:    s_mov_b32 s15, s11
; SI-NEXT:    s_mov_b32 s6, s10
; SI-NEXT:    s_mov_b32 s7, s11
; SI-NEXT:    buffer_load_dword v0, off, s[16:19], 0
; SI-NEXT:    buffer_load_dword v1, off, s[4:7], 0
; SI-NEXT:    buffer_load_dword v2, off, s[12:15], 0
; SI-NEXT:    s_mov_b32 s2, 0x3f200000
; SI-NEXT:    s_mov_b32 s8, s0
; SI-NEXT:    s_mov_b32 s9, s1
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_lshrrev_b32_e32 v3, 16, v0
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_lshrrev_b32_e32 v4, 16, v1
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_lshrrev_b32_e32 v5, 16, v2
; SI-NEXT:    v_cvt_f32_f16_e32 v3, v3
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    v_cvt_f32_f16_e32 v4, v4
; SI-NEXT:    v_cvt_f32_f16_e32 v5, v5
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    v_cvt_f32_f16_e32 v2, v2
; SI-NEXT:    v_cmp_gt_f32_e32 vcc, s2, v3
; SI-NEXT:    v_cndmask_b32_e32 v3, v5, v4, vcc
; SI-NEXT:    v_cmp_gt_f32_e32 vcc, 0.5, v0
; SI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v3, v3
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    v_lshlrev_b32_e32 v1, 16, v3
; SI-NEXT:    v_or_b32_e32 v0, v0, v1
; SI-NEXT:    buffer_store_dword v0, off, s[8:11], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_v2f16_imm_b:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x24
; VI-NEXT:    s_mov_b32 s11, 0xf000
; VI-NEXT:    s_mov_b32 s10, -1
; VI-NEXT:    s_mov_b32 s14, s10
; VI-NEXT:    s_mov_b32 s15, s11
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s8, s0
; VI-NEXT:    s_mov_b32 s9, s1
; VI-NEXT:    s_mov_b32 s0, s2
; VI-NEXT:    s_mov_b32 s1, s3
; VI-NEXT:    s_mov_b32 s2, s10
; VI-NEXT:    s_mov_b32 s3, s11
; VI-NEXT:    s_mov_b32 s12, s6
; VI-NEXT:    s_mov_b32 s13, s7
; VI-NEXT:    s_mov_b32 s6, s10
; VI-NEXT:    s_mov_b32 s7, s11
; VI-NEXT:    buffer_load_dword v0, off, s[0:3], 0
; VI-NEXT:    buffer_load_dword v1, off, s[4:7], 0
; VI-NEXT:    buffer_load_dword v2, off, s[12:15], 0
; VI-NEXT:    s_movk_i32 s0, 0x3900
; VI-NEXT:    s_waitcnt vmcnt(2)
; VI-NEXT:    v_lshrrev_b32_e32 v3, 16, v0
; VI-NEXT:    v_cmp_gt_f16_e32 vcc, 0.5, v0
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; VI-NEXT:    v_lshrrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_lshrrev_b32_e32 v2, 16, v2
; VI-NEXT:    v_cmp_gt_f16_e32 vcc, s0, v3
; VI-NEXT:    v_cndmask_b32_e32 v1, v2, v1, vcc
; VI-NEXT:    v_lshlrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_or_b32_sdwa v0, v0, v1 dst_sel:DWORD dst_unused:UNUSED_PAD src0_sel:WORD_0 src1_sel:DWORD
; VI-NEXT:    buffer_store_dword v0, off, s[8:11], 0
; VI-NEXT:    s_endpgm
    <2 x half> addrspace(1)* %r,
    <2 x half> addrspace(1)* %a,
    <2 x half> addrspace(1)* %c,
    <2 x half> addrspace(1)* %d) {
entry:
  %a.val = load <2 x half>, <2 x half> addrspace(1)* %a
  %c.val = load <2 x half>, <2 x half> addrspace(1)* %c
  %d.val = load <2 x half>, <2 x half> addrspace(1)* %d
  %fcmp = fcmp olt <2 x half> %a.val, <half 0xH3800, half 0xH3900>
  %r.val = select <2 x i1> %fcmp, <2 x half> %c.val, <2 x half> %d.val
  store <2 x half> %r.val, <2 x half> addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_v2f16_imm_c(
; SI-LABEL: select_v2f16_imm_c:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s11, 0xf000
; SI-NEXT:    s_mov_b32 s10, -1
; SI-NEXT:    s_mov_b32 s18, s10
; SI-NEXT:    s_mov_b32 s19, s11
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s12, s6
; SI-NEXT:    s_mov_b32 s13, s7
; SI-NEXT:    s_mov_b32 s6, s10
; SI-NEXT:    s_mov_b32 s7, s11
; SI-NEXT:    s_mov_b32 s16, s2
; SI-NEXT:    s_mov_b32 s17, s3
; SI-NEXT:    buffer_load_dword v3, off, s[4:7], 0
; SI-NEXT:    s_mov_b32 s14, s10
; SI-NEXT:    s_mov_b32 s15, s11
; SI-NEXT:    buffer_load_dword v0, off, s[16:19], 0
; SI-NEXT:    buffer_load_dword v1, off, s[12:15], 0
; SI-NEXT:    v_mov_b32_e32 v2, 0x3f200000
; SI-NEXT:    s_mov_b32 s8, s0
; SI-NEXT:    s_mov_b32 s9, s1
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_lshrrev_b32_e32 v5, 16, v3
; SI-NEXT:    v_cvt_f32_f16_e32 v5, v5
; SI-NEXT:    v_cvt_f32_f16_e32 v3, v3
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_cvt_f32_f16_e32 v4, v0
; SI-NEXT:    v_lshrrev_b32_e32 v0, 16, v0
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_lshrrev_b32_e32 v6, 16, v1
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    v_cvt_f32_f16_e32 v6, v6
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    v_cmp_nlt_f32_e32 vcc, v0, v5
; SI-NEXT:    v_cndmask_b32_e32 v0, v2, v6, vcc
; SI-NEXT:    v_cmp_nlt_f32_e32 vcc, v4, v3
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    v_cndmask_b32_e32 v1, 0.5, v1, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v1, v1
; SI-NEXT:    v_lshlrev_b32_e32 v0, 16, v0
; SI-NEXT:    v_or_b32_e32 v0, v1, v0
; SI-NEXT:    buffer_store_dword v0, off, s[8:11], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_v2f16_imm_c:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x24
; VI-NEXT:    s_mov_b32 s11, 0xf000
; VI-NEXT:    s_mov_b32 s10, -1
; VI-NEXT:    s_mov_b32 s14, s10
; VI-NEXT:    s_mov_b32 s15, s11
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s8, s0
; VI-NEXT:    s_mov_b32 s9, s1
; VI-NEXT:    s_mov_b32 s12, s6
; VI-NEXT:    s_mov_b32 s13, s7
; VI-NEXT:    s_mov_b32 s0, s2
; VI-NEXT:    s_mov_b32 s1, s3
; VI-NEXT:    s_mov_b32 s6, s10
; VI-NEXT:    s_mov_b32 s7, s11
; VI-NEXT:    s_mov_b32 s2, s10
; VI-NEXT:    s_mov_b32 s3, s11
; VI-NEXT:    buffer_load_dword v0, off, s[0:3], 0
; VI-NEXT:    buffer_load_dword v4, off, s[4:7], 0
; VI-NEXT:    buffer_load_dword v1, off, s[12:15], 0
; VI-NEXT:    v_mov_b32_e32 v2, 0x3800
; VI-NEXT:    v_mov_b32_e32 v3, 0x3900
; VI-NEXT:    s_waitcnt vmcnt(2)
; VI-NEXT:    v_lshrrev_b32_e32 v6, 16, v0
; VI-NEXT:    s_waitcnt vmcnt(1)
; VI-NEXT:    v_cmp_nlt_f16_e32 vcc, v0, v4
; VI-NEXT:    v_lshrrev_b32_e32 v5, 16, v4
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; VI-NEXT:    v_lshrrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_cmp_nlt_f16_e32 vcc, v6, v5
; VI-NEXT:    v_cndmask_b32_e32 v1, v3, v1, vcc
; VI-NEXT:    v_lshlrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_or_b32_sdwa v0, v0, v1 dst_sel:DWORD dst_unused:UNUSED_PAD src0_sel:WORD_0 src1_sel:DWORD
; VI-NEXT:    buffer_store_dword v0, off, s[8:11], 0
; VI-NEXT:    s_endpgm
    <2 x half> addrspace(1)* %r,
    <2 x half> addrspace(1)* %a,
    <2 x half> addrspace(1)* %b,
    <2 x half> addrspace(1)* %d) {
entry:
  %a.val = load <2 x half>, <2 x half> addrspace(1)* %a
  %b.val = load <2 x half>, <2 x half> addrspace(1)* %b
  %d.val = load <2 x half>, <2 x half> addrspace(1)* %d
  %fcmp = fcmp olt <2 x half> %a.val, %b.val
  %r.val = select <2 x i1> %fcmp, <2 x half> <half 0xH3800, half 0xH3900>, <2 x half> %d.val
  store <2 x half> %r.val, <2 x half> addrspace(1)* %r
  ret void
}

define amdgpu_kernel void @select_v2f16_imm_d(
; SI-LABEL: select_v2f16_imm_d:
; SI:       ; %bb.0: ; %entry
; SI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x9
; SI-NEXT:    s_mov_b32 s11, 0xf000
; SI-NEXT:    s_mov_b32 s10, -1
; SI-NEXT:    s_mov_b32 s18, s10
; SI-NEXT:    s_mov_b32 s19, s11
; SI-NEXT:    s_waitcnt lgkmcnt(0)
; SI-NEXT:    s_mov_b32 s12, s6
; SI-NEXT:    s_mov_b32 s13, s7
; SI-NEXT:    s_mov_b32 s6, s10
; SI-NEXT:    s_mov_b32 s7, s11
; SI-NEXT:    s_mov_b32 s16, s2
; SI-NEXT:    s_mov_b32 s17, s3
; SI-NEXT:    buffer_load_dword v3, off, s[4:7], 0
; SI-NEXT:    s_mov_b32 s14, s10
; SI-NEXT:    s_mov_b32 s15, s11
; SI-NEXT:    buffer_load_dword v0, off, s[16:19], 0
; SI-NEXT:    buffer_load_dword v1, off, s[12:15], 0
; SI-NEXT:    v_mov_b32_e32 v2, 0x3f200000
; SI-NEXT:    s_mov_b32 s8, s0
; SI-NEXT:    s_mov_b32 s9, s1
; SI-NEXT:    s_waitcnt vmcnt(2)
; SI-NEXT:    v_lshrrev_b32_e32 v5, 16, v3
; SI-NEXT:    v_cvt_f32_f16_e32 v5, v5
; SI-NEXT:    v_cvt_f32_f16_e32 v3, v3
; SI-NEXT:    s_waitcnt vmcnt(1)
; SI-NEXT:    v_lshrrev_b32_e32 v4, 16, v0
; SI-NEXT:    s_waitcnt vmcnt(0)
; SI-NEXT:    v_lshrrev_b32_e32 v6, 16, v1
; SI-NEXT:    v_cvt_f32_f16_e32 v4, v4
; SI-NEXT:    v_cvt_f32_f16_e32 v0, v0
; SI-NEXT:    v_cvt_f32_f16_e32 v6, v6
; SI-NEXT:    v_cvt_f32_f16_e32 v1, v1
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, v4, v5
; SI-NEXT:    v_cndmask_b32_e32 v2, v2, v6, vcc
; SI-NEXT:    v_cmp_lt_f32_e32 vcc, v0, v3
; SI-NEXT:    v_cndmask_b32_e32 v0, 0.5, v1, vcc
; SI-NEXT:    v_cvt_f16_f32_e32 v2, v2
; SI-NEXT:    v_cvt_f16_f32_e32 v0, v0
; SI-NEXT:    v_lshlrev_b32_e32 v1, 16, v2
; SI-NEXT:    v_or_b32_e32 v0, v0, v1
; SI-NEXT:    buffer_store_dword v0, off, s[8:11], 0
; SI-NEXT:    s_endpgm
;
; VI-LABEL: select_v2f16_imm_d:
; VI:       ; %bb.0: ; %entry
; VI-NEXT:    s_load_dwordx8 s[0:7], s[0:1], 0x24
; VI-NEXT:    s_mov_b32 s11, 0xf000
; VI-NEXT:    s_mov_b32 s10, -1
; VI-NEXT:    s_mov_b32 s14, s10
; VI-NEXT:    s_mov_b32 s15, s11
; VI-NEXT:    s_waitcnt lgkmcnt(0)
; VI-NEXT:    s_mov_b32 s8, s0
; VI-NEXT:    s_mov_b32 s9, s1
; VI-NEXT:    s_mov_b32 s12, s6
; VI-NEXT:    s_mov_b32 s13, s7
; VI-NEXT:    s_mov_b32 s0, s2
; VI-NEXT:    s_mov_b32 s1, s3
; VI-NEXT:    s_mov_b32 s6, s10
; VI-NEXT:    s_mov_b32 s7, s11
; VI-NEXT:    s_mov_b32 s2, s10
; VI-NEXT:    s_mov_b32 s3, s11
; VI-NEXT:    buffer_load_dword v0, off, s[0:3], 0
; VI-NEXT:    buffer_load_dword v4, off, s[4:7], 0
; VI-NEXT:    buffer_load_dword v1, off, s[12:15], 0
; VI-NEXT:    v_mov_b32_e32 v2, 0x3800
; VI-NEXT:    v_mov_b32_e32 v3, 0x3900
; VI-NEXT:    s_waitcnt vmcnt(2)
; VI-NEXT:    v_lshrrev_b32_e32 v6, 16, v0
; VI-NEXT:    s_waitcnt vmcnt(1)
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, v0, v4
; VI-NEXT:    v_lshrrev_b32_e32 v5, 16, v4
; VI-NEXT:    s_waitcnt vmcnt(0)
; VI-NEXT:    v_cndmask_b32_e32 v0, v2, v1, vcc
; VI-NEXT:    v_lshrrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_cmp_lt_f16_e32 vcc, v6, v5
; VI-NEXT:    v_cndmask_b32_e32 v1, v3, v1, vcc
; VI-NEXT:    v_lshlrev_b32_e32 v1, 16, v1
; VI-NEXT:    v_or_b32_sdwa v0, v0, v1 dst_sel:DWORD dst_unused:UNUSED_PAD src0_sel:WORD_0 src1_sel:DWORD
; VI-NEXT:    buffer_store_dword v0, off, s[8:11], 0
; VI-NEXT:    s_endpgm
    <2 x half> addrspace(1)* %r,
    <2 x half> addrspace(1)* %a,
    <2 x half> addrspace(1)* %b,
    <2 x half> addrspace(1)* %c) {
entry:
  %a.val = load <2 x half>, <2 x half> addrspace(1)* %a
  %b.val = load <2 x half>, <2 x half> addrspace(1)* %b
  %c.val = load <2 x half>, <2 x half> addrspace(1)* %c
  %fcmp = fcmp olt <2 x half> %a.val, %b.val
  %r.val = select <2 x i1> %fcmp, <2 x half> %c.val, <2 x half> <half 0xH3800, half 0xH3900>
  store <2 x half> %r.val, <2 x half> addrspace(1)* %r
  ret void
}