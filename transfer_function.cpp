/*
Joshua Tlatelpa-Agustin
9/18/24
bf language compiler
written for adv compilers course
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <unordered_set>
#include <sstream>
#include <cassert>
#include <utility> // for std::pair

#include "llvm/IR/IRBuilder.h"    // For IRBuilder and instruction building
#include "llvm/IR/LLVMContext.h"  // For LLVMContext, which manages LLVM global state
#include "llvm/IR/Module.h"       // For Module, representing a full LLVM program
#include "llvm/IR/BasicBlock.h"   // For BasicBlock, representing code blocks
#include "llvm/IR/Function.h"     // For Function, representing functions in LLVM IR
#include "llvm/IR/Verifier.h"     // For verifying correctness of the generated IR
#include "llvm/IR/Instructions.h" // For specific instructions like PHINode, LoadInst, StoreInst, etc.
#include "llvm/IR/Type.h"

#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Utils.h>

#include <llvm/Passes/PassBuilder.h>



#include "llvm/IR/ConstantRange.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <string>

using namespace llvm;

int main() {
    // Define a bit-width for APInt (e.g., 4 bits)
    unsigned bitWidth = 4;

    // Initialize Mask and C as APInt values
    APInt Mask(bitWidth, 0b0011); // Example Mask (only lower 2 bits matter)
    APInt C(bitWidth, 0b0001);    // Example C (comparison constant)

    // Create the ConstantRange using makeMaskNotEqualRange
    ConstantRange range = ConstantRange::makeMaskNotEqualRange(Mask, C);

    // Convert bounds to strings using raw_string_ostream
    std::string lowerStr, upperStr;
    raw_string_ostream lowerOS(lowerStr), upperOS(upperStr);
    range.getLower().print(lowerOS, /*isSigned=*/false);
    range.getUpper().print(upperOS, /*isSigned=*/false);

    // Print the lower and upper bounds of the ConstantRange
    std::cout << "Range Lower Bound: " << lowerOS.str() << "\n";
    std::cout << "Range Upper Bound: " << upperOS.str() << "\n";

    // Check if a specific value is within the range
    APInt value(bitWidth, 2); // Example value
    bool contains = range.contains(value);

    // Use raw_string_ostream for value as well
    std::string valueStr;
    raw_string_ostream valueOS(valueStr);
    value.print(valueOS, /*isSigned=*/false);

    std::cout << "Does the range contain " << valueOS.str() << "? "
              << (contains ? "Yes" : "No") << "\n";

    return 0;
}
