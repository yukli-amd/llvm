//===- llvm/Analysis/LegacyDivergenceAnalysis.h - KernelDivergence Analysis -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// Modifications Copyright (c) 2019 Advanced Micro Devices, Inc. All rights reserved.
// Notified per clause 4(b) of the license.
//
//===----------------------------------------------------------------------===//
//
// The kernel divergence analysis is an LLVM pass which can be used to find out
// if a branch instruction in a GPU program (kernel) is divergent or not. It can help
// branch optimizations such as jump threading and loop unswitching to make
// better decisions.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_ANALYSIS_LEGACY_DIVERGENCE_ANALYSIS_H
#define LLVM_ANALYSIS_LEGACY_DIVERGENCE_ANALYSIS_H

#include "llvm/ADT/DenseSet.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/DivergenceAnalysis.h"

namespace llvm {
class Value;
class GPUDivergenceAnalysis;
class LegacyDivergenceAnalysis : public FunctionPass {
public:
  static char ID;

  LegacyDivergenceAnalysis() : FunctionPass(ID) {
    initializeLegacyDivergenceAnalysisPass(*PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override;

  bool runOnFunction(Function &F) override;

  // Print all divergent branches in the function.
  void print(raw_ostream &OS, const Module *) const override;

  // Returns true if V is divergent at its definition.
  bool isDivergent(const Value *V) const;

  // Returns true if U is divergent. Uses of a uniform value can be divergent.
  bool isDivergentUse(const Use *U) const;

  // Returns true if V is uniform/non-divergent.
  bool isUniform(const Value *V) const { return !isDivergent(V); }

  // Returns true if U is uniform/non-divergent. Uses of a uniform value can be
  // divergent.
  bool isUniformUse(const Use *U) const { return !isDivergentUse(U); }

  // Keep the analysis results uptodate by removing an erased value.
  void removeValue(const Value *V) { DivergentValues.erase(V); }

private:
  // Whether analysis should be performed by GPUDivergenceAnalysis.
  bool shouldUseGPUDivergenceAnalysis(const Function &F) const;

  // (optional) handle to new DivergenceAnalysis
  std::unique_ptr<GPUDivergenceAnalysis> gpuDA;

  // Stores all divergent values.
  DenseSet<const Value *> DivergentValues;

  // Stores divergent uses of possibly uniform values.
  DenseSet<const Use *> DivergentUses;
};
} // End llvm namespace

#endif //LLVM_ANALYSIS_LEGACY_DIVERGENCE_ANALYSIS_H
