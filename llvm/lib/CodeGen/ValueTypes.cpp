//===----------- ValueTypes.cpp - Implementation of EVT methods -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TypeSize.h"
#include "llvm/Support/WithColor.h"
using namespace llvm;

EVT EVT::changeExtendedTypeToInteger() const {
  assert(isExtended() && "Type is not extended!");
  LLVMContext &Context = LLVMTy->getContext();
  return getIntegerVT(Context, getSizeInBits());
}

EVT EVT::changeExtendedVectorElementTypeToInteger() const {
  assert(isExtended() && "Type is not extended!");
  LLVMContext &Context = LLVMTy->getContext();
  EVT IntTy = getIntegerVT(Context, getScalarSizeInBits());
  return getVectorVT(Context, IntTy, getVectorElementCount());
}

EVT EVT::changeExtendedVectorElementType(EVT EltVT) const {
  assert(isExtended() && "Type is not extended!");
  LLVMContext &Context = LLVMTy->getContext();
  return getVectorVT(Context, EltVT, getVectorElementCount());
}

EVT EVT::getExtendedIntegerVT(LLVMContext &Context, unsigned BitWidth) {
  EVT VT;
  VT.LLVMTy = IntegerType::get(Context, BitWidth);
  assert(VT.isExtended() && "Type is not extended!");
  return VT;
}

EVT EVT::getExtendedVectorVT(LLVMContext &Context, EVT VT, unsigned NumElements,
                             bool IsScalable) {
  EVT ResultVT;
  ResultVT.LLVMTy =
      VectorType::get(VT.getTypeForEVT(Context), NumElements, IsScalable);
  assert(ResultVT.isExtended() && "Type is not extended!");
  return ResultVT;
}

EVT EVT::getExtendedVectorVT(LLVMContext &Context, EVT VT, ElementCount EC) {
  EVT ResultVT;
  ResultVT.LLVMTy = VectorType::get(VT.getTypeForEVT(Context), EC);
  assert(ResultVT.isExtended() && "Type is not extended!");
  return ResultVT;
}

#ifdef SCALABLE_MATRIX
EVT EVT::getExtendedMatrixVT(LLVMContext &Context, EVT VT, unsigned NumElements,
                             unsigned NumElements2, bool IsScalable) {
  EVT ResultVT;
  ResultVT.LLVMTy = ScalableMatrixType::get(
      VT.getTypeForEVT(Context), NumElements, NumElements2, IsScalable);
  assert(ResultVT.isExtended() && "Type is not extended!");
  return ResultVT;
}
#endif

bool EVT::isExtendedFloatingPoint() const {
  assert(isExtended() && "Type is not extended!");
  return LLVMTy->isFPOrFPVectorTy();
}

bool EVT::isExtendedInteger() const {
  assert(isExtended() && "Type is not extended!");
  return LLVMTy->isIntOrIntVectorTy();
}

bool EVT::isExtendedScalarInteger() const {
  assert(isExtended() && "Type is not extended!");
  return LLVMTy->isIntegerTy();
}

bool EVT::isExtendedVector() const {
  assert(isExtended() && "Type is not extended!");
  return LLVMTy->isVectorTy();
}

#ifdef SCALABLE_MATRIX
bool EVT::isExtendedMatrix() const {
  assert(isExtended() && "Type is not extended!");
  return LLVMTy->isMatrixTy();
}
#endif

bool EVT::isExtended16BitVector() const {
  return isExtendedVector() &&
         getExtendedSizeInBits() == TypeSize::getFixed(16);
}

bool EVT::isExtended32BitVector() const {
  return isExtendedVector() &&
         getExtendedSizeInBits() == TypeSize::getFixed(32);
}

bool EVT::isExtended64BitVector() const {
  return isExtendedVector() &&
         getExtendedSizeInBits() == TypeSize::getFixed(64);
}

bool EVT::isExtended128BitVector() const {
  return isExtendedVector() &&
         getExtendedSizeInBits() == TypeSize::getFixed(128);
}

bool EVT::isExtended256BitVector() const {
  return isExtendedVector() &&
         getExtendedSizeInBits() == TypeSize::getFixed(256);
}

bool EVT::isExtended512BitVector() const {
  return isExtendedVector() &&
         getExtendedSizeInBits() == TypeSize::getFixed(512);
}

bool EVT::isExtended1024BitVector() const {
  return isExtendedVector() &&
         getExtendedSizeInBits() == TypeSize::getFixed(1024);
}

bool EVT::isExtended2048BitVector() const {
  return isExtendedVector() &&
         getExtendedSizeInBits() == TypeSize::getFixed(2048);
}

bool EVT::isExtendedFixedLengthVector() const {
  return isExtendedVector() && isa<FixedVectorType>(LLVMTy);
}

bool EVT::isExtendedScalableVector() const {
  return isExtendedVector() && isa<ScalableVectorType>(LLVMTy);
}

#ifdef SCALABLE_MATRIX
bool EVT::isExtendedScalableMatrix() const {
  return isExtendedMatrix() && isa<ScalableMatrixType>(LLVMTy);
}
#endif

EVT EVT::getExtendedVectorElementType() const {
  assert(isExtended() && "Type is not extended!");
  return EVT::getEVT(cast<VectorType>(LLVMTy)->getElementType());
}

#ifdef SCALABLE_MATRIX
EVT EVT::getExtendedMatrixElementType() const {
  assert(isExtended() && "Type is not extended!");
  return EVT::getEVT(cast<ScalableMatrixType>(LLVMTy)->getElementType());
}
#endif

unsigned EVT::getExtendedVectorNumElements() const {
  assert(isExtended() && "Type is not extended!");
  ElementCount EC = cast<VectorType>(LLVMTy)->getElementCount();
  if (EC.isScalable()) {
    WithColor::warning()
        << "The code that requested the fixed number of elements has made the "
           "assumption that this vector is not scalable. This assumption was "
           "not correct, and this may lead to broken code\n";
  }
  return EC.getKnownMinValue();
}

ElementCount EVT::getExtendedVectorElementCount() const {
  assert(isExtended() && "Type is not extended!");
  return cast<VectorType>(LLVMTy)->getElementCount();
}

TypeSize EVT::getExtendedSizeInBits() const {
  assert(isExtended() && "Type is not extended!");
  if (IntegerType *ITy = dyn_cast<IntegerType>(LLVMTy))
    return TypeSize::getFixed(ITy->getBitWidth());
  if (VectorType *VTy = dyn_cast<VectorType>(LLVMTy))
    return VTy->getPrimitiveSizeInBits();
  llvm_unreachable("Unrecognized extended type!");
}

/// getEVTString - This function returns value type as a string, e.g. "i32".
std::string EVT::getEVTString() const {
  switch (V.SimpleTy) {
  default:
#ifdef SCALABLE_MATRIX
    if (isMatrix()) {
      std::string result;
      if (isScalableMatrix())
        result = "mx" + utostr(getMatrixNumElems()) + "xnx" +
                 utostr(getMatrixNumElems2()) + "x" +
                 getMatrixElementType().getEVTString();
      else
        result = "m" + utostr(getMatrixNumElems()) + "x" +
                 getMatrixElementType().getEVTString();
      return result;
    }
#endif
    if (isVector())
      return (isScalableVector() ? "nxv" : "v") +
             utostr(getVectorElementCount().getKnownMinValue()) +
             getVectorElementType().getEVTString();
    if (isInteger())
      return "i" + utostr(getSizeInBits());
    if (isFloatingPoint())
      return "f" + utostr(getSizeInBits());
    llvm_unreachable("Invalid EVT!");
#ifdef FP8_DATATYPES
  case MVT::bf8:
    return "bf8";
  case MVT::hf8:
    return "hf8";
#endif
  case MVT::bf16:      return "bf16";
  case MVT::ppcf128:   return "ppcf128";
  case MVT::isVoid:    return "isVoid";
  case MVT::Other:     return "ch";
  case MVT::Glue:      return "glue";
  case MVT::x86mmx:    return "x86mmx";
  case MVT::x86amx:    return "x86amx";
  case MVT::i64x8:     return "i64x8";
  case MVT::Metadata:  return "Metadata";
  case MVT::Untyped:   return "Untyped";
  case MVT::funcref:   return "funcref";
  case MVT::externref: return "externref";
  case MVT::aarch64svcount:
    return "aarch64svcount";
  case MVT::spirvbuiltin:
    return "spirvbuiltin";
  }
}

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
void EVT::dump() const {
  print(dbgs());
  dbgs() << "\n";
}
#endif

/// getTypeForEVT - This method returns an LLVM type corresponding to the
/// specified EVT.  For integer types, this returns an unsigned type.  Note
/// that this will abort for types that cannot be represented.
Type *EVT::getTypeForEVT(LLVMContext &Context) const {
  // clang-format off
  switch (V.SimpleTy) {
  default:
    assert(isExtended() && "Type is not extended!");
    return LLVMTy;
  case MVT::isVoid:  return Type::getVoidTy(Context);
  case MVT::i1:      return Type::getInt1Ty(Context);
  case MVT::i2:      return Type::getIntNTy(Context, 2);
  case MVT::i4:      return Type::getIntNTy(Context, 4);
  case MVT::i8:      return Type::getInt8Ty(Context);
  case MVT::i16:     return Type::getInt16Ty(Context);
  case MVT::i32:     return Type::getInt32Ty(Context);
  case MVT::i64:     return Type::getInt64Ty(Context);
  case MVT::i128:    return IntegerType::get(Context, 128);
  case MVT::f16:     return Type::getHalfTy(Context);
  case MVT::bf16:    return Type::getBFloatTy(Context);
#ifdef FP8_DATATYPES
  case MVT::bf8:     return Type::getBF8Ty(Context);
  case MVT::hf8:     return Type::getHF8Ty(Context);
#endif
  case MVT::f32:     return Type::getFloatTy(Context);
  case MVT::f64:     return Type::getDoubleTy(Context);
  case MVT::f80:     return Type::getX86_FP80Ty(Context);
  case MVT::f128:    return Type::getFP128Ty(Context);
  case MVT::ppcf128: return Type::getPPC_FP128Ty(Context);
  case MVT::x86mmx:  return Type::getX86_MMXTy(Context);
  case MVT::aarch64svcount:
    return TargetExtType::get(Context, "aarch64.svcount");
  case MVT::x86amx:  return Type::getX86_AMXTy(Context);
  case MVT::i64x8:   return IntegerType::get(Context, 512);
  case MVT::externref: return Type::getWasm_ExternrefTy(Context);
  case MVT::funcref: return Type::getWasm_FuncrefTy(Context);
  case MVT::v1i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 1);
  case MVT::v2i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 2);
  case MVT::v4i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 4);
  case MVT::v8i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 8);
  case MVT::v16i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 16);
  case MVT::v32i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 32);
  case MVT::v64i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 64);
  case MVT::v128i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 128);
  case MVT::v256i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 256);
  case MVT::v512i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 512);
  case MVT::v1024i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 1024);
  case MVT::v2048i1:
    return FixedVectorType::get(Type::getInt1Ty(Context), 2048);
  case MVT::v128i2:
    return FixedVectorType::get(Type::getIntNTy(Context, 2), 128);
  case MVT::v256i2:
    return FixedVectorType::get(Type::getIntNTy(Context, 2), 256);
  case MVT::v64i4:
    return FixedVectorType::get(Type::getIntNTy(Context, 4), 64);
  case MVT::v128i4:
    return FixedVectorType::get(Type::getIntNTy(Context, 4), 128);
  case MVT::v1i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 1);
  case MVT::v2i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 2);
  case MVT::v4i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 4);
  case MVT::v8i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 8);
  case MVT::v16i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 16);
  case MVT::v32i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 32);
  case MVT::v64i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 64);
  case MVT::v128i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 128);
  case MVT::v256i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 256);
  case MVT::v512i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 512);
  case MVT::v1024i8:
    return FixedVectorType::get(Type::getInt8Ty(Context), 1024);
  case MVT::v1i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 1);
  case MVT::v2i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 2);
  case MVT::v3i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 3);
  case MVT::v4i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 4);
  case MVT::v8i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 8);
  case MVT::v16i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 16);
  case MVT::v32i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 32);
  case MVT::v64i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 64);
  case MVT::v128i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 128);
  case MVT::v256i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 256);
  case MVT::v512i16:
    return FixedVectorType::get(Type::getInt16Ty(Context), 512);
  case MVT::v1i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 1);
  case MVT::v2i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 2);
  case MVT::v3i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 3);
  case MVT::v4i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 4);
  case MVT::v5i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 5);
  case MVT::v6i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 6);
  case MVT::v7i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 7);
  case MVT::v8i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 8);
  case MVT::v9i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 9);
  case MVT::v10i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 10);
  case MVT::v11i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 11);
  case MVT::v12i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 12);
  case MVT::v16i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 16);
  case MVT::v32i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 32);
  case MVT::v64i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 64);
  case MVT::v128i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 128);
  case MVT::v256i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 256);
  case MVT::v512i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 512);
  case MVT::v1024i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 1024);
  case MVT::v2048i32:
    return FixedVectorType::get(Type::getInt32Ty(Context), 2048);
  case MVT::v1i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 1);
  case MVT::v2i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 2);
  case MVT::v3i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 3);
  case MVT::v4i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 4);
  case MVT::v8i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 8);
  case MVT::v16i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 16);
  case MVT::v32i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 32);
  case MVT::v64i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 64);
  case MVT::v128i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 128);
  case MVT::v256i64:
    return FixedVectorType::get(Type::getInt64Ty(Context), 256);
  case MVT::v1i128:
    return FixedVectorType::get(Type::getInt128Ty(Context), 1);
  case MVT::v1f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 1);
  case MVT::v2f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 2);
  case MVT::v3f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 3);
  case MVT::v4f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 4);
  case MVT::v8f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 8);
  case MVT::v16f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 16);
  case MVT::v32f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 32);
  case MVT::v64f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 64);
  case MVT::v128f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 128);
  case MVT::v256f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 256);
  case MVT::v512f16:
    return FixedVectorType::get(Type::getHalfTy(Context), 512);
  case MVT::v2bf16:
    return FixedVectorType::get(Type::getBFloatTy(Context), 2);
  case MVT::v3bf16:
    return FixedVectorType::get(Type::getBFloatTy(Context), 3);
  case MVT::v4bf16:
    return FixedVectorType::get(Type::getBFloatTy(Context), 4);
  case MVT::v8bf16:
    return FixedVectorType::get(Type::getBFloatTy(Context), 8);
  case MVT::v16bf16:
    return FixedVectorType::get(Type::getBFloatTy(Context), 16);
  case MVT::v32bf16:
    return FixedVectorType::get(Type::getBFloatTy(Context), 32);
  case MVT::v64bf16:
    return FixedVectorType::get(Type::getBFloatTy(Context), 64);
  case MVT::v128bf16:
    return FixedVectorType::get(Type::getBFloatTy(Context), 128);
  case MVT::v1f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 1);
  case MVT::v2f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 2);
  case MVT::v3f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 3);
  case MVT::v4f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 4);
  case MVT::v5f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 5);
  case MVT::v6f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 6);
  case MVT::v7f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 7);
  case MVT::v8f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 8);
  case MVT::v9f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 9);
  case MVT::v10f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 10);
  case MVT::v11f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 11);
  case MVT::v12f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 12);
  case MVT::v16f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 16);
  case MVT::v32f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 32);
  case MVT::v64f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 64);
  case MVT::v128f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 128);
  case MVT::v256f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 256);
  case MVT::v512f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 512);
  case MVT::v1024f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 1024);
  case MVT::v2048f32:
    return FixedVectorType::get(Type::getFloatTy(Context), 2048);
  case MVT::v1f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 1);
  case MVT::v2f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 2);
  case MVT::v3f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 3);
  case MVT::v4f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 4);
  case MVT::v8f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 8);
  case MVT::v16f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 16);
  case MVT::v32f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 32);
  case MVT::v64f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 64);
  case MVT::v128f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 128);
  case MVT::v256f64:
    return FixedVectorType::get(Type::getDoubleTy(Context), 256);
  case MVT::nxv1i1:
    return ScalableVectorType::get(Type::getInt1Ty(Context), 1);
  case MVT::nxv2i1:
    return ScalableVectorType::get(Type::getInt1Ty(Context), 2);
  case MVT::nxv4i1:
    return ScalableVectorType::get(Type::getInt1Ty(Context), 4);
  case MVT::nxv8i1:
    return ScalableVectorType::get(Type::getInt1Ty(Context), 8);
  case MVT::nxv16i1:
    return ScalableVectorType::get(Type::getInt1Ty(Context), 16);
  case MVT::nxv32i1:
    return ScalableVectorType::get(Type::getInt1Ty(Context), 32);
  case MVT::nxv64i1:
    return ScalableVectorType::get(Type::getInt1Ty(Context), 64);
  case MVT::nxv1i8:
    return ScalableVectorType::get(Type::getInt8Ty(Context), 1);
  case MVT::nxv2i8:
    return ScalableVectorType::get(Type::getInt8Ty(Context), 2);
  case MVT::nxv4i8:
    return ScalableVectorType::get(Type::getInt8Ty(Context), 4);
  case MVT::nxv8i8:
    return ScalableVectorType::get(Type::getInt8Ty(Context), 8);
  case MVT::nxv16i8:
    return ScalableVectorType::get(Type::getInt8Ty(Context), 16);
  case MVT::nxv32i8:
    return ScalableVectorType::get(Type::getInt8Ty(Context), 32);
  case MVT::nxv64i8:
    return ScalableVectorType::get(Type::getInt8Ty(Context), 64);
  case MVT::nxv1i16:
    return ScalableVectorType::get(Type::getInt16Ty(Context), 1);
  case MVT::nxv2i16:
    return ScalableVectorType::get(Type::getInt16Ty(Context), 2);
  case MVT::nxv4i16:
    return ScalableVectorType::get(Type::getInt16Ty(Context), 4);
  case MVT::nxv8i16:
    return ScalableVectorType::get(Type::getInt16Ty(Context), 8);
  case MVT::nxv16i16:
    return ScalableVectorType::get(Type::getInt16Ty(Context), 16);
  case MVT::nxv32i16:
    return ScalableVectorType::get(Type::getInt16Ty(Context), 32);
  case MVT::nxv1i32:
    return ScalableVectorType::get(Type::getInt32Ty(Context), 1);
  case MVT::nxv2i32:
    return ScalableVectorType::get(Type::getInt32Ty(Context), 2);
  case MVT::nxv4i32:
    return ScalableVectorType::get(Type::getInt32Ty(Context), 4);
  case MVT::nxv8i32:
    return ScalableVectorType::get(Type::getInt32Ty(Context), 8);
  case MVT::nxv16i32:
    return ScalableVectorType::get(Type::getInt32Ty(Context), 16);
  case MVT::nxv32i32:
    return ScalableVectorType::get(Type::getInt32Ty(Context), 32);
  case MVT::nxv1i64:
    return ScalableVectorType::get(Type::getInt64Ty(Context), 1);
  case MVT::nxv2i64:
    return ScalableVectorType::get(Type::getInt64Ty(Context), 2);
  case MVT::nxv4i64:
    return ScalableVectorType::get(Type::getInt64Ty(Context), 4);
  case MVT::nxv8i64:
    return ScalableVectorType::get(Type::getInt64Ty(Context), 8);
  case MVT::nxv16i64:
    return ScalableVectorType::get(Type::getInt64Ty(Context), 16);
  case MVT::nxv32i64:
    return ScalableVectorType::get(Type::getInt64Ty(Context), 32);
  case MVT::nxv1f16:
    return ScalableVectorType::get(Type::getHalfTy(Context), 1);
  case MVT::nxv2f16:
    return ScalableVectorType::get(Type::getHalfTy(Context), 2);
  case MVT::nxv4f16:
    return ScalableVectorType::get(Type::getHalfTy(Context), 4);
  case MVT::nxv8f16:
    return ScalableVectorType::get(Type::getHalfTy(Context), 8);
  case MVT::nxv16f16:
    return ScalableVectorType::get(Type::getHalfTy(Context), 16);
  case MVT::nxv32f16:
    return ScalableVectorType::get(Type::getHalfTy(Context), 32);
  case MVT::nxv1bf16:
    return ScalableVectorType::get(Type::getBFloatTy(Context), 1);
  case MVT::nxv2bf16:
    return ScalableVectorType::get(Type::getBFloatTy(Context), 2);
  case MVT::nxv4bf16:
    return ScalableVectorType::get(Type::getBFloatTy(Context), 4);
  case MVT::nxv8bf16:
    return ScalableVectorType::get(Type::getBFloatTy(Context), 8);
  case MVT::nxv16bf16:
    return ScalableVectorType::get(Type::getBFloatTy(Context), 16);
  case MVT::nxv32bf16:
    return ScalableVectorType::get(Type::getBFloatTy(Context), 32);
  case MVT::nxv1f32:
    return ScalableVectorType::get(Type::getFloatTy(Context), 1);
  case MVT::nxv2f32:
    return ScalableVectorType::get(Type::getFloatTy(Context), 2);
  case MVT::nxv4f32:
    return ScalableVectorType::get(Type::getFloatTy(Context), 4);
  case MVT::nxv8f32:
    return ScalableVectorType::get(Type::getFloatTy(Context), 8);
  case MVT::nxv16f32:
    return ScalableVectorType::get(Type::getFloatTy(Context), 16);
  case MVT::nxv1f64:
    return ScalableVectorType::get(Type::getDoubleTy(Context), 1);
  case MVT::nxv2f64:
    return ScalableVectorType::get(Type::getDoubleTy(Context), 2);
  case MVT::nxv4f64:
    return ScalableVectorType::get(Type::getDoubleTy(Context), 4);
  case MVT::nxv8f64:
    return ScalableVectorType::get(Type::getDoubleTy(Context), 8);
#ifdef FP8_DATATYPES
  case MVT::nxv1bf8:
    return ScalableVectorType::get(Type::getBF8Ty(Context), 1);
  case MVT::nxv2bf8:
    return ScalableVectorType::get(Type::getBF8Ty(Context), 2);
  case MVT::nxv4bf8:
    return ScalableVectorType::get(Type::getBF8Ty(Context), 4);
  case MVT::nxv8bf8:
    return ScalableVectorType::get(Type::getBF8Ty(Context), 8);
  case MVT::nxv16bf8:
    return ScalableVectorType::get(Type::getBF8Ty(Context), 16);
  case MVT::nxv32bf8:
    return ScalableVectorType::get(Type::getBF8Ty(Context), 32);
  case MVT::nxv64bf8:
    return ScalableVectorType::get(Type::getBF8Ty(Context), 64);
  case MVT::nxv1hf8:
    return ScalableVectorType::get(Type::getHF8Ty(Context), 1);
  case MVT::nxv2hf8:
    return ScalableVectorType::get(Type::getHF8Ty(Context), 2);
  case MVT::nxv4hf8:
    return ScalableVectorType::get(Type::getHF8Ty(Context), 4);
  case MVT::nxv8hf8:
    return ScalableVectorType::get(Type::getHF8Ty(Context), 8);
  case MVT::nxv16hf8:
    return ScalableVectorType::get(Type::getHF8Ty(Context), 16);
  case MVT::nxv32hf8:
    return ScalableVectorType::get(Type::getHF8Ty(Context), 32);
  case MVT::nxv64hf8:
    return ScalableVectorType::get(Type::getHF8Ty(Context), 64);
#endif
#ifdef SCALABLE_MATRIX
  case MVT::m1x1xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 1, false);
  case MVT::m1x2xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 2, false);
  case MVT::m1x4xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 4, false);
  case MVT::m1x8xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 8, false);
  case MVT::m1x16xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 16, false);
  case MVT::m1x32xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 32, false);
  case MVT::m1x64xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 64, false);
  case MVT::m1x1xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 1, false);
  case MVT::m1x2xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 2, false);
  case MVT::m1x4xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 4, false);
  case MVT::m1x8xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 8, false);
  case MVT::m1x16xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 16, false);
  case MVT::m1x32xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 32, false);
  case MVT::m1x1xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 1, false);
  case MVT::m1x2xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 2, false);
  case MVT::m1x4xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 4, false);
  case MVT::m1x8xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 8, false);
  case MVT::m1x16xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 16, false);
  case MVT::m1x1xi64:
    return ScalableMatrixType::get(Type::getInt64Ty(Context), 1, 1, false);
  case MVT::m1x2xi64:
    return ScalableMatrixType::get(Type::getInt64Ty(Context), 1, 2, false);
  case MVT::m1x4xi64:
    return ScalableMatrixType::get(Type::getInt64Ty(Context), 1, 4, false);
  case MVT::m1x8xi64:
    return ScalableMatrixType::get(Type::getInt64Ty(Context), 1, 8, false);
#ifdef FP8_DATATYPES
  case MVT::m1x1xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 1, false);
  case MVT::m1x2xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 2, false);
  case MVT::m1x4xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 4, false);
  case MVT::m1x8xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 8, false);
  case MVT::m1x16xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 16, false);
  case MVT::m1x32xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 32, false);
  case MVT::m1x64xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 64, false);
  case MVT::m1x1xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 1, false);
  case MVT::m1x2xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 2, false);
  case MVT::m1x4xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 4, false);
  case MVT::m1x8xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 8, false);
  case MVT::m1x16xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 16, false);
  case MVT::m1x32xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 32, false);
  case MVT::m1x64xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 64, false);
#endif
  case MVT::m1x1xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 1, false);
  case MVT::m1x2xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 2, false);
  case MVT::m1x4xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 4, false);
  case MVT::m1x8xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 8, false);
  case MVT::m1x16xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 16, false);
  case MVT::m1x32xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 32, false);
  case MVT::m1x1xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 1, false);
  case MVT::m1x2xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 2, false);
  case MVT::m1x4xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 4, false);
  case MVT::m1x8xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 8, false);
  case MVT::m1x16xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 16, false);
  case MVT::m1x32xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 32, false);
  case MVT::m1x1xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 1, false);
  case MVT::m1x2xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 2, false);
  case MVT::m1x4xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 4, false);
  case MVT::m1x8xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 8, false);
  case MVT::m1x16xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 16, false);
  case MVT::m1x1xf64:
    return ScalableMatrixType::get(Type::getDoubleTy(Context), 1, 1, false);
  case MVT::m1x2xf64:
    return ScalableMatrixType::get(Type::getDoubleTy(Context), 1, 2, false);
  case MVT::m1x4xf64:
    return ScalableMatrixType::get(Type::getDoubleTy(Context), 1, 4, false);
  case MVT::m1x8xf64:
    return ScalableMatrixType::get(Type::getDoubleTy(Context), 1, 8, false);
  case MVT::mx1xnx1xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 1, true);
  case MVT::mx1xnx2xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 2, true);
  case MVT::mx1xnx4xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 4, true);
  case MVT::mx1xnx8xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 8, true);
  case MVT::mx1xnx16xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 16, true);
  case MVT::mx1xnx32xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 32, true);
  case MVT::mx1xnx64xi8:
    return ScalableMatrixType::get(Type::getInt8Ty(Context), 1, 64, true);
  case MVT::mx1xnx1xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 1, true);
  case MVT::mx1xnx2xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 2, true);
  case MVT::mx1xnx4xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 4, true);
  case MVT::mx1xnx8xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 8, true);
  case MVT::mx1xnx16xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 16, true);
  case MVT::mx1xnx32xi16:
    return ScalableMatrixType::get(Type::getInt16Ty(Context), 1, 32, true);
  case MVT::mx1xnx1xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 1, true);
  case MVT::mx1xnx2xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 2, true);
  case MVT::mx1xnx4xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 4, true);
  case MVT::mx1xnx8xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 8, true);
  case MVT::mx1xnx16xi32:
    return ScalableMatrixType::get(Type::getInt32Ty(Context), 1, 16, true);
  case MVT::mx1xnx1xi64:
    return ScalableMatrixType::get(Type::getInt64Ty(Context), 1, 1, true);
  case MVT::mx1xnx2xi64:
    return ScalableMatrixType::get(Type::getInt64Ty(Context), 1, 2, true);
  case MVT::mx1xnx4xi64:
    return ScalableMatrixType::get(Type::getInt64Ty(Context), 1, 4, true);
  case MVT::mx1xnx8xi64:
    return ScalableMatrixType::get(Type::getInt64Ty(Context), 1, 8, true);
#ifdef FP8_DATATYPES
  case MVT::mx1xnx1xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 1, true);
  case MVT::mx1xnx2xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 2, true);
  case MVT::mx1xnx4xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 4, true);
  case MVT::mx1xnx8xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 8, true);
  case MVT::mx1xnx16xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 16, true);
  case MVT::mx1xnx32xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 32, true);
  case MVT::mx1xnx64xbf8:
    return ScalableMatrixType::get(Type::getBF8Ty(Context), 1, 64, true);
  case MVT::mx1xnx1xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 1, true);
  case MVT::mx1xnx2xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 2, true);
  case MVT::mx1xnx4xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 4, true);
  case MVT::mx1xnx8xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 8, true);
  case MVT::mx1xnx16xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 16, true);
  case MVT::mx1xnx32xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 32, true);
  case MVT::mx1xnx64xhf8:
    return ScalableMatrixType::get(Type::getHF8Ty(Context), 1, 64, true);
#endif
  case MVT::mx1xnx1xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 1, true);
  case MVT::mx1xnx2xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 2, true);
  case MVT::mx1xnx4xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 4, true);
  case MVT::mx1xnx8xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 8, true);
  case MVT::mx1xnx16xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 16, true);
  case MVT::mx1xnx32xbf16:
    return ScalableMatrixType::get(Type::getBFloatTy(Context), 1, 32, true);
  case MVT::mx1xnx1xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 1, true);
  case MVT::mx1xnx2xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 2, true);
  case MVT::mx1xnx4xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 4, true);
  case MVT::mx1xnx8xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 8, true);
  case MVT::mx1xnx16xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 16, true);
  case MVT::mx1xnx32xf16:
    return ScalableMatrixType::get(Type::getHalfTy(Context), 1, 32, true);
  case MVT::mx1xnx1xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 1, true);
  case MVT::mx1xnx2xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 2, true);
  case MVT::mx1xnx4xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 4, true);
  case MVT::mx1xnx8xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 8, true);
  case MVT::mx1xnx16xf32:
    return ScalableMatrixType::get(Type::getFloatTy(Context), 1, 16, true);
  case MVT::mx1xnx1xf64:
    return ScalableMatrixType::get(Type::getDoubleTy(Context), 1, 1, true);
  case MVT::mx1xnx2xf64:
    return ScalableMatrixType::get(Type::getDoubleTy(Context), 1, 2, true);
  case MVT::mx1xnx4xf64:
    return ScalableMatrixType::get(Type::getDoubleTy(Context), 1, 4, true);
  case MVT::mx1xnx8xf64:
    return ScalableMatrixType::get(Type::getDoubleTy(Context), 1, 8, true);
#endif
  case MVT::Metadata: return Type::getMetadataTy(Context);
  }
  // clang-format on
}

/// Return the value type corresponding to the specified type.  This returns all
/// pointers as MVT::iPTR.  If HandleUnknown is true, unknown types are returned
/// as Other, otherwise they are invalid.
MVT MVT::getVT(Type *Ty, bool HandleUnknown){
  assert(Ty != nullptr && "Invalid type");
  switch (Ty->getTypeID()) {
  default:
    if (HandleUnknown) return MVT(MVT::Other);
    llvm_unreachable("Unknown type!");
  case Type::VoidTyID:
    return MVT::isVoid;
  case Type::IntegerTyID:
    return getIntegerVT(cast<IntegerType>(Ty)->getBitWidth());
#ifdef FP8_DATATYPES
  case Type::BF8TyID:
    return MVT(MVT::bf8);
  case Type::HF8TyID:
    return MVT(MVT::hf8);
#endif
  case Type::HalfTyID:      return MVT(MVT::f16);
  case Type::BFloatTyID:    return MVT(MVT::bf16);
  case Type::FloatTyID:     return MVT(MVT::f32);
  case Type::DoubleTyID:    return MVT(MVT::f64);
  case Type::X86_FP80TyID:  return MVT(MVT::f80);
  case Type::X86_MMXTyID:   return MVT(MVT::x86mmx);
  case Type::TargetExtTyID: {
    TargetExtType *TargetExtTy = cast<TargetExtType>(Ty);
    if (TargetExtTy->getName() == "aarch64.svcount")
      return MVT(MVT::aarch64svcount);
    else if (TargetExtTy->getName().starts_with("spirv."))
      return MVT(MVT::spirvbuiltin);
    if (HandleUnknown)
      return MVT(MVT::Other);
    llvm_unreachable("Unknown target ext type!");
  }
  case Type::X86_AMXTyID:   return MVT(MVT::x86amx);
  case Type::FP128TyID:     return MVT(MVT::f128);
  case Type::PPC_FP128TyID: return MVT(MVT::ppcf128);
  case Type::PointerTyID:   return MVT(MVT::iPTR);
  case Type::FixedVectorTyID:
  case Type::ScalableVectorTyID: {
    VectorType *VTy = cast<VectorType>(Ty);
    return getVectorVT(
      getVT(VTy->getElementType(), /*HandleUnknown=*/ false),
            VTy->getElementCount());
  }
#ifdef SCALABLE_MATRIX
  case Type::ScalableMatrixTyID:
    ScalableMatrixType *MTy = cast<ScalableMatrixType>(Ty);
    return getMatrixVT(getVT(MTy->getElementType(), /*HandleUnknown=*/false),
                       MTy->getNumElts(), MTy->getNumElts2(),
                       MTy->getScalable());
  }
#endif
}

/// getEVT - Return the value type corresponding to the specified type.  This
/// returns all pointers as MVT::iPTR.  If HandleUnknown is true, unknown types
/// are returned as Other, otherwise they are invalid.
EVT EVT::getEVT(Type *Ty, bool HandleUnknown){
  switch (Ty->getTypeID()) {
  default:
    return MVT::getVT(Ty, HandleUnknown);
  case Type::IntegerTyID:
    return getIntegerVT(Ty->getContext(), cast<IntegerType>(Ty)->getBitWidth());
  case Type::FixedVectorTyID:
  case Type::ScalableVectorTyID: {
    VectorType *VTy = cast<VectorType>(Ty);
    return getVectorVT(Ty->getContext(),
                       getEVT(VTy->getElementType(), /*HandleUnknown=*/ false),
                       VTy->getElementCount());
  }
#ifdef SCALABLE_MATRIX
  case Type::ScalableMatrixTyID: {
    ScalableMatrixType *VTy = cast<ScalableMatrixType>(Ty);
    return getMatrixVT(Ty->getContext(),
                       getEVT(VTy->getElementType(), /*HandleUnknown=*/false),
                       VTy->getNumElts(), VTy->getNumElts2(),
                       VTy->getScalable());
  }
#endif
  }
}

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
void MVT::dump() const {
  print(dbgs());
  dbgs() << "\n";
}
#endif

void MVT::print(raw_ostream &OS) const {
  if (SimpleTy == INVALID_SIMPLE_VALUE_TYPE)
    OS << "invalid";
  else
    OS << EVT(*this).getEVTString();
}

