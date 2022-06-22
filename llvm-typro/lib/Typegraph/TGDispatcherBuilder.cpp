#include "TGDispatcherBuilder.h"
#ifndef WITHOUT_LLVM
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Operator.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Typegraph/TypegraphSettings.h>
#endif

#include <queue>

using namespace llvm;

namespace typegraph {

#ifndef WITHOUT_LLVM

namespace {
size_t getSize(llvm::Module &M, llvm::Type *Ty) {
  return Ty->isSized() ? M.getDataLayout().getTypeSizeInBits(Ty) : 0;
}
} // namespace

void LLVMDispatcherBuilder::initialize() {
  // Search for usages
  /*
  for (auto &It : Functions) {
    auto *F = It.second->Ref;
    for (auto *User : F->users()) {
      if (auto *C = dyn_cast<llvm::CallBase>(User)) {
        if (C->getCalledFunction() == F)
          continue;
      }
      It.second->Uses.push_back(User);
    }
  }
   // */

  // Generate target set IDs
  for (auto &RP: ResolvePoints) {
    if (RP.VertexID < 0 || RP.Hash != nullptr)
      continue;
    std::string S = std::to_string(RP.VertexID) + "|" + std::to_string(RP.CallInst->arg_size());
    S += "," + std::to_string(getSize(M, RP.CallInst->getType()));
    for (unsigned I = 0; I < RP.CallInst->getNumArgOperands(); I++) {
      S += "," + std::to_string(getSize(M, RP.CallInst->getArgOperand(I)->getType()));
    }
    std::set<DFunction *> Targets(RP.Targets.begin(), RP.Targets.end());
    for (auto *F: Targets) {
      S += "," + std::to_string((uintptr_t) F);
    }
    RP.Hash = Symbols->get(S);
    CountPerTargetHash[RP.Hash]++;
  }
}

void LLVMDispatcherBuilder::collectCluster(std::set<DFunction *> &Cluster, std::set<DResolvePoint *> &RPs) {
  std::queue<DFunction *> Q;
  for (auto *F: Cluster) {
    F->InCluster = true;
    Q.push(F);
  }

  while (!Q.empty()) {
    auto *F = Q.front();
    Q.pop();
    for (auto &RP: ResolvePoints) {
      if (std::find(RP.Targets.begin(), RP.Targets.end(), F) != RP.Targets.end()) {
        if (RPs.insert(&RP).second) {
          // functions F2 belong into same cluster than F
          for (auto *F2: RP.Targets) {
            if (F2->InCluster || F2->Removed || !F2->Ref)
              continue;
            Cluster.insert(F2);
            F2->InCluster = true;
            Q.push(F2);
          }
        }
      }
    }
  }
}

long LLVMDispatcherBuilder::assignClusterIDs(std::set<DFunction *> &Cluster, std::set<DResolvePoint *> &RPs,
                                             long MinNumber) {
  if (Cluster.size() > 5) {
    DResolvePoint *Biggest =
        nullptr; // the biggest set of idents that is: subset of cluster, not equal to cluster, size >4.
    for (auto &RP: RPs) {
      if (RP->Targets.size() <= 4 || RP->Targets.size() >= Cluster.size() ||
          (Biggest && RP->Targets.size() <= Biggest->Targets.size()))
        continue;
      bool AllIdentsInCluster = true;
      for (auto *F: RP->Targets) {
        if (Cluster.find(F) == Cluster.end()) {
          AllIdentsInCluster = false;
          break;
        }
      }
      if (AllIdentsInCluster)
        Biggest = RP;
    }

    // if we found a sub-cluster that is large enough, give it consecutive numbers
    if (Biggest) {
      std::set<DFunction *> SubCluster(Biggest->Targets.begin(), Biggest->Targets.end());
      MinNumber = assignClusterIDs(SubCluster, RPs, MinNumber);
      std::set<DFunction *> RemainingCluster;
      std::set_difference(Cluster.begin(), Cluster.end(), SubCluster.begin(), SubCluster.end(),
                          std::inserter(RemainingCluster, RemainingCluster.end()));
      MinNumber = assignClusterIDs(RemainingCluster, RPs, MinNumber);
      return MinNumber;
    }
  }

  // otherwise - screw it, doesn't matter.
  for (auto &F: Cluster) {
    F->ID = MinNumber++;
  }
  return MinNumber;
}

void LLVMDispatcherBuilder::assignOptimalIDs() {
  // Cluster 1: external functions
  std::set<DFunction *> ExternalCluster;
  std::set<DResolvePoint *> ExternalRPs;
  for (auto &It: Functions) {
    if (It.second->Removed || !It.second->Ref || It.second->ID != NOID || It.second->InCluster)
      continue;
    if (It.second->Leaking) {
      ExternalCluster.insert(It.second.get());
    }
  }
  for (auto &RP: ResolvePoints) {
    if (RP.IsExternal) {
      for (auto *F: RP.Targets) {
        if (F->Removed || !F->Ref || F->ID != NOID || F->InCluster)
          continue;
        ExternalCluster.insert(F);
      }
    }
  }
  collectCluster(ExternalCluster, ExternalRPs);
  NextId = assignClusterIDs(ExternalCluster, ExternalRPs, NextId);

  // Other clusters (pure internal)
  long Max = NextId - 1;
  for (auto &It: Functions) {
    if (It.second->Removed || !It.second->Ref || It.second->ID != NOID || It.second->InCluster)
      continue;
    std::set<DFunction *> Cluster;
    std::set<DResolvePoint *> RPs;
    Cluster.insert(It.second.get());
    collectCluster(Cluster, RPs);
    Max = std::max(Max, assignClusterIDs(Cluster, RPs, Settings.enforce_min_id) - 1);
  }

  llvm::errs() << "Function IDs: " << Functions.size() << " functions, max id " << Max << " \n";
}

void LLVMDispatcherBuilder::assignMissingIDs() {
  for (auto &It: Functions) {
    if (It.second->ID == NOID && !It.second->Removed && It.second->Ref) {
      It.second->ID = NextId++;
    }
  }
}

void LLVMDispatcherBuilder::debugIDAssignment() {
  // ID assignment analysis
  int Count = 0;
  float AverageWidth = 0.0;
  float WorstWidth = 1.0;
  float WorstWidthAbove20 = 1.0;
  for (auto &RP: ResolvePoints) {
    if (RP.Targets.size() < 4)
      continue;
    long MinID = RP.Targets[0]->ID;
    long MaxID = RP.Targets[0]->ID;
    for (auto *F: RP.Targets) {
      MinID = std::min(MinID, F->ID);
      MaxID = std::max(MaxID, F->ID);
    }
    // width: 100% = perfect density, 70% = 30% "empty" entries
    float Width = (MaxID + 1 - MinID) * 1.0 / RP.Targets.size();
    if (RP.Type == EXTERNAL_FUNCTION) {
      Width = (MaxID + 1) * 1.0 / (RP.Targets.size() + Settings.enforce_min_id);
    }
    Count++;
    AverageWidth += Width;
    WorstWidth = std::min(WorstWidth, Width);
    if (RP.Targets.size() >= 20)
      WorstWidthAbove20 = std::min(WorstWidthAbove20, Width);
  }

  if (Count > 0) {
    fprintf(stderr, "Average width of %4d calls:   %.2f%%\n", Count, AverageWidth * 100.0 / Count);
    fprintf(stderr, "Worst width:                   %.2f%%\n", WorstWidth * 100.0);
    fprintf(stderr, "Worst width (for large calls): %.2f%%\n", WorstWidthAbove20 * 100.0);
  }
}

static Constant *constantCast(Constant *NewValue, Type *Type) {
  if (Type == NewValue->getType()) {
    return NewValue;
  }
  if (Type->isPointerTy()) {
    if (NewValue->getType()->isPointerTy()) {
      return ConstantExpr::getBitCast(NewValue, Type);
    }
    if (NewValue->getType()->isIntegerTy()) {
      return ConstantExpr::getIntToPtr(NewValue, Type);
    }
    return nullptr;
  }
  if (Type->isIntegerTy()) {
    if (NewValue->getType()->isPointerTy()) {
      return ConstantExpr::getPtrToInt(NewValue, Type);
    }
    if (NewValue->getType()->isIntegerTy()) {
      return ConstantExpr::getIntegerCast(NewValue, Type, true);
    }
    return nullptr;
  }
  return nullptr;
}

template <class T> static Constant *replaceValue(LLVMDispatcherBuilder *Builder, T *User, Value *Old, Constant *New) {
  std::vector<Constant *> Fields;
  // std::vector<Function *> AffectedFunctions;
  for (unsigned I = 0; I < User->getNumOperands(); I++) {
    auto *Op = User->getOperand(I);
    if (Op && Op == Old) {
      auto *CastedNewValue = constantCast(New, Op->getType());
      assert(CastedNewValue != nullptr && "Invalid conversion");
      Fields.push_back(CastedNewValue);
    } else {
      Fields.push_back(Op);
      /*if (auto *F = dyn_cast<Function>(Op)) {
        AffectedFunctions.push_back(F);
      }*/
    }
  }
  auto *NewThing = T::get(User->getType(), Fields);
  /*for (auto *F : AffectedFunctions) {
    auto *DF = Builder->getFunctionOpt(F);
    if (DF != nullptr) {
      // DF->Uses.push_back(NewThing);
    }
  }*/
  return NewThing;
}

bool LLVMDispatcherBuilder::typesafeReplaceAllUsesWith(User *OldValue, Constant *NewValue) {
  std::vector<User *> Users(OldValue->user_begin(), OldValue->user_end());
  bool Result = true;
  for (auto *User: Users) {
    if (!isa<Constant>(User)) {
      User->replaceUsesOfWith(OldValue, NewValue);
    } else if (isa<BitCastOperator>(User) || isa<PtrToIntOperator>(User)) {
      // User is already a cast
      auto *CastedNewValue = constantCast(NewValue, User->getType());
      assert(CastedNewValue != nullptr && "Invalid conversion (1)");
      return typesafeReplaceAllUsesWith(User, CastedNewValue);
    } else if (auto *Op = dyn_cast<AddOperator>(User)) {
      auto *Op0 = cast<Constant>(User->getOperand(0));
      auto *Op1 = cast<Constant>(User->getOperand(1));
      auto *NewOp = ConstantExpr::getAdd(Op0 == OldValue ? constantCast(NewValue, Op0->getType()) : Op0,
                                         Op1 == OldValue ? constantCast(NewValue, Op1->getType()) : Op1,
                                         Op->hasNoUnsignedWrap(), Op->hasNoSignedWrap());
      Result &= typesafeReplaceAllUsesWith(Op, NewOp);
    } else if (auto *ConstStruct = dyn_cast<ConstantStruct>(User)) {
      // oldValue is part of a constant struct
      Result &= typesafeReplaceAllUsesWith(User, replaceValue(this, ConstStruct, OldValue, NewValue));
    } else if (auto *ConstArray = dyn_cast<ConstantArray>(User)) {
      Result &= typesafeReplaceAllUsesWith(User, replaceValue(this, ConstArray, OldValue, NewValue));
    } else if (auto *V = dyn_cast<GlobalVariable>(User)) {
      if (V->getInitializer() == OldValue) {
        auto *NewInit = constantCast(NewValue, V->getValueType());
        assert(NewInit != nullptr);
        V->setInitializer(NewInit);
      } else {
        llvm::errs() << "[WARNING] cannot replace stuff in global var: " << *User << "    (" << *OldValue << ")\n";
        assert(false);
        Result = false;
      }
    } else {
      llvm::errs() << "[WARNING] cannot replace stuff: " << *User << "    (" << *OldValue << ")\n";
      assert(false);
      Result = false;
    }
  }
  return Result;
}

/**
 * Replace all uses of "Old" inside "V" with "New"
 * @param V
 * @param Old
 * @param New
 * @return
 */
bool LLVMDispatcherBuilder::typesafeReplaceUseWith(User *User, Value *Old, Constant *New) {
  if (!isa<Constant>(User)) {
    if (New->getType() != Old->getType()) {
      New = ConstantExpr::getBitCast(New, Old->getType());
    }
    bool Found = false;
    for (unsigned I = 0; I < User->getNumOperands(); I++) {
      if (User->getOperand(I) == Old) {
        User->setOperand(I, New);
        Found = true;
      }
    }
    return Found;
  }
  // that's a constant
  if (isa<BitCastOperator>(User) || isa<PtrToIntOperator>(User)) {
    // User is already a cast
    auto *CastedNewValue = constantCast(New, User->getType());
    assert(CastedNewValue != nullptr && "Invalid conversion (1)");

    // Check for special case: call (bitcast f)
    bool HasDirectCallUses = false;
    bool HasNonDirectCallUses = false;
    for (auto *User2: User->users()) {
      if (auto *C = dyn_cast<CallBase>(User2)) {
        if (C->getCalledValue() == User) {
          HasDirectCallUses = true;
          continue;
        }
      }
      HasNonDirectCallUses = true;
    }

    if (HasDirectCallUses && HasNonDirectCallUses) {
      // replace every use except direct calls
      for (auto *User2: User->users()) {
        if (auto *C = dyn_cast<CallBase>(User2)) {
          if (C->getCalledValue() == User) {
            continue;
          }
        }
        typesafeReplaceUseWith(User2, User, CastedNewValue);
      }
    } else if (!HasDirectCallUses) {
      User->replaceAllUsesWith(CastedNewValue);
    }
    return true;
    // return typesafeReplaceAllUsesWith(User, CastedNewValue);
  }
  if (auto *ConstStruct = dyn_cast<ConstantStruct>(User)) {
    // oldValue is part of a constant struct
    User->replaceAllUsesWith(replaceValue(this, ConstStruct, Old, New));
    return true;
    // return typesafeReplaceAllUsesWith(User, replaceValue(this, ConstStruct, Old, New));
  }
  if (auto *ConstArray = dyn_cast<ConstantArray>(User)) {
    User->replaceAllUsesWith(replaceValue(this, ConstArray, Old, New));
    return true;
    // return typesafeReplaceAllUsesWith(ConstArray, replaceValue(this, ConstArray, Old, New));
  }
  if (auto *V = dyn_cast<GlobalVariable>(User)) {
    if (V->getInitializer() == Old) {
      auto *NewInit = constantCast(New, V->getValueType());
      assert(NewInit != nullptr);
      V->setInitializer(NewInit);
      return true;
    }
    llvm::errs() << "[WARNING] cannot replace stuff in global var: " << *User << "    (" << *Old << ")\n";
    assert(false);
    return false;
  }
  if (auto *A = dyn_cast<GlobalAlias>(User)) {
    if (A->getAliasee() == Old) {
      A->setAliasee(New);
      return true;
    }
    llvm::errs() << "[WARNING] cannot replace stuff in alias: " << *User << "    (" << *Old << ")\n";
    assert(false);
    return false;
  }
  llvm::errs() << "Can't replace things: " << *User << " | " << *Old << " | " << *New << "\n";
  assert(false);
  return false;
}

namespace {
bool canReplaceUseLibcSafe(User *User) {
  if (auto *GV = dyn_cast<GlobalVariable>(User)) {
    return GV->getName() != "llvm.global_ctors" && GV->getName() != "llvm.global_dtors" && GV->getName() != "llvm.used";
  }
  if (auto *C = dyn_cast<Constant>(User)) {
    for (auto *User2: C->users()) {
      if (!canReplaceUseLibcSafe(User2))
        return false;
    }
  }
  if (auto *Ins = dyn_cast<Instruction>(User)) {
    if (Ins->getFunction()->hasName() && Ins->getFunction()->getName() == "__libc_sigaction")
      return false;
  }
  if (Settings.link_with_libc)
    return true;
  if (auto *A = dyn_cast<GlobalAlias>(User)) {
    if (A->getName() == "_init" || A->getName() == "_fini")
      return false;
  }
  if (auto *C = dyn_cast<CallInst>(User)) {
    if (C->getCalledFunction()) {
      auto Name = C->getCalledFunction()->getName();
      if (Name == "__cxa_atexit" || Name == "__libc_start_main")
        return false;
    }
  }
  return true;
}
} // namespace

void LLVMDispatcherBuilder::replaceFunctionsWithIDs() {
  for (auto &It: Functions) {
    auto *F = It.second->Ref;
    if (It.second->Removed || !F)
      continue;
    if (It.second->ID == NOID)
      It.second->ID = NextId++;
    auto *C = ConstantExpr::getIntToPtr(ConstantInt::get(IntPtr, It.second->ID), F->getType());

    // Functions never used in direct calls can be replaced by LLVM (by far more efficient than our method)
    if (canReplaceAllFunctionUsages(It.second.get()->Ref)) {
      F->replaceAllUsesWith(C);
      continue;
    }

    // other functions must have every use replaced manually
    std::vector<std::pair<User *, Value *>> Usages;
    collectFunctionUsages(Usages, It.second.get()->Ref);
    for (auto &It2: Usages) {
      auto *User = It2.first;
      auto *Old = It2.second;
      if (!canReplaceUseLibcSafe(User))
        continue;
      // Replace function F with ID in User:
      // llvm::errs() << "User [before]    " << F->getName() << "     " << *User << "\n";
      if (!typesafeReplaceUseWith(User, Old, C)) {
        llvm::errs() << "[WARN] Function use operand not found! " << *User << " old=" << *Old << "\n";
        //assert(false);
      }
      // llvm::errs() << "User [after]     " << F->getName() << "     " << *User << "\n";
    }
  }
}

static Value *castTo(IRBuilder<> &Builder, Value *V, Type *Type) {
  if (Type == V->getType()) {
    return V;
  }
  if (Type->isPointerTy()) {
    if (V->getType()->isPointerTy()) {
      return Builder.CreateBitCast(V, Type);
    }
    if (V->getType()->isIntegerTy()) {
      return Builder.CreateIntToPtr(V, Type);
    }
    return nullptr;
  }
  if (Type->isIntegerTy()) {
    if (V->getType()->isPointerTy()) {
      return Builder.CreatePtrToInt(V, Type);
    }
    if (V->getType()->isIntegerTy()) {
      return Builder.CreateIntCast(V, Type, true);
    }
    return nullptr;
  }
  if (Type->isFloatingPointTy() && V->getType()->isFloatingPointTy()) {
    return Builder.CreateFPCast(V, Type);
  }
  return nullptr;
}

void LLVMDispatcherBuilder::generateCodeForResolvePoint(DResolvePoint &RP) {
  // llvm::errs() << "CodeGen:" << *RP.CallInst << " | with " << RP.Targets.size()
  //              << " targets, external=" << (RP.IsExternal ? 1 : 0) << "\n";
  auto *F = RP.CallInst->getFunction();

  if (RP.Type == CALL) {
    // Check if we should generate a dispatcher function for this one
    if (RP.Hash && CountPerTargetHash[RP.Hash] >= Settings.enforce_dispatcher_limit && RP.Targets.size() > 4) {
      // Replace this!
      auto FunctionName = "__tg_dispatcher_" + std::to_string((uintptr_t) RP.Hash);
      auto *Dispatcher = M.getFunction(FunctionName);
      if (Dispatcher == nullptr) {
        // Create a new dispatcher function
        std::vector<Type *> ArgumentTypes;
        for (unsigned I = 0; I < RP.CallInst->getNumArgOperands(); I++) {
          ArgumentTypes.push_back(RP.CallInst->getArgOperand(I)->getType());
        }
        ArgumentTypes.push_back(RP.CallInst->getCalledValue()->getType());
        auto *FT = FunctionType::get(RP.CallInst->getType(), ArgumentTypes, false);
        Dispatcher = cast<Function>(M.getOrInsertFunction(FunctionName, FT).getCallee());
        Dispatcher->setCallingConv(CallingConv::Fast);
        Dispatcher->setLinkage(llvm::GlobalValue::PrivateLinkage);
        // Replicate the indirect call
        IRBuilder<> B(BasicBlock::Create(Context, "entry", Dispatcher));
        std::vector<Value *> Arguments;
        for (unsigned I = 0; I < Dispatcher->arg_size() - 1; I++) {
          Arguments.push_back(Dispatcher->getArg(I));
        }
        auto *Callee = Dispatcher->getArg(Dispatcher->arg_size() - 1);
        auto *Call = B.CreateCall(Callee, Arguments);
        if (Call->getType()->isVoidTy()) {
          B.CreateRetVoid();
        } else {
          B.CreateRet(Call);
        }
        // Let it resolve
        DResolvePoint DispatcherRP = RP;
        DispatcherRP.CallInst = Call;
        DispatcherRP.Hash = nullptr;
        generateCodeForResolvePoint(DispatcherRP);
      }
      // Replace call with dispatcher call
      IRBuilder<> B(RP.CallInst);
      std::vector<Value *> Arguments;
      for (unsigned I = 0; I < RP.CallInst->getNumArgOperands(); I++) {
        Arguments.push_back(castTo(B, RP.CallInst->getArgOperand(I), Dispatcher->getFunctionType()->getParamType(I)));
      }
      auto *DispatcherFuncType = Dispatcher->getFunctionType()->getParamType(Dispatcher->getFunctionType()->getNumParams() - 1);
      Arguments.push_back(castTo(B, RP.CallInst->getCalledValue(), DispatcherFuncType));
      auto *NewCall = B.CreateCall(Dispatcher, Arguments);
      NewCall->setCallingConv(Dispatcher->getCallingConv());
      RP.CallInst->replaceAllUsesWith(castTo(B, NewCall, RP.CallInst->getType()));
      RP.CallInst->eraseFromParent();
      RP.CallInst = nullptr;
      return;
    }

    if (RP.Targets.size() == 1 && !RP.IsExternal) {
      // Optimization: only one possible target -> set directly
      if (RP.CallInst->getCalledValue()->getType() == RP.Targets[0]->Ref->getType()) {
        RP.CallInst->setCalledFunction(RP.Targets[0]->Ref);
      } else {
        // fix argument / return types
        IRBuilder<> Builder(RP.CallInst);
        std::vector<Value *> Arguments;
        auto *FT = RP.Targets[0]->Ref->getFunctionType();
        for (unsigned I = 0; I < RP.CallInst->getNumArgOperands(); I++) {
          if (I < FT->getNumParams()) {
            Arguments.push_back(castTo(Builder, RP.CallInst->getArgOperand(I), FT->getParamType(I)));
          } else {
            Arguments.push_back(RP.CallInst->getArgOperand(I));
          }
        }
        auto *Call = Builder.CreateCall(RP.Targets[0]->Ref, Arguments);
        Call->setCallingConv(RP.Targets[0]->Ref->getCallingConv());
        if (!RP.CallInst->user_empty()) {
          auto *CastedCall = castTo(Builder, Call, RP.CallInst->getType());
          RP.CallInst->replaceAllUsesWith(CastedCall);
        }
        RP.CallInst->eraseFromParent();
        RP.CallInst = nullptr;
      }
    } else {
      // build a switch case
      auto *Invoke = llvm::dyn_cast_or_null<InvokeInst>(RP.CallInst);
      auto *FirstBB = RP.CallInst->getParent();
      auto *SecondBB = Invoke ? Invoke->getNormalDest() : FirstBB->splitBasicBlock(RP.CallInst);
      auto *DefaultBB = BasicBlock::Create(Context, "", F);
      if (!Invoke)
        FirstBB->getTerminator()->eraseFromParent();
      auto CaseNumber = RP.Targets.size() + (RP.IsExternal ? 1 : 0);
      IRBuilder<> Builder(FirstBB);
      auto *FunctionID = Builder.CreatePtrToInt(RP.CallInst->getCalledValue(), IntPtr);
      auto *Switch = Builder.CreateSwitch(FunctionID, DefaultBB, CaseNumber);
      llvm::PHINode *Phi = nullptr;
      if (!RP.CallInst->getType()->isVoidTy() && CaseNumber > 0) {
        Builder.SetInsertPoint(SecondBB, SecondBB->begin());
        Phi = Builder.CreatePHI(RP.CallInst->getType(), CaseNumber);
        RP.CallInst->replaceAllUsesWith(Phi);
      }

      // create a case for each function
      std::vector<BasicBlock *> newBasicBlocks;
      for (auto *Target: RP.Targets) {
        auto *BB = BasicBlock::Create(Context, "", F);
        auto *OutBB = BB;
        Builder.SetInsertPoint(BB);
        // Cast arguments if necessary
        std::vector<Value *> Arguments;
        auto *FT = Target->Ref->getFunctionType();
        for (unsigned I = 0; I < RP.CallInst->getNumArgOperands(); I++) {
          if (I < FT->getNumParams()) {
            auto *Arg = castTo(Builder, RP.CallInst->getArgOperand(I), FT->getParamType(I));
            if (!Arg) {
              llvm::errs() << "In call " << *RP.CallInst << " / function type " << *FT << "\n";
              llvm::errs() << "Can't cast argument " << I << ": " << *RP.CallInst->getArgOperand(I) << " to type "
                           << *FT->getParamType(I) << "\n";
            }
            assert(Arg);
            Arguments.push_back(Arg);
          } else {
            Arguments.push_back(RP.CallInst->getArgOperand(I));
          }
        }

        // Build a direct call
        auto *Call = Invoke
                         ? (llvm::CallBase *) Builder.CreateInvoke(Target->Ref, SecondBB, Invoke->getUnwindDest(), Arguments)
                         : (llvm::CallBase *) Builder.CreateCall(Target->Ref, Arguments);
        Call->setCallingConv(RP.Targets[0]->Ref->getCallingConv());
        Call->setDebugLoc(RP.CallInst->getDebugLoc());
        if (RP.CallInst->isTailCall()) {
          if (auto *Call2 = dyn_cast<CallInst>(Call)) {
            Call2->setTailCall(true);
          }
        }
        if (Phi) {
          if (!Invoke) {
            auto *CastedCall = castTo(Builder, Call, Phi->getType());
            if (!CastedCall) {
              llvm::errs() << "PHI type error: Can't cast " << *Call->getType() << " to " << *Phi->getType() << "\n";
              llvm::errs() << *RP.CallInst << "\n" << *Call << "\n";
            }
            assert(CastedCall);
            Phi->addIncoming(CastedCall, BB);
          } else if (Call->getType() == Phi->getType()) {
            Phi->addIncoming(Call, BB);
          } else {
            // We need a new BB (just to cast the result of a successful invoke)!
            OutBB = BasicBlock::Create(Context, "", F);
            cast<InvokeInst>(Call)->setNormalDest(OutBB);
            Builder.SetInsertPoint(OutBB);
            Phi->addIncoming(castTo(Builder, Call, Phi->getType()), OutBB);
            Builder.CreateBr(SecondBB);
          }
        }
        if (!Invoke)
          Builder.CreateBr(SecondBB);
        // Add to outer switch-case struct
        Switch->addCase(ConstantInt::get(IntPtr, Target->ID), BB);
        newBasicBlocks.push_back(OutBB);
      }

      // create the default case handler
      Builder.SetInsertPoint(DefaultBB);
      if (RP.IsExternal) {
        // llvm::errs() << "TYPE " << "__tg_dispatcher_" + *RP.CallName << " = " << *RP.CallInst->getCalledValue()->getType() << "\n";
        auto *GVPtr = RP.CallInst->getModule()->getOrInsertGlobal("__tg_dyn_dispatcher_" + *RP.CallName,
                                                                  RP.CallInst->getCalledValue()->getType());
        if (auto *GV = dyn_cast<GlobalVariable>(GVPtr)) {
          // GV->setVisibility(llvm::GlobalValue::HiddenVisibility);
          GV->setLinkage(llvm::GlobalValue::PrivateLinkage);
          GV->setInitializer(ConstantData::getNullValue(RP.CallInst->getCalledValue()->getType()));
          newTrampoline(GV);
          ExternalCallDispatchers[RP.CallName] = GV;
        }
        auto *Dispatcher = Builder.CreateLoad(GVPtr);
        // move call target to r11 (x64) / x16(aarch64)
        const char *Constraints;
        auto TT = StringRef(M.getTargetTriple());
        if (TT.startswith("x86_64-")) {
          Constraints = "{r11},~{dirflag},~{fpsr},~{flags}";
        } else if (TT.startswith("aarch64-")) {
          Constraints = "{x16}";
        } else if (TT.startswith("mips64el-")) {
          Constraints = "{$14},~{$1}";
        } else {
          llvm::report_fatal_error("Unsupported target architecture for TyPro dynamic linking!");
        }
        auto *Asm =
            InlineAsm::get(
                FunctionType::get(Type::getVoidTy(Context), RP.CallInst->getCalledValue()->getType(), false),
                "", Constraints, true);
        Builder.CreateCall(Asm, {RP.CallInst->getCalledValue()});
        std::vector<Value *> Arguments(RP.CallInst->arg_begin(), RP.CallInst->arg_end());
        auto *Call = Builder.CreateCall(Dispatcher, Arguments);
        Call->setDebugLoc(RP.CallInst->getDebugLoc());
        if (Phi) {
          Phi->addIncoming(castTo(Builder, Call, Phi->getType()), DefaultBB);
        }
        Builder.CreateBr(SecondBB);
        newBasicBlocks.push_back(DefaultBB);
      } else {
        // not external - terminate when no valid function ID is found
        generateErrorCase(Builder, RP, FunctionID);
      }

      // In case of Invoke: patch PHIs in default dest block (which now has different predecessors)
      if (Invoke) {
        for (auto &Ins: *SecondBB) {
          auto *SecondPhi = dyn_cast<PHINode>(&Ins);
          if (!SecondPhi) break;
          if (SecondPhi == Phi) continue;

          auto *V = SecondPhi->getIncomingValueForBlock(FirstBB);
          if (V == Phi) {
            // case 1: phi uses call return value => "our" phi => inline our new phi
            for (unsigned int BlockIndex = 0; BlockIndex < Phi->getNumIncomingValues(); BlockIndex++) {
              SecondPhi->addIncoming(Phi->getIncomingValue(BlockIndex), Phi->getIncomingBlock(BlockIndex));
            }
          } else {
            // case 2: phi uses other value => create new cases for each BB, remove old one
            for (auto *BB: newBasicBlocks) {
              SecondPhi->addIncoming(V, BB);
            }
          }
          SecondPhi->removeIncomingValue(FirstBB);
        }
        if (Phi && Phi->user_empty())
          Phi->eraseFromParent();
      }

      // remove original call
      if (!RP.CallInst->user_empty()) {
        // This should only happen if a call has zero targets and its return value is actually used
        RP.CallInst->replaceAllUsesWith(UndefValue::get(RP.CallInst->getType()));
      }
      RP.CallInst->eraseFromParent();
      RP.CallInst = nullptr;
    }

    // llvm::errs() << "CodeGen finished: " << *F << "\n\n\n";

  } else {
    // Generate code for resolve points. Remember: here a nullptr is a valid result.
    // we don't care about "external" for now

    auto *DeepMetadata = RP.CallInst->getMetadata("typegraph-resolve-deep");
    if (!DeepMetadata) {
      Value *OldValue = RP.CallInst->getArgOperand(RP.ResolveArgnum);
      Value *NewValue = generateBackTranslation(RP, OldValue, RP.CallInst);
      RP.CallInst->setArgOperand(RP.ResolveArgnum, NewValue);
    } else {
      // has a "deep" resolve point (function pointer in struct) - patch it
      IRBuilder<> B(RP.CallInst);
      std::vector<Value *> GepIndices;
      for (unsigned I = 0; I < DeepMetadata->getNumOperands(); I++) {
        int Idx = std::stoi(cast<MDString>(DeepMetadata->getOperand(I))->getString().str());
        GepIndices.push_back(ConstantInt::get(Type::getInt32Ty(Context), Idx));
      }
      auto *Addr = B.CreateGEP(RP.CallInst->getArgOperand(RP.ResolveArgnum), GepIndices);
      // load ID, translate, store back
      auto *OldValue = B.CreateLoad(Addr);
      auto *NewValue = generateBackTranslation(RP, OldValue, RP.CallInst);
      B.SetInsertPoint(RP.CallInst);
      B.CreateStore(NewValue, Addr);
      // reset after call
      B.SetInsertPoint(RP.CallInst->getNextNode());
      B.CreateStore(OldValue, Addr);
    }
  }
}

llvm::Value * LLVMDispatcherBuilder::generateBackTranslation(DResolvePoint &RP, llvm::Value *OldValue, llvm::Instruction *SplitBefore) {
  auto *F = RP.CallInst->getFunction();
  llvm::PointerType *T = cast<PointerType>(OldValue->getType());

  // build a switch case
  auto *FirstBB = RP.CallInst->getParent();
  auto *SecondBB = FirstBB->splitBasicBlock(SplitBefore);
  auto *DefaultBB = BasicBlock::Create(Context, "", F);
  FirstBB->getTerminator()->eraseFromParent();
  IRBuilder<> Builder(FirstBB);
  auto *FunctionID = Builder.CreatePtrToInt(OldValue, IntPtr);
  auto *Switch = Builder.CreateSwitch(FunctionID, DefaultBB, RP.Targets.size() + Settings.enforce_min_id);
  Builder.SetInsertPoint(SecondBB, SecondBB->begin());
  llvm::PHINode *Phi = Builder.CreatePHI(T, RP.Targets.size() + 1);

  // create a case for each "constant" like SIG_IGN (=1)...
  auto *ConstantFPBB = BasicBlock::Create(Context, "", F);
  Builder.SetInsertPoint(ConstantFPBB);
  Phi->addIncoming(OldValue, ConstantFPBB);
  Builder.CreateBr(SecondBB);
  for (long ID = 0; ID < (long) Settings.enforce_min_id; ID++) {
    Switch->addCase(ConstantInt::get(IntPtr, ID), ConstantFPBB);
  }
  // create a case for each function
  for (auto *Target: RP.Targets) {
    auto *BB = BasicBlock::Create(Context, "", F);
    Builder.SetInsertPoint(BB);
    if (Target->Ref) {
      Phi->addIncoming(castTo(Builder, Target->Ref, Phi->getType()), BB);
    } else {
      Phi->addIncoming(castTo(Builder, ConstantPointerNull::get(T), Phi->getType()), BB);
    }
    Builder.CreateBr(SecondBB);
    // Add to outer switch-case struct
    Switch->addCase(ConstantInt::get(IntPtr, Target->ID), BB);
  }

  // create the default case handler - terminate when no valid function ID is found (or call to libtypro-rt)
  Builder.SetInsertPoint(DefaultBB);
  if (RP.IsExternal) {
    auto *I8Ptr = Type::getInt8PtrTy(RP.CallInst->getContext());
    auto Resolver = RP.CallInst->getModule()->getOrInsertFunction("__tg_resolve_symbol", FunctionType::get(I8Ptr, {I8Ptr, IntPtr}, false));
    auto *Name = Builder.CreateGlobalStringPtr(*RP.CallName);
    auto *ResolvedPtr = Builder.CreateCall(Resolver, {Name, FunctionID});
    Phi->addIncoming(castTo(Builder, ResolvedPtr, Phi->getType()), DefaultBB);
    Builder.CreateBr(SecondBB);
  } else {
    // not external - terminate when no valid function ID is found
    generateErrorCase(Builder, RP, FunctionID);
  }

  return Phi;
}

void LLVMDispatcherBuilder::generateErrorCase(IRBuilder<> &Builder, DResolvePoint &RP, Value *FunctionID) {
  if (Settings.enforce_debug) {
    auto *PtrType = Type::getInt8PtrTy(Context);
    auto *Stderr = M.getOrInsertGlobal("stderr", PtrType);
    auto Fprintf = M.getOrInsertFunction("fprintf", FunctionType::get(Type::getInt32Ty(Context), {PtrType, PtrType}, true));
    auto Msg = "[TG-Enforce] Invalid call target detected! Call '"+*RP.CallName+"' with target %ld (=%p) (valid targets: ";
    for (auto *Target : RP.Targets) {
      Msg += std::to_string(Target->ID) + ",";
    }
    Msg.back() = ')';
    Msg += "\n";
    auto *MsgPtr = Builder.CreateGlobalStringPtr(Msg);
    Builder.CreateCall(Fprintf, {Builder.CreateLoad(Stderr), MsgPtr, FunctionID, FunctionID})
        ->setDebugLoc(RP.CallInst->getDebugLoc());
  }
  Builder.CreateIntrinsic(Intrinsic::trap, {}, {})->setDebugLoc(RP.CallInst->getDebugLoc());
  Builder.CreateUnreachable();
}

void LLVMDispatcherBuilder::printFunctionIDs() {
  for (auto &It: Functions) {
    if (It.second->Removed || !It.second->Ref)
      continue;
    llvm::errs() << " - [ID " << It.second->ID << "] " << It.second->Ref->getName();
    if (!It.second->Leaking)
      llvm::errs() << " (internal)";
    llvm::errs() << "\n";
  }
}

void LLVMDispatcherBuilder::writeFunctionIDsToFile() {
  std::error_code EC;
  llvm::raw_fd_ostream File(Settings.output_filename + "-ids.txt", EC);
  for (auto &It: Functions) {
    if (It.second->Removed || !It.second->Ref)
      continue;
    File << " - [ID " << It.second->ID << "] " << It.second->Ref->getName();
    if (!It.second->Leaking)
      File << " (internal)";
    File << "\n";
  }
}

namespace {
/**
 * Add "" if any other value is possible
 * @param V
 * @param PotentialValues
 */
void getPotentialStringValues(llvm::Value *V, std::set<std::string> &PotentialValues) {
  if (auto *CDA = dyn_cast<ConstantDataArray>(V)) {
    if (CDA->isCString()) {
      PotentialValues.insert(CDA->getAsCString());
      return;
    }
  }

  if (auto *GV = dyn_cast<GlobalVariable>(V)) {
    getPotentialStringValues(GV->getInitializer(), PotentialValues);
    if (!GV->isConstant()) {
      PotentialValues.insert("");
    }
    return;
  }

  if (auto *GEP = dyn_cast<GEPOperator>(V)) {
    if (GEP->getNumIndices() == 2 && GEP->getType() == Type::getInt8PtrTy(V->getContext())) {
      auto *Op = GEP->getOperand(0);
      if (Op->getType()->isPointerTy() && cast<PointerType>(Op->getType())->getPointerElementType()->isArrayTy()) {
        getPotentialStringValues(Op, PotentialValues);
        return;
      }
    }
  }

  if (auto *SI = dyn_cast<SelectInst>(V)) {
    getPotentialStringValues(SI->getTrueValue(), PotentialValues);
    getPotentialStringValues(SI->getFalseValue(), PotentialValues);
    return;
  }

  llvm::outs() << "TODO UNKNOWN " << *V << "\n";
  PotentialValues.insert("");
}
} // namespace

void LLVMDispatcherBuilder::generateCodeForDynamicSymbols() {
  if (DynamicSymbolCalls.empty())
    return;

  auto *VoidPtr = Type::getInt8PtrTy(M.getContext());
  auto DlsymToId = M.getOrInsertFunction("__tg_dlsym_to_id", VoidPtr, VoidPtr, VoidPtr);
  for (auto &It: DynamicSymbolCalls) {
    // make global pointer storage
    auto *GVPtr = M.getOrInsertGlobal("__tg_dlsym_" + *It.second, VoidPtr);
    if (auto *GV = dyn_cast<GlobalVariable>(GVPtr)) {
      GV->setLinkage(llvm::GlobalValue::PrivateLinkage);
      GV->setInitializer(ConstantData::getNullValue(VoidPtr));
      newTrampoline(GV);
      ExternalDynamicSymbolInfos[It.second] = GV;
    }

    auto *Call = It.first;
    IRBuilder<> Builder(Call->getNextNode());
    auto *Call2 = Builder.CreateCall(DlsymToId, {Builder.CreateLoad(GVPtr), Call});
    llvm::Value *Result = Call2;
    if (Call2->getType() != Call->getType()) {
      Result = Builder.CreateBitCast(Call2, Call->getType());
    }
    Call->replaceAllUsesWith(Result);
    Call2->setArgOperand(1, Call);

    // Get potential targets for dynamic symbol resolvers (symbol names)
    getPotentialStringValues(Call->getArgOperand(1), DynamicSymbolTargets[It.second]);
    llvm::errs() << "SET from getPotentialStringValues for " << *It.second << ":\n";
    for (auto S: DynamicSymbolTargets[It.second]) {
      llvm::errs() << " - \'" << S << "\'\n";
    }
  }
}

void LLVMDispatcherBuilder::collectFunctionUsages(std::vector<std::pair<llvm::User *, llvm::Value *>> &Usages, llvm::Value *F) {
  for (auto *User: F->users()) {
    if (auto *C = dyn_cast<CallBase>(User)) {
      if (C->getCalledValue() == F)
        continue;
    }
    if (auto *A = dyn_cast<GlobalAlias>(User)) {
      collectFunctionUsages(Usages, A);
      continue;
    }
    if (auto *C = dyn_cast<BitCastOperator>(User)) {
      if (!canReplaceAllFunctionUsages(C)) {
        collectFunctionUsages(Usages, C);
        continue;
      }
    }
    Usages.push_back({User, F});
  }
}

bool LLVMDispatcherBuilder::canReplaceAllFunctionUsages(Value *V) {
  for (auto *User: V->users()) {
    if (auto *C = dyn_cast<CallBase>(User)) {
      if (C->getCalledValue() == V)
        return false;
    }
    // Special case: call (bitcast f, ...)
    if (auto *BCOp = dyn_cast<BitCastOperator>(User)) {
      if (!canReplaceAllFunctionUsages(BCOp))
        return false;
    }
    if (!canReplaceUseLibcSafe(User))
      return false;
    // an alias might be turned into a GOT symbol, so we can't replace the use in it
    if (auto *A = dyn_cast<GlobalAlias>(User)) {
      return false;
    }
  }
  return true;
}

void LLVMDispatcherBuilder::newTrampoline(llvm::GlobalVariable *GV) {
  GV->setSection("tg_trampolines");
  if (ExternalCallDispatchers.empty() && ExternalDynamicSymbolInfos.empty()) {
    GV->setAlignment(MaybeAlign(4096));
  }
}

void LLVMDispatcherBuilder::allTrampolinesGenerated() {
  if (!ExternalCallDispatchers.empty() || !ExternalDynamicSymbolInfos.empty()) {
    auto *GVPtr = M.getOrInsertGlobal("__tg_end_of_trampolines", Type::getInt8Ty(M.getContext()));
    auto *GV = cast<GlobalVariable>(GVPtr);
    // GV->setVisibility(llvm::GlobalValue::HiddenVisibility);
    // GV->setLinkage(llvm::GlobalValue::PrivateLinkage);
    GV->setInitializer(ConstantData::getNullValue(Type::getInt8Ty(M.getContext())));
    GV->setSection("tg_trampolines");
    GV->setAlignment(MaybeAlign(4096));
  }
}

#endif

} // namespace typegraph
