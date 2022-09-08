#ifndef LLVM_TYPEGRAPHS_ASMWRITERX86_64_H
#define LLVM_TYPEGRAPHS_ASMWRITERX86_64_H

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
    Builder.write("\xcc", 1);
  }

  JmpTarget writeJNEByte() {
    Builder.write("\x75\x00", 2, 1);
    return JmpTarget(Builder, Builder.currentOffset());
  }

  JmpTarget writeJEByte() {
    Builder.write("\x74\x00", 2, 1);
    return JmpTarget(Builder, Builder.currentOffset());
  }

  JmpTarget writeJAByte() {
    Builder.write("\x77\x00", 2, 1);
    return JmpTarget(Builder, Builder.currentOffset());
  }

  JmpTarget writeJAEByte() {
    Builder.write("\x73\x00", 2, 1);
    return JmpTarget(Builder, Builder.currentOffset());
  }

  JmpTarget writeJAInt() {
    Builder.write("\x0f\x87\x00\x00\x00\x00", 6, 2);
    return JmpTarget(Builder, Builder.currentOffset(), 4);
  }

  JmpTarget writeJAEInt() {
    Builder.write("\x0f\x83\x00\x00\x00\x00", 6, 2);
    return JmpTarget(Builder, Builder.currentOffset(), 4);
  }

  JmpTarget writeJmpByte() {
    Builder.write("\xeb\x00", 2, 1);
    return JmpTarget(Builder, Builder.currentOffset());
  }

  JmpTarget writeJmpInt() {
    Builder.write("\xe9\x00\x00\x00\x00", 5, 1);
    return JmpTarget(Builder, Builder.currentOffset(), 4);
  }

  void writeLongJmp(void *TargetPtr) {
    uint64_t Target = (uint64_t)TargetPtr;
    ssize_t Diff = Target - ((uint64_t) Builder.currentPos() + 5);
    if (Diff < -0x7ffff000 || Diff > 0x7ffff000) {
      // movabs r11, <C> ; jmp r11 | 49 bb <C> 41 ff e3
      Builder.write("\x49\xbb", 2);
      Builder.write(Target);
      Builder.write("\x41\xff\xe3", 3);
    } else {
      // jmp rip+-<Diff>
      Builder.write("\xe9", 1);
      Builder.write((uint32_t) Diff);
    }
  }

  void writeCmpR11(uint64_t C) {
    if (C <= 0x7fffffff) {
      Builder.write("\x49\x81\xfb", 3);
      Builder.write((uint32_t)C);
    } else {
      // inefficient! movabs r12, <C> ; cmp r11, r12
      // or: movabs rax, <C> ; cmp r11, rax
      // Builder.write("\x49\xbc", 2);
      Builder.write("\x48\xb8", 2);
      Builder.write(C);
      // Builder.write("\x4d\x39\xe3", 3);
      Builder.write("\x49\x39\xc3", 3);
    }
  }

  void writeSubR11(uint64_t C) {
    if (C == 0) return;
    if (C <= 0x7f) {
      Builder.write("\x49\x83\xeb", 3);
      Builder.write((uint8_t) C);
    } else if (C <= 0x7fffffff) {
      Builder.write("\x49\x81\xeb", 3);
      Builder.write((uint32_t) C);
    } else {
      assert(false && "Unsupported!");
    }
  }

  void writeAddR11(uint64_t C) {
    if (C == 0) return;
    if (C <= 0x7f) {
      Builder.write("\x49\x83\xc3", 3);
      Builder.write((uint8_t) C);
    } else if (C <= 0x7fffffff) {
      Builder.write("\x49\x81\xc3", 3);
      Builder.write((uint32_t) C);
    } else {
      assert(false && "Unsupported!");
    }
  }

  void writePushRax() {
    Builder.write("\x50", 1);
  }

  JmpTarget writeLeaRipToRax() {
    // 48 8d 05 10 00 00 00    lea    rax, [rip+0x10]
    Builder.write("\x48\x8d\x05\x00\x00\x00\x00", 7, 3);
    return JmpTarget(Builder, Builder.currentOffset(), 4);
  }

  void writeRaxRip8Jmp() {
    // 42 ff 24 d8             jmp    QWORD PTR [rax+r11*8]
    Builder.write("\x42\xff\x24\xd8", 4);
  }

  void writeMovR11ToRsi() {
    // mov    rsi, r11
    Builder.write("\x4c\x89\xde", 3);
  }

  void writeConstantToRdi(uintptr_t Rdi) {
    // movabs rdi, 0x123456789abcdef
    Builder.write("\x48\xbf", 2);
    Builder.write(Rdi);
  }

  void writeConstantToR11(uintptr_t R11) {
    // movabs rdi, 0x123456789abcdef
    Builder.write("\x49\xbb", 2);
    Builder.write(R11);
  }

  void writeCall(uint64_t Target) {
    ssize_t Diff = Target - ((uint64_t) Builder.currentPos() + 5);
    if (Diff < -0x7ffff000 || Diff > 0x7ffff000) {
      // movabs r11, <C> ; call r11 | 49 bb <C> 41 ff d3
      Builder.write("\x49\xbb", 2);
      Builder.write(Target);
      Builder.write("\x41\xff\xd3", 3);
    } else {
      // call rip+-<Diff>
      Builder.write("\xe8", 1);
      Builder.write((uint32_t) Diff);
    }
  }

  void writeCrash() {
    Builder.write("\x0f\x0b", 2);
  }

  void genIfThenElseChain(const std::vector<FunctionInfos, ProtectedAllocator<FunctionInfos>> &Targets, FunctionID PreferredID) {
    auto PreferredFunc = std::find(Targets.begin(), Targets.end(), PreferredID);
    if (PreferredFunc != Targets.end()) {
      writeCmpR11(PreferredID);
      auto Jmp = writeJNEByte();
      writeLongJmp(PreferredFunc->Address);
      Jmp.setTarget(Builder.currentOffset());
    }

    for (auto &Target: Targets) {
      if (Target.ID != PreferredID) {
        writeCmpR11(Target.ID);
        auto Jmp = writeJNEByte();
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
   * @param R11HasLastBeenComparedTo If the last instruction before this tree was a "cmp r11, <const>", pass this
   * constant. Saves one instruction.
   */
  void genTree(const std::vector<FunctionInfos, ProtectedAllocator<FunctionInfos>> &Targets, size_t StartIndex, size_t EndIndex, FunctionID Min, FunctionID Max,
               FunctionID R11HasLastBeenComparedTo = -1) {
    // only 1 target
    if (EndIndex - StartIndex == 1) {
      if (Min != Max) {
        if (R11HasLastBeenComparedTo != Targets[StartIndex].ID)
          writeCmpR11(Targets[StartIndex].ID);
        auto Jmp = writeJNEByte();
        writeLongJmp(Targets[StartIndex].Address);
        Jmp.setTarget(Builder.currentOffset());
      } else {
        writeLongJmp(Targets[StartIndex].Address);
      }
      return;
    }
    // only 2 targets
    if (EndIndex - StartIndex == 2) {
      if (R11HasLastBeenComparedTo != Targets[StartIndex].ID)
        writeCmpR11(Targets[StartIndex].ID);
      auto Jmp = writeJNEByte();
      writeLongJmp(Targets[StartIndex].Address);
      Jmp.setTarget(Builder.currentOffset());
      if (Min == Targets[StartIndex].ID) Min++;
      genTree(Targets, StartIndex+1, EndIndex, Min, Max);
      return;
    }
    // >2 targets
    size_t PivotIndex = (StartIndex + 1 + EndIndex) / 2;
    writeCmpR11(Targets[PivotIndex].ID);
    auto Jmp = PivotIndex - StartIndex <= 4 ? writeJAEByte() : writeJAEInt();
    genTree(Targets, StartIndex, PivotIndex, Min, Targets[PivotIndex].ID - 1);
    if (PivotIndex - StartIndex < Targets[PivotIndex].ID - Min) {
      // error case
      ErrorCases.push_back(Targets.size() - PivotIndex > 4 ? writeJmpInt(): writeJmpByte());
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
    writeSubR11(Offset);
    JmpTarget ElseTarget{Builder, 0, 0};
    if (Max != Targets[EndIndex - 1].ID - Offset) {
      writeCmpR11(Targets[EndIndex - 1].ID - Offset);
      ElseTarget = writeJAInt();
      Max = Targets[EndIndex - 1].ID - Offset;
    }

    auto JmpTableEntries = writeLeaRipToRax();
    writeRaxRip8Jmp();
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
        writeAddR11(Offset); // for error reporting
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
    // total: 13 bytes, padded to 16 bytes
    // push rdi ; mov edi, Index
    Builder.write("\x57\x48\xc7\xc7", 4);
    Builder.write((uint32_t) Index);
    // jmp <tail>
    LazyDispatcherTailJumps.push_back(writeJmpInt());
    Builder.write("\xcc\xcc\xcc", 3);
  }

  void genLazyDispatcherTail() {
    // total: 43 bytes
    // all heads jump here:
    for (auto &J: LazyDispatcherTailJumps) {
      J.setTarget(Builder.currentOffset());
    }
    LazyDispatcherTailJumps.clear();
    // push rsi ; push rdx ; push rcx ; push r8 ; push r9 ; push r10 ; push r11 ; push rsi // second push rsi is for stack alignment
    Builder.write("\x56\x52\x51\x41\x50\x41\x51\x41\x52\x41\x53\x56", 12);
    // mov rsi, r11 ; call generateDispatcher
    writeMovR11ToRsi();
    writeCall((uintptr_t) &generateDispatcher);
    // pop rsi ; pop r11 ; pop r10 ; pop r9 ; pop r8 ; pop rcx ; pop rdx ; pop rsi ; pop rdi ; jmp rax
    Builder.write("\x5e\x41\x5b\x41\x5a\x41\x59\x41\x58\x59\x5a\x5e\x5f\xff\xe0", 15);
  }

  inline size_t getLazyDispatcherEstimatedBytes(size_t Count) {
    return 16 * Count + 43;
  }

  void genInvalidIdDebugTrap(int Index) {
    writePushRax();
    writeMovR11ToRsi();
    writeConstantToRdi(Index);
    writeCall((uintptr_t) &__tg_dynamic_error);
  }
};

#endif // LLVM_TYPEGRAPHS_ASMWRITERX86_64_H
