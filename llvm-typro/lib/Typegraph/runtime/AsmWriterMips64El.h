#ifndef LLVM_TYPEGRAPHS_ASMWRITERMIPS64EL_H
#define LLVM_TYPEGRAPHS_ASMWRITERMIPS64EL_H

#include <vector>
#include <cstdint>

/**
 * my register: >>t6<< =$14, t4-t7 ($12-$15)
 * scratch register: >>t7<< =$15 (t3 in pwntools)
 * argument registers: $4-$11 (a0-a7)
 * other registers to store on stack: a0-a7, t6, ra
 * stack alignment: 16bytes
 * return value: v0/v1 ($2/$3)
 *
 * https://techpubs.jurassic.nl/manuals/0640/developer/Mpro_n32_ABI/sgi_html/ch02.html
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
      *((char**) Builder.byOffset(I)) = Builder.currentPos();
    }
  }

  void writeDebuggerTrap() {
    // 7000003f        sdbbp
    // Builder.write((uint32_t) 0x7000003f);
    Builder.write((uint32_t) 0x0000000d);
  }

  JmpTarget writeBNE(uintptr_t C) {
    writeConstantToAT(C);
    // 15c10001        bne     t2, at, 0x20
    Builder.write32(0x15c10000);
    size_t Offset = Builder.currentOffset();
    Builder.write32(0x0);  // https://en.wikipedia.org/wiki/Delay_slot
    return JmpTarget(Builder, Offset, MIPS_BRANCH_REL);
  }

  JmpTarget writeBGTU(uintptr_t C) {
    // x86: ja
    return writeBranchBGEU(C + 1);
  }

  JmpTarget writeBranchBGEU(uintptr_t C) {
    // x86: jae
    if (C > 0x7fff || true) {
      writeConstantToAT(C);
      //   8:   01c1082b        sltu    at, t2, at
      //   c:   10200001        beqz    at, 0x14
      Builder.write32(0x01c1082b);
    } else {
      //   0:   2dc17abc        sltiu   at, t2, 31420
      //   4:   10200001        beqz    at, 0xc
      Builder.write32(0x2dc10000 | C);
    }
    Builder.write32(0x10200000);
    size_t Offset = Builder.currentOffset();
    Builder.write32(0x0); // nop
    return JmpTarget(Builder, Offset, MIPS_BRANCH_REL);
  }

  JmpTarget writeJump(bool WithDelaySlot = true) {
    // 08000000        j       0x0
    Builder.write32(0x08000000);
    size_t Offset = Builder.currentOffset();
    if (WithDelaySlot) {
      Builder.write32(0x0); // nop
    }
    return JmpTarget(Builder, Offset, MIPS_BRANCH_ABS);
  }

  void writeLongJmp(void *TargetPtr) {
    writeConstantToT9((uintptr_t) TargetPtr);
    // 03200008        jr      t9
    Builder.write32(0x03200008);
    Builder.write32(0x0); // delay slot
  }

  void writeSubT6(uint64_t C) {
    if (C < 0x8000) {
      // 65ce8000        daddiu  t2, t2, -0x8000
      Builder.write32(0x65ce0000 | ((-C) & 0xffff));
    } else {
      // 01cf702f        dsubu   t2, t2, t3
      writeConstantToT7(C);
      Builder.write32(0x01cf702f);
    }
  }

  void writeAddT6(uint64_t C) {
    if (-0x8000 <= (int64_t) C && C <= 0x7fff) {
      // 65ce8000        daddiu  t2, t2, -0x8000
      Builder.write32(0x65ce0000 | (C & 0xffff));
    } else {
      // 01cf702d        daddu   t2, t2, t3
      writeConstantToT7(C);
      Builder.write32(0x01cf702d);
    }
  }

  JmpTarget writeJumpPCRelativeAddress(uintptr_t DistanceAfterJmp) {
    // load t9 + (t2<<3) + X => t9
    // tailcall t9

    ssize_t ExpectedJumpOffset = Builder.currentOffset() - Builder.currentFunctionStartOffset() + 20 + DistanceAfterJmp;
    if (-0x8000 <= ExpectedJumpOffset && ExpectedJumpOffset < 0x7fff) {
      //   0:   000e78f8        dsll    t3, t2, 0x3
      //   4:   01f9782d        daddu   t3, t3, t9
      //   8:   ddf90000        ld      t9, 0(t3)
      //   c:   03200008        jr      t9
      //  10:   00000000        nop
      Builder.write32(0x000e78f8);
      Builder.write32(0x01f9782d);
      Builder.write32(0xddf90000);
      size_t Offset = Builder.currentOffset();
      Builder.write32(0x03200008);
      Builder.write32(0x0);
      return JmpTarget(Builder, Offset, MIPS_BRANCH_MEMREL16);
    } else {
      assert(false && "Not implemented!");
    }
  }

  void writeMovT6ToA1() {
    // 01c02825        move    a1, t2
    Builder.write32(0x01c02825);
  }

  void writeConstantToA0(uintptr_t A0) {
    if (A0 > 0x7fffffff) {
      //   0:   3c041234        lui     a0, 0x1234
      //   4:   34845678        ori     a0, a0, 0x5678
      //   8:   00042438        dsll    a0, a0, 0x10
      //   c:   348490ab        ori     a0, a0, 0x90ab
      //  10:   00042438        dsll    a0, a0, 0x10
      //  14:   3484cdef        ori     a0, a0, 0xcdef
      Builder.write32(0x3c040000 | (A0 >> 48));
      Builder.write32(0x34840000 | ((A0 >> 32) & 0xffff));
      Builder.write32(0x00042438);
      Builder.write32(0x34840000 | ((A0 >> 16) & 0xffff));
      Builder.write32(0x00042438);
      Builder.write32(0x34840000 | (A0 & 0xffff));
    } else if (A0 > 0xffff) {
      //   0:   3c041234        lui     a0, 0x1234
      //   4:   34845678        ori     a0, a0, 0x5678
      Builder.write32(0x3c040000 | (A0 >> 16));
      Builder.write32(0x34840000 | (A0 & 0xffff));
    } else {
      // 34040000        li      a0, 0
      Builder.write32(0x34040000 | A0);
    }
  }

  void writeConstantToT7(uintptr_t T7) {
    if (T7 > 0x7fffffff) {
      //   0:   3c0f1234        lui     t3, 0x1234
      //   4:   35ef5678        ori     t3, t3, 0x5678
      //   8:   000f7c38        dsll    t3, t3, 0x10
      //   c:   35ef90ab        ori     t3, t3, 0x90ab
      //  10:   000f7c38        dsll    t3, t3, 0x10
      //  14:   35efcdef        ori     t3, t3, 0xcdef
      Builder.write32(0x3c0f0000 | (T7 >> 48));
      Builder.write32(0x35ef0000 | ((T7 >> 32) & 0xffff));
      Builder.write32(0x000f7c38);
      Builder.write32(0x35ef0000 | ((T7 >> 16) & 0xffff));
      Builder.write32(0x000f7c38);
      Builder.write32(0x35ef0000 | (T7 & 0xffff));
    } else if (T7 > 0xffff) {
      //   0:   3c0f1234        lui     t3, 0x1234
      //   4:   35ef5678        ori     t3, t3, 0x5678
      Builder.write32(0x3c0f0000 | (T7 >> 16));
      Builder.write32(0x35ef0000 | (T7 & 0xffff));
    } else {
      // 340f1234        li      t3, 0x1234
      Builder.write32(0x340f0000 | T7);
    }
  }

  /**
   * Register t9 = $25 is usually used in jalr calls. Seems to be part of the convention, but never documented.
   * Seems to be relevant for position-independent code, $t9=start of function.
   * Must not be used except for calls and tailcalls. Must be restored after calls.
   * @param T9
   */
  void writeConstantToT9(uintptr_t T9) {
    if (T9 > 0x7fffffff) {
      //   0:   3c191234        lui     t9, 0x1234
      //   4:   37395678        ori     t9, t9, 0x5678
      // OR:
      //   0:   3419efff        li      t9, 0xefff
      // THEN:
      //   8:   0019cc38        dsll    t9, t9, 0x10
      //   c:   373990ab        ori     t9, t9, 0x90ab
      //  10:   0019cc38        dsll    t9, t9, 0x10
      //  14:   3739cdef        ori     t9, t9, 0xcdef
      if (T9 > 0xffffffffffff) {
        Builder.write32(0x3c190000 | (T9 >> 48));
        Builder.write32(0x37390000 | ((T9 >> 32) & 0xffff));
      } else {
        Builder.write32(0x34190000 | (T9 >> 32));
      }
      Builder.write32(0x0019cc38);
      Builder.write32(0x37390000 | ((T9 >> 16) & 0xffff));
      Builder.write32(0x0019cc38);
      Builder.write32(0x37390000 | (T9 & 0xffff));
    } else if (T9 > 0xffff) {
      //   0:   3c191234        lui     t9, 0x1234
      //   4:   37395678        ori     t9, t9, 0x5678
      Builder.write32(0x3c190000 | (T9 >> 16));
      Builder.write32(0x37390000 | (T9 & 0xffff));
    } else {
      //    0:   34191234        li      t9, 0x1234
      Builder.write32(0x34190000 | T9);
    }
  }

  void writeConstantToAT(uintptr_t AT) {
    if (AT > 0x7fffffff) {
      //   0:   3c011234        lui     at, 0x1234
      //   4:   34215678        ori     at, at, 0x5678
      //   8:   00010c38        dsll    at, at, 0x10
      //   c:   342190ab        ori     at, at, 0x90ab
      //  10:   00010c38        dsll    at, at, 0x10
      //  14:   3421cdef        ori     at, at, 0xcdef
      Builder.write32(0x3c010000 | (AT >> 48));
      Builder.write32(0x34210000 | ((AT >> 32) & 0xffff));
      Builder.write32(0x00010c38);
      Builder.write32(0x34210000 | ((AT >> 16) & 0xffff));
      Builder.write32(0x00010c38);
      Builder.write32(0x34210000 | (AT & 0xffff));
    } else if (AT > 0xffff) {
      //   0:   3c011234        lui     at, 0x1234
      //   4:   34215678        ori     at, at, 0x5678
      Builder.write32(0x3c010000 | (AT >> 16));
      Builder.write32(0x34210000 | (AT & 0xffff));
    } else {
      // 34040000        li      at, 0
      Builder.write32(0x34010000 | AT);
    }
  }

  void writeCall(uint64_t Target) {
    writeConstantToT9(Target);
    // 0320f809        jalr    t9
    Builder.write32(0x0320f809);
    Builder.write32(0x0); // delay slot
  }

  void writeCrash() {
    // 0000000d        break
    Builder.write32(0x0000000d);
  }

  void genIfThenElseChain(std::vector<FunctionInfos> &Targets, FunctionID PreferredID) {
    auto PreferredFunc = std::find(Targets.begin(), Targets.end(), PreferredID);
    if (PreferredFunc != Targets.end()) {
      auto Jmp = writeBNE(PreferredID);
      writeLongJmp(PreferredFunc->Address);
      Jmp.setTarget(Builder.currentOffset());
    }

    for (auto &Target: Targets) {
      if (Target.ID != PreferredID) {
        auto Jmp = writeBNE(Target.ID);
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
  void genTree(std::vector<FunctionInfos> &Targets, size_t StartIndex, size_t EndIndex, FunctionID Min, FunctionID Max) {
    // only 1 target
    if (EndIndex - StartIndex == 1) {
      if (Min != Max) {
        auto Jmp = writeBNE(Targets[StartIndex].ID);
        writeLongJmp(Targets[StartIndex].Address);
        Jmp.setTarget(Builder.currentOffset());
      } else {
        writeLongJmp(Targets[StartIndex].Address);
      }
      return;
    }
    // only 2 targets
    if (EndIndex - StartIndex == 2) {
      auto Jmp = writeBNE(Targets[StartIndex].ID);
      writeLongJmp(Targets[StartIndex].Address);
      Jmp.setTarget(Builder.currentOffset());
      if (Min == Targets[StartIndex].ID) Min++;
      genTree(Targets, StartIndex+1, EndIndex, Min, Max);
      return;
    }
    // >2 targets
    size_t PivotIndex = (StartIndex + 1 + EndIndex) / 2;
    auto Jmp = writeBranchBGEU(Targets[PivotIndex].ID);
    genTree(Targets, StartIndex, PivotIndex, Min, Targets[PivotIndex].ID - 1);
    if (PivotIndex - StartIndex < Targets[PivotIndex].ID - Min) {
      // error case
      ErrorCases.push_back(writeJump());
    }
    Jmp.setTarget(Builder.currentOffset());
    genTree(Targets, PivotIndex, EndIndex, Targets[PivotIndex].ID, Max);
  }

  size_t genJumptable(std::vector<FunctionInfos> &Targets, size_t StartIndex, size_t EndIndex, FunctionID Min, FunctionID Max) {
    size_t Offset = 0;
    if (Targets[StartIndex].ID - Min >= 4) {
      Offset = Targets[StartIndex].ID;
      Min = 0;
      Max -= Offset;
    }
    writeSubT6(Offset);
    JmpTarget ElseTarget{Builder, 0, 0};
    if (Max != Targets[EndIndex - 1].ID - Offset) {
      ElseTarget = writeBGTU(Targets[EndIndex - 1].ID - Offset);
      Max = Targets[EndIndex - 1].ID - Offset;
    }

    auto JmpTableEntries = writeJumpPCRelativeAddress(sizeof(uint64_t) * Min);
    JmpTableEntries.setTarget(Builder.currentOffset() - sizeof(uint64_t) * Min);
    size_t I = StartIndex;
    for (size_t ID = Min; ID <= Max; ID++) {
      while (ID > Targets[I].ID - Offset) I++;
      if (ID == Targets[I].ID - Offset) {
        Builder.write((uintptr_t) Targets[I].Address);
        I++;
      } else {
        ErrorCasesAbsolute.push_back(Builder.currentOffset());
        Builder.write((uint64_t) 0);
      }
    }

    ElseTarget.setTarget(Builder.currentOffset());
    return Offset;
  }

  void genSwitch(std::vector<FunctionInfos> &Targets, size_t StartIndex, size_t EndIndex, FunctionID PreferredID) {
    if (EndIndex - StartIndex <= 4) {
      genIfThenElseChain(Targets, PreferredID);
    } else {
      size_t NumTargets = EndIndex - StartIndex;
      size_t NumIds = Targets[EndIndex-1].ID - Targets[StartIndex].ID + 1;
      if (NumTargets > 8 && (NumIds - NumTargets <= 4 || NumTargets*1.5 >= NumIds)) {
        // fprintf(stderr, "Building jumptable with %ld targets in %ld IDs\n", NumTargets, NumIds);
        size_t Offset = genJumptable(Targets, StartIndex, EndIndex, 0, std::numeric_limits<FunctionID>::max());
        writeAddT6(Offset); // for error reporting
        // fprintf(stderr, " => offset=%ld\n", Offset);
      } else {
        genTree(Targets, StartIndex, EndIndex, 0, std::numeric_limits<FunctionID>::max());
      }
    }
  }

  void genSwitch(std::vector<FunctionInfos> &Targets, FunctionID PreferredID) {
    std::sort(Targets.begin(), Targets.end());
    genSwitch(Targets, 0, Targets.size(), PreferredID);
  }

  /*
   * my registers: >>t6<< =$14, t4-t7 ($12-$15)
   * argument registers: $4-$11 (a0-a7)
   * other registers to store on stack: a0-a7, t6, ra
   * stack alignment: 16bytes
   * return value: v0/v1 ($2/$3)
   */

  void genLazyDispatcher(int Index) {
    // sd $a0, -8($sp)
    // li $a0, Index
    // j <...>
    // total: 16 bytes
    size_t Offset = Builder.currentOffset();
    Builder.write32(0xffa4fff8);
    writeConstantToA0(Index);
    LazyDispatcherTailJumps.push_back(writeJump(false));
    if (Builder.currentOffset() < Offset + 16)
      Builder.write32(0x00000000);  // nop
  }

  void genLazyDispatcherTail() {
    // total: 48 + 32 + 60 bytes = 140
    // all heads jump here:
    for (auto &J: LazyDispatcherTailJumps) {
      J.setTarget(Builder.currentOffset());
    }
    LazyDispatcherTailJumps.clear();

    // The stack pointer sp must always be kept aligned to 16 bytes
    // push a1-a7 ($5-$11), t6($14) and ra
    // daddi $sp, $sp, -96 ; sd $a1, 0($sp) ; sd $a2, 8($sp) ; sd $a3, 16($sp) ; sd $a4, 24($sp) ; sd $a5, 32($sp) ; sd $a6, 40($sp) ; sd $a7, 48($sp) ; sd $ra, 56($sp) ; sd $14, 64($sp) ; sd $gp, 72($sp) ; sd $25, 80($sp)
    Builder.write("\xa0\xff\xbd\x63\x00\x00\xa5\xff\x08\x00\xa6\xff\x10\x00\xa7\xff\x18\x00\xa8\xff\x20\x00\xa9\xff\x28\x00\xaa\xff\x30\x00\xab\xff\x38\x00\xbf\xff\x40\x00\xae\xff\x48\x00\xbc\xff\x50\x00\xb9\xff", 48);
    // mov rsi, r11 ; call generateDispatcher
    writeMovT6ToA1();
    writeCall((uintptr_t) &generateDispatcher);
    // ld $a1, 0($sp) ; ld $a2, 8($sp) ; ld $a3, 16($sp) ; ld $a4, 24($sp) ; ld $a5, 32($sp) ; ld $a6, 40($sp) ; ld $a7, 48($sp) ; ld $ra, 56($sp) ; ld $14, 64($sp) ; ld $gp, 72($sp) ; ld $25, 80($sp) ; ld $a0, 88($sp) ; daddi $sp, $sp, 96 ; move $t9, $v0 ; jr $v0
    Builder.write("\x00\x00\xa5\xdf\x08\x00\xa6\xdf\x10\x00\xa7\xdf\x18\x00\xa8\xdf\x20\x00\xa9\xdf\x28\x00\xaa\xdf\x30\x00\xab\xdf\x38\x00\xbf\xdf\x40\x00\xae\xdf\x48\x00\xbc\xdf\x50\x00\xb9\xdf\x58\x00\xa4\xdf\x60\x00\xbd\x63\x08\x00\x40\x00\x25\xc8\x40\x00", 60);
  }

  inline size_t getLazyDispatcherEstimatedBytes(size_t Count) {
    return 16 * Count + 140;
  }

  void genInvalidIdDebugTrap(int Index) {
    writeMovT6ToA1();
    writeConstantToA0(Index);
    writeCall((uintptr_t) &__tg_dynamic_error);
  }
};

#endif // LLVM_TYPEGRAPHS_ASMWRITERMIPS64EL_H
