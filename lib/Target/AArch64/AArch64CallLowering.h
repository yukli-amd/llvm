//===- AArch64CallLowering.h - Call lowering --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file describes how to lower LLVM calls to machine code calls.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AARCH64_AARCH64CALLLOWERING_H
#define LLVM_LIB_TARGET_AARCH64_AARCH64CALLLOWERING_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/CodeGen/GlobalISel/CallLowering.h"
#include "llvm/IR/CallingConv.h"
#include <cstdint>
#include <functional>

namespace llvm {

class AArch64TargetLowering;
class CCValAssign;
class DataLayout;
class MachineIRBuilder;
class MachineRegisterInfo;
class Type;

class AArch64CallLowering: public CallLowering {
public:
  AArch64CallLowering(const AArch64TargetLowering &TLI);

  bool lowerReturn(MachineIRBuilder &MIRBuilder, const Value *Val,
                   ArrayRef<Register> VRegs,
                   Register SwiftErrorVReg) const override;

  bool lowerFormalArguments(MachineIRBuilder &MIRBuilder, const Function &F,
                            ArrayRef<ArrayRef<Register>> VRegs) const override;

  bool lowerCall(MachineIRBuilder &MIRBuilder,
                 CallLoweringInfo &Info) const override;

  /// Returns true if the call can be lowered as a tail call.
  bool
  isEligibleForTailCallOptimization(MachineIRBuilder &MIRBuilder,
                                    CallLoweringInfo &Info,
                                    SmallVectorImpl<ArgInfo> &InArgs) const;

  bool supportSwiftError() const override { return true; }

private:
  using RegHandler = std::function<void(MachineIRBuilder &, Type *, unsigned,
                                        CCValAssign &)>;

  using MemHandler =
      std::function<void(MachineIRBuilder &, int, CCValAssign &)>;

  void splitToValueTypes(const ArgInfo &OrigArgInfo,
                         SmallVectorImpl<ArgInfo> &SplitArgs,
                         const DataLayout &DL, MachineRegisterInfo &MRI,
                         CallingConv::ID CallConv) const;

  bool
  doCallerAndCalleePassArgsTheSameWay(CallLoweringInfo &Info,
                                      MachineFunction &MF,
                                      SmallVectorImpl<ArgInfo> &InArgs) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AARCH64_AARCH64CALLLOWERING_H
