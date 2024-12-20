//===-- llvm/CodeGen/LowLevelType.cpp -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
/// \file This file implements the more header-heavy bits of the LLT class to
/// avoid polluting users' namespaces.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/LowLevelType.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

LLT::LLT(MVT VT) {
  if (VT.isMatrix()) {
    bool asMatrix = (VT.getMatrixNumElements() > 1 && VT.getMatrixNumElements() > 1) || VT.isScalableMatrix();
    init(/*IsPointer=*/false, asMatrix, /*IsVector=*/false, /*IsScalar=*/!asMatrix,
         ElementCount::getFixed(0), VT.getMatrixElementType().getSizeInBits(),
         /*AddressSpace=*/0, VT.getMatrixNumElements(), VT.getMatrixNumElements2(), VT.isScalableMatrix());
  } else if (VT.isVector()) {
    bool asVector = VT.getVectorMinNumElements() > 1 || VT.isScalableVector();
    init(/*IsPointer=*/false, /*IsMatrix=*/false, asVector, /*IsScalar=*/!asVector,
         VT.getVectorElementCount(), VT.getVectorElementType().getSizeInBits(),
         /*AddressSpace=*/0);
  } else if (VT.isValid() && !VT.isScalableTargetExtVT()) {
    // Aggregates are no different from real scalars as far as GlobalISel is
    // concerned.
    init(/*IsPointer=*/false, /*IsMatrix=*/false, /*IsVector=*/false, /*IsScalar=*/true,
         ElementCount::getFixed(0), VT.getSizeInBits(), /*AddressSpace=*/0);
  } else {
    IsScalar = false;
    IsPointer = false;
    IsVector = false;
    IsMatrix = false;
    RawData = 0;
  }
}

void LLT::print(raw_ostream &OS) const {
  if (isMatrix()) {
    OS << "<";
    OS << getElementCount() << " x " << getElementType() << ">";
  } else if (isVector()) {
    OS << "<";
    OS << getElementCount() << " x " << getElementType() << ">";
  } else if (isPointer())
    OS << "p" << getAddressSpace();
  else if (isValid()) {
    assert(isScalar() && "unexpected type");
    OS << "s" << getScalarSizeInBits();
  } else
    OS << "LLT_invalid";
}

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
LLVM_DUMP_METHOD void LLT::dump() const {
  print(dbgs());
  dbgs() << '\n';
}
#endif

const constexpr LLT::BitFieldInfo LLT::ScalarSizeFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerSizeFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerAddressSpaceFieldInfo;
const constexpr LLT::BitFieldInfo LLT::VectorElementsFieldInfo;
const constexpr LLT::BitFieldInfo LLT::VectorScalableFieldInfo;
const constexpr LLT::BitFieldInfo LLT::VectorSizeFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerVectorElementsFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerVectorScalableFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerVectorSizeFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerVectorAddressSpaceFieldInfo;
const constexpr LLT::BitFieldInfo LLT::MatrixElementsFieldInfo;
const constexpr LLT::BitFieldInfo LLT::MatrixElements2FieldInfo;
const constexpr LLT::BitFieldInfo LLT::MatrixSizeFieldInfo;
const constexpr LLT::BitFieldInfo LLT::MatrixScalableFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerMatrixElementsFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerMatrixElements2FieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerMatrixSizeFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerMatrixAddressSpaceFieldInfo;
const constexpr LLT::BitFieldInfo LLT::PointerMatrixScalableFieldInfo;