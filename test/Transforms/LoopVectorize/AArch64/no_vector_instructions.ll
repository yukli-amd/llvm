; REQUIRES: asserts
; RUN: opt < %s -loop-vectorize -force-vector-interleave=1 -S -debug-only=loop-vectorize 2>&1 | FileCheck %s

target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64--linux-gnu"

; CHECK-LABEL: all_scalar
; CHECK:       LV: Found scalar instruction: %i.next = add nuw nsw i64 %i, 2
; CHECK:       LV: Found an estimated cost of 2 for VF 2 For instruction: %i.next = add nuw nsw i64 %i, 2
; CHECK:       LV: Not considering vector loop of width 2 because it will not generate any vector instructions
;
define void @all_scalar(i64* %a, i64 %n) {
entry:
  br label %for.body

for.body:
  %i = phi i64 [ 0, %entry ], [ %i.next, %for.body ]
  %tmp0 = getelementptr i64, i64* %a, i64 %i
  store i64 0, i64* %tmp0, align 1
  %i.next = add nuw nsw i64 %i, 2
  %cond = icmp eq i64 %i.next, %n
  br i1 %cond, label %for.end, label %for.body

for.end:
  ret void
}
