#ifndef LLVM_TYPEGRAPHS_ASMWRITERAARCH64_H
#define LLVM_TYPEGRAPHS_ASMWRITERAARCH64_H

#include <vector>
#include <cstdint>
#include <limits>

/**
 * ID comes in r16, r17 is scratch register.
 * Calling convention (args): x0, x1, x2, ..., x7
 * x29 = sp, x30 = ra
 */
struct AsmWriter {
  AssemblyBuilder &Builder;

  std::vector<JmpTarget> ErrorCases;
  std::vector<size_t> ErrorCasesAbsolute;
  std::vector<JmpTarget> LazyDispatcherTailJumps;

  void errorCasesJumpHere() {
    for (auto &J: ErrorCases)
      J.setTarget(Builder.currentOffset());
    for (auto &I: ErrorCasesAbsolute) {
      Builder.write64At(I, (uintptr_t) Builder.currentPos());
    }
  }

  void writeDebuggerTrap() {
    // d4200000        brk     #0x0
    Builder.write((uint32_t) 0xd4200000);
  }

  JmpTarget writeBNE() {
    size_t Offset = Builder.currentOffset();
    Builder.write32Mask(0x54000001, ~ARM_CONDITIONAL_BRANCH_IMM_MASK);
    return JmpTarget(Builder, Offset, ARM_CONDITIONAL_BRANCH_IMM);
  }

  JmpTarget writeBranchHi() {
    // x86: ja
    size_t Offset = Builder.currentOffset();
    // 54000008        b.hi    0x0  // b.pmore
    Builder.write32Mask(0x54000008, ~ARM_CONDITIONAL_BRANCH_IMM_MASK);
    return JmpTarget(Builder, Offset, ARM_CONDITIONAL_BRANCH_IMM);
  }

  JmpTarget writeBranchHs() {
    // x86: jae
    size_t Offset = Builder.currentOffset();
    // 54000002        b.cs    0x0  // b.hs, b.nlast
    Builder.write32Mask(0x54000002, ~ARM_CONDITIONAL_BRANCH_IMM_MASK);
    return JmpTarget(Builder, Offset, ARM_CONDITIONAL_BRANCH_IMM);
  }

  JmpTarget writeBranch() {
    size_t Offset = Builder.currentOffset();
    Builder.write32Mask(0x14000000, ~ARM_BRANCH_IMM_MASK);
    return JmpTarget(Builder, Offset, ARM_BRANCH_IMM);
  }

  void writeLongJmp(void *TargetPtr) {
    writeConstantToX17((uintptr_t) TargetPtr);
    // d61f0220        br      x17
    Builder.write32(0xd61f0220);
  }

  void writeCmpX16(uint64_t C) {
    if (C < 0xfff) {
      // f100021f        cmp     x16, #0x0
      Builder.write32(0xf100021f | (C << 10));
    } else {
      writeConstantToX17(C);
      // eb11021f        cmp     x16, x17
      Builder.write(0xeb11021f);
    }
  }

  void writeSubX16(uint64_t C) {
    if (!C)
      return;
    if (C > 0xffffff) {
      writeConstantToX17(C);
      // cb110210        sub     x16, x16, x17
      Builder.write32(0xcb110210);
      return;
    }
    if (C & 0xfff) {
      // d1000210        sub     x16, x16, #0x0
      Builder.write32(0xd1000210 | ((C & 0xfff) << 10));
    }
    C >>= 12;
    if (C & 0xfff) {
      // d1400210        sub     x16, x16, #0x0, lsl #12
      Builder.write32(0xd1400210 | ((C & 0xfff) << 10));
    }
  }

  void writeAddX16(uint64_t C) {
    if (!C)
      return;
    if (C > 0xffffff) {
      writeConstantToX17(C);
      // 8b110210        add     x16, x16, x17
      Builder.write32(0x8b110210);
      return;
    }
    if (C & 0xfff) {
      // 91000210        add     x16, x16, #0x0
      Builder.write32(0x91000210 | ((C & 0xfff) << 10));
    }
    C >>= 12;
    if (C & 0xfff) {
      // 91400210        add     x16, x16, #0x0, lsl #12
      Builder.write32(0x91400210 | ((C & 0xfff) << 10));
    }
  }

  JmpTarget writeJumpPCRelativeAddress() {
    size_t Offset = Builder.currentOffset();
    // 10000071        adr     x17, 0xc
    Builder.write32Mask(0x10000011, ~ARM_CONDITIONAL_BRANCH_IMM_MASK);
    // f8707a31        ldr     x17, [x17, x16, lsl #3]
    Builder.write32(0xf8707a31);
    // d61f0220        br      x17
    Builder.write32(0xd61f0220);
    return JmpTarget(Builder, Offset, ARM_CONDITIONAL_BRANCH_IMM);
  }

  void writeMovX16ToX1() {
    // aa1003e1        mov     x1, x16
    Builder.write32(0xaa1003e1);
  }

  void writeConstantToX0(uintptr_t X0) {
    // mov X0&0xffff
    Builder.write32(0xd2800000u | ((X0 & 0xffff) << 5));
    X0 >>= 16;
    if (X0) {
      // movk X0, <C>, lsl 16
      Builder.write32(0xf2a00000u | ((X0 & 0xffff) << 5));
      X0 >>= 16;
      if (X0) {
        // movk X0, <C>, lsl 32
        Builder.write32(0xf2c00000u | ((X0 & 0xffff) << 5));
        X0 >>= 16;
        if (X0) {
          // movk X0, <C>, lsl 48
          Builder.write32(0xf2e00000u | ((X0 & 0xffff) << 5));
        }
      }
    }
  }

  void writeConstantToX17(uintptr_t X17) {
    // mov X17&0xffff
    Builder.write32(0xd2800011u | ((X17 & 0xffff) << 5));
    X17 >>= 16;
    if (X17) {
      // movk X17, <C>, lsl 16
      Builder.write32(0xf2a00011u | ((X17 & 0xffff) << 5));
      X17 >>= 16;
      if (X17) {
        // movk X17, <C>, lsl 32
        Builder.write32(0xf2c00011u | ((X17 & 0xffff) << 5));
        X17 >>= 16;
        if (X17) {
          // movk X17, <C>, lsl 48
          Builder.write32(0xf2e00011u | ((X17 & 0xffff) << 5));
        }
      }
    }
  }

  void writeCall(uint64_t Target) {
    writeConstantToX17(Target);
    // d63f0220        blr     x17
    Builder.write32(0xd63f0220);
  }

  void writeCrash() {
    // brk 1
    Builder.write((uint32_t) 0xd4200020);
  }

  void genIfThenElseChain(const std::vector<FunctionInfos, ProtectedAllocator<FunctionInfos>> &Targets, FunctionID PreferredID) {
    auto PreferredFunc = std::find(Targets.begin(), Targets.end(), PreferredID);
    if (PreferredFunc != Targets.end()) {
      writeCmpX16(PreferredID);
      auto Jmp = writeBNE();
      writeLongJmp(PreferredFunc->Address);
      Jmp.setTarget(Builder.currentOffset());
    }

    for (auto &Target: Targets) {
      if (Target.ID != PreferredID) {
        writeCmpX16(Target.ID);
        auto Jmp = writeBNE();
        writeLongJmp(Target.Address);
        Jmp.setTarget(Builder.currentOffset());
      }
    }
  }

  /**
   * Guarantees: handles everything in Targets[StartIndex:EndIndex]
   * and has dedicated jump to errors for everything <= Targets[EndIndex-1].ID (if at least 3 targets given).
   * Execution continues only if r11 > Targets[EndIndex-1].ID
   *
   * @param Targets List of possible functions
   * @param StartIndex Subset start of functions to handle
   * @param EndIndex Subset end of functions to handle
   * @param Min guaranteed minimum value of the ID
   * @param Max guaranteed maximum value of the ID
   * @param X16HasLastBeenComparedTo If the last instruction before this tree was a "cmp r11, <const>", pass this
   * constant. Saves one instruction.
   */
  void genTree(const std::vector<FunctionInfos, ProtectedAllocator<FunctionInfos>> &Targets, size_t StartIndex, size_t EndIndex, FunctionID Min, FunctionID Max,
               FunctionID X16HasLastBeenComparedTo = -1) {
    // only 1 target
    if (EndIndex - StartIndex == 1) {
      if (Min != Max) {
        if (X16HasLastBeenComparedTo != Targets[StartIndex].ID)
          writeCmpX16(Targets[StartIndex].ID);
        auto Jmp = writeBNE();
        writeLongJmp(Targets[StartIndex].Address);
        Jmp.setTarget(Builder.currentOffset());
      } else {
        writeLongJmp(Targets[StartIndex].Address);
      }
      return;
    }
    // only 2 targets
    if (EndIndex - StartIndex == 2) {
      if (X16HasLastBeenComparedTo != Targets[StartIndex].ID)
        writeCmpX16(Targets[StartIndex].ID);
      auto Jmp = writeBNE();
      writeLongJmp(Targets[StartIndex].Address);
      Jmp.setTarget(Builder.currentOffset());
      if (Min == Targets[StartIndex].ID) Min++;
      genTree(Targets, StartIndex+1, EndIndex, Min, Max);
      return;
    }
    // >2 targets
    size_t PivotIndex = (StartIndex + 1 + EndIndex) / 2;
    writeCmpX16(Targets[PivotIndex].ID);
    auto Jmp = writeBranchHs();
    genTree(Targets, StartIndex, PivotIndex, Min, Targets[PivotIndex].ID - 1);
    if (PivotIndex - StartIndex < Targets[PivotIndex].ID - Min) {
      // error case
      ErrorCases.push_back(writeBranch());
    }
    Jmp.setTarget(Builder.currentOffset());
    genTree(Targets, PivotIndex, EndIndex, Targets[PivotIndex].ID, Max, Targets[PivotIndex].ID);
  }

  size_t genJumptable(const std::vector<FunctionInfos, ProtectedAllocator<FunctionInfos>> &Targets, size_t StartIndex, size_t EndIndex, FunctionID Min, FunctionID Max) {
    size_t Offset = 0;
    if (Targets[StartIndex].ID - Min >= 4) {
      Offset = Targets[StartIndex].ID;
      Min = 0;
      Max -= Offset;
    }
    writeSubX16(Offset);
    JmpTarget ElseTarget{Builder, 0, 0};
    if (Max != Targets[EndIndex - 1].ID - Offset) {
      writeCmpX16(Targets[EndIndex - 1].ID - Offset);
      ElseTarget = writeBranchHi();
      Max = Targets[EndIndex - 1].ID - Offset;
    }

    auto JmpTableEntries = writeJumpPCRelativeAddress();
    JmpTableEntries.setTarget(Builder.currentOffset() - sizeof(uint64_t) * Min);
    size_t I = StartIndex;
    for (size_t ID = Min; ID <= Max; ID++) {
      while (ID > Targets[I].ID - Offset) I++;
      if (ID == Targets[I].ID - Offset) {
        Builder.write((uintptr_t) Targets[I].Address);
        I++;
      } else {
        ErrorCasesAbsolute.push_back(Builder.currentOffset());
        Builder.write("\x00\x00\x00\x00\x00\x00\x00\x00", 8, 0);
      }
    }

    ElseTarget.setTarget(Builder.currentOffset());
    return Offset;
  }

  void genSwitch(const std::vector<FunctionInfos, ProtectedAllocator<FunctionInfos>> &Targets, size_t StartIndex, size_t EndIndex, FunctionID PreferredID) {
    if (EndIndex - StartIndex <= 4) {
      genIfThenElseChain(Targets, PreferredID);
    } else {
      size_t NumTargets = EndIndex - StartIndex;
      size_t NumIds = Targets[EndIndex-1].ID - Targets[StartIndex].ID + 1;
      if (NumTargets > 8 && (NumIds - NumTargets <= 4 || NumTargets*1.5 >= NumIds)) {
        // fprintf(stderr, "Building jumptable with %ld targets in %ld IDs\n", NumTargets, NumIds);
        size_t Offset = genJumptable(Targets, StartIndex, EndIndex, 0, std::numeric_limits<FunctionID>::max());
        writeAddX16(Offset); // for error reporting
        // fprintf(stderr, " => offset=%ld\n", Offset);
      } else {
        genTree(Targets, StartIndex, EndIndex, 0, std::numeric_limits<FunctionID>::max());
      }
    }
  }

  void genSwitch(std::vector<FunctionInfos, ProtectedAllocator<FunctionInfos>> &Targets, FunctionID PreferredID) {
    std::sort(Targets.begin(), Targets.end());
    genSwitch(Targets, 0, Targets.size(), PreferredID);
  }

  void genLazyDispatcher(int Index) {
    // total: 12 bytes, padded to 16 bytes
    size_t Offset = Builder.currentOffset();
    // stp x0,x1, [sp, -16]! ; mov x0, Index
    // a9bf07e0        stp     x0, x1, [sp, #-16]!
    Builder.write32(0xa9bf07e0);
    writeConstantToX0(Index);
    // b <tail>
    LazyDispatcherTailJumps.push_back(writeBranch());
    if (Builder.currentOffset() < Offset + 16)
      Builder.write32(0xd503201f);  // nop
  }

  void genLazyDispatcherTail() {
    // total: 24+20+32 bytes = 76
    // all heads jump here:
    for (auto &J: LazyDispatcherTailJumps) {
      J.setTarget(Builder.currentOffset());
    }
    LazyDispatcherTailJumps.clear();

    // The stack pointer sp must always be kept aligned to 16 bytes
    // stp x29,x30, [sp, -16]! ; stp x2,x3, [sp, -16]! ; stp x4,x5, [sp, -16]! ; stp x6,x7, [sp, -16]! ; stp x16,x8, [sp, -16]!
    Builder.write("\xfd\x7b\xbf\xa9\xe2\x0f\xbf\xa9\xe4\x17\xbf\xa9\xe6\x1f\xbf\xa9\xf0\x23\xbf\xa9", 20);
    // mov rsi, r11 ; call generateDispatcher
    writeMovX16ToX1();
    writeCall((uintptr_t) &generateDispatcher);
    // mov x17, x0 ; ldp x16,x8, [sp], 16 ; ldp x6,x7, [sp], 16 ; ldp x4,x5, [sp], 16 ; ldp x2,x3, [sp], 16 ; ldp x29,x30, [sp], 16 ; ldp x0,x1, [sp], 16 ; br x17
    Builder.write("\xf1\x03\x00\xaa\xf0\x23\xc1\xa8\xe6\x1f\xc1\xa8\xe4\x17\xc1\xa8\xe2\x0f\xc1\xa8\xfd\x7b\xc1\xa8\xe0\x07\xc1\xa8\x20\x02\x1f\xd6", 32);
  }

  inline size_t getLazyDispatcherEstimatedBytes(size_t Count) {
    return 16 * Count + 76;
  }

  void genInvalidIdDebugTrap(int Index) {
    writeMovX16ToX1();
    writeConstantToX0(Index);
    writeCall((uintptr_t) &__tg_dynamic_error);
  }
};

#endif // LLVM_TYPEGRAPHS_ASMWRITERAARCH64_H
