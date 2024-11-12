/*
Joshua Tlatelpa-Agustin
11/11/24
bf transfer function precision test
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
using namespace std;

/*
for a bitwidth that arrives as a function argument, enumerates all
possible valid abstract values at that bitwidth, for your chosen abstract domain.
*/
std::vector<APInt> all_abstract_values(unsigned int bitwidth)
{
    std::vector<APInt> list;
    unsigned int value = 0;

    // 2^bitwidth
    unsigned int max_value_represented_by_X = 1u << bitwidth;

    // Loop to generate all values from 0 to (2^bitwidth - 1)
    for (unsigned int i = 0; i < max_value_represented_by_X; i++)
    {
        list.push_back(APInt(bitwidth, value));
        value++;
    }

    return list;
}

vector<APInt> josh_transfer_function(APInt Mask, APInt C)
{
    unsigned bitWidth = Mask.getBitWidth();

    vector<APInt> possible_x = all_abstract_values(bitWidth);
    vector<APInt> list;

    for (auto &token : possible_x)
    {
        // Perform the bitwise AND operation and compare with C
        APInt result = token & Mask;

        // If the result of (token & Mask) is not equal to C, add to list
        if (result != C)
            list.push_back(token);
    }

    return list;
}

void print_vector_APInts(vector<APInt> list)
{
    for (auto token : list)
    {
        token.print(llvm::outs(), /*isSigned=*/false); // Use llvm::outs() to print
        llvm::outs() << "\n";                          // Print newline after each value
    }
}

int main()
{
    // vector<APInt> test = all_abstract_values(4);

    // print_vector_APInts(test);
    // Define a bit-width for APInt (e.g., 4 bits)
    unsigned bitWidth = 4;

    // Initialize Mask and C as APInt values
    APInt Mask(bitWidth, 0b0011); // Example Mask (only lower 2 bits matter)
    APInt C(bitWidth, 0b0001);    // Example C (comparison constant)

    /*
   Initialize a range containing all values X that satisfy `(X & Mask) != C`.
   Note that the range returned may contain values where `(X & Mask) == C`
   holds, making it less precise, but still conservative.
   */
    // Create the ConstantRange using makeMaskNotEqualRange
    ConstantRange range = ConstantRange::makeMaskNotEqualRange(Mask, C);
    range.dump();
    cout << endl;

    vector<APInt> josh_range = josh_transfer_function(Mask, C);

    print_vector_APInts(josh_range);

    /*
    APInt Mask(bitWidth, 0b0011); // Example Mask (only lower 2 bits matter)
    APInt C(bitWidth, 0b0001);
    ConstantRange range = ConstantRange::makeMaskNotEqualRange(Mask, C);
    range.dump();

    [2,1)
    X values that when & by 'Mask' != C
    X & 0011 != 0001
    0010(2), 0011(3), 0100(4), 0101(5), 0110(6), 0111(7),
    1000(8), 1001(9), 1010(10), 1011(11), 1100(12), 1101(13),
    1110 (14), 1111(15), 0000(0)

    in this case we are saying values who's lower bits  don't have XX01
    here we have proof that it's not very precise ie 5,9, 13
    */

    return 0;
}
