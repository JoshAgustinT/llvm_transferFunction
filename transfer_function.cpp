/*
Joshua Tlatelpa-Agustin
11/10/24
Testing llvm transfer functions
written for adv compilers course
*/
#include <iostream>
#include <vector>
#include <chrono>
#include "llvm/IR/ConstantRange.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_ostream.h"

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

void print_vector_APInts_as_list(vector<APInt> list)
{
    llvm::outs() << "{";
    for (int i = 0; i < list.size(); i++)
    {
        APInt token = list[i];
        token.print(llvm::outs(), /*isSigned=*/false); // Use llvm::outs() to print

        if (i + 1 < list.size())
            llvm::outs() << ", "; // Print newline after each value
    }
    llvm::outs() << "}\n";
}

/*
takes an abstract value and returns a set of APInts that are in the concretization set for that abstract value
ie {[1,5)} returns {1,2,3,4}

can also be disjointed set so {[1,3), [9,11)}  returns {1,2,9,10}

also wraps
[2,1) returns  {0010(2), 0011(3), 0100(4), 0101(5), 0110(6), 0111(7),
    1000(8), 1001(9), 1010(10), 1011(11), 1100(12), 1101(13),
    1110 (14), 1111(15), 0000(0) }
*/
vector<APInt> concretization_function(vector<ConstantRange> abstractValues)
{
    vector<APInt> list;

    for (const auto &range : abstractValues)
    {
        APInt lower = range.getLower();
        APInt upper = range.getUpper();

        if (lower.ult(upper))
        {
            for (APInt current = lower; current.ult(upper); current = current + 1)
                list.push_back(current);

            // Case 2: Wrapped range [lower, max] and [0, upper)
        }
        else
        {
            // Iterate from `lower` to the maximum value representable by the bit width.
            APInt maxValue = APInt::getMaxValue(range.getBitWidth());
            for (APInt current = lower; current.ult(maxValue); current = current + 1)
                list.push_back(current);

            list.push_back(maxValue);

            // // Then iterate from 0 to `upper - 1`.
            for (APInt current = APInt(upper.getBitWidth(), 0); current.ult(upper); current = current + 1)
                list.push_back(current);
        }
    }

    return list;
}

/*
 it takes a set of APInts and returns the best abstract value whose concretization set includes the given set.
 ie {1,2,9,10} returns {[1,3), [9,11)}
 or  {1,2,3,4} returns {[1,5)}
*/
vector<ConstantRange> abstraction_function(vector<APInt> set)
{
    vector<ConstantRange> list;

    // Sort the set to ensure contiguous elements are next to each other.
    // std::sort(set.begin(), set.end());

    // Iterate through the sorted set to find contiguous ranges.
    APInt currentLower = set[0];
    APInt currentUpper = set[0] + 1; // Start as [lower, upper)

    for (size_t i = 1; i < set.size(); ++i)
    {
        // Check if the current element is contiguous with the previous one.
        if (set[i] == currentUpper)
        {
            // Extend the current range's upper bound.
            currentUpper = set[i] + 1;
        }
        else
        {
            // Not contiguous: save the current range and start a new one.
            list.push_back(ConstantRange(currentLower, currentUpper));
            currentLower = set[i];
            currentUpper = set[i] + 1;
        }
    }

    // Add the final range to the list.
    list.push_back(ConstantRange(currentLower, currentUpper));

    return list;
}

void dump_unsigned(ConstantRange range)
{

    std::string lowerStr, upperStr;

    // Create raw_string_ostreams to capture output from the APInt `print` method.
    llvm::raw_string_ostream lowerStream(lowerStr);
    llvm::raw_string_ostream upperStream(upperStr);

    range.getLower().print(lowerStream, false); // false indicates unsigned printing
    range.getUpper().print(upperStream, false);

    // Now print to std::cout with your desired formatting.
    std::cout << "[" << lowerStream.str() << "," << upperStream.str() << ")\n";
}

void __test_llvm_vs_naive(unsigned int bitwidth)
{

    vector<APInt> all_abstract_vals = all_abstract_values(bitwidth);
    // print_vector_APInts(all_abstract_vals);

    int count_abstract_values = all_abstract_vals.size();
    int count_llvm_more_precise = 0;
    int count_josh_more_precise = 0;
    int count_equally_precise = 0;

    for (unsigned int i = 0; i < all_abstract_vals.size(); i++)
    {
        for (unsigned int j = 0; j < all_abstract_vals.size(); j++)
        {
            // cout<< "starting loop:["<<to_string(i)<<","<<to_string(j)<<"]"<<endl;
            // cout<< "Mask: "<<i  <<" C: "<< j<<endl;
            APInt Mask = all_abstract_vals[i]; // Example Mask (only lower 2 bits matter)
            APInt C = all_abstract_vals[j];    // Example C (comparison constant)

            ConstantRange range = ConstantRange::makeMaskNotEqualRange(Mask, C);
            vector<APInt> josh_range = josh_transfer_function(Mask, C);

            vector<ConstantRange> t = {range};
            vector<APInt> llvm_range = concretization_function(t);

            // range.dump();
            // print_vector_APInts(llvm_range);
            if (llvm_range.size() > josh_range.size())
                count_josh_more_precise++;
            if (llvm_range.size() < josh_range.size())
                count_llvm_more_precise++;
            if (llvm_range.size() == josh_range.size())
                count_equally_precise++;
        }
    }

    cout << "=== Precision Comparison Summary ===" << endl;
    cout << "Bitwidth tested: " << bitwidth << endl;

    cout << "Total number of abstract values: " << count_abstract_values << endl;
    cout << "LLVM more precise cases: " << count_llvm_more_precise << endl;
    cout << "Josh's transfer function more precise cases: " << count_josh_more_precise << endl;
    cout << "Equally precise cases: " << count_equally_precise << endl;
}

int main()
{

    int max_bits = 4;
    cout << "-------------------------------------" << endl;

    for (int i = 1; i < max_bits; i++)
    {
        // Record the start time
        auto start = std::chrono::high_resolution_clock::now();

        // Call the test function
        __test_llvm_vs_naive(i);

        // Record the end time
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate duration in seconds
        std::chrono::duration<double> elapsed = end - start;

        // Output the time taken for the current iteration
        std::cout << "Finished in: " << elapsed.count() << " seconds" << std::endl;
        cout << "-------------------------------------" << endl;
    }

    // Example
    APInt Mask(4, 0b0011); // Example Mask (only lower 2 bits matter)
    APInt C(4, 0b0001);    // Example C (comparison constant)

    // Example:
    ConstantRange range = ConstantRange::makeMaskNotEqualRange(Mask, C);
    range.dump();
    vector<APInt> josh_range = josh_transfer_function(Mask, C);
    vector<ConstantRange> josh_CR = abstraction_function(josh_range);
    for (auto token : josh_CR)
        dump_unsigned(token);

    vector<ConstantRange> t = {range};
    vector<APInt> llvm_range = concretization_function(t);

    print_vector_APInts_as_list(josh_range);
    print_vector_APInts_as_list(llvm_range);
    //----------------------
    // Concretization fn tests
    // vector<ConstantRange> set1 = {ConstantRange(APInt(4, 1), APInt(4, 3)), ConstantRange(APInt(4, 9), APInt(4, 11))};
    // vector<APInt> test1 = concretization_function(set1);
    // print_vector_APInts(test1);

    // vector<ConstantRange> set1 ={ConstantRange(APInt(4, 2), APInt(4, 1))};
    // set1[0].dump();
    // vector<APInt> test1 = concretization_function(set1);
    // print_vector_APInts(test1);

    // vector<ConstantRange> set1 ={ConstantRange(APInt(4, 1), APInt(4, 5))};
    // vector<APInt> test1 = concretization_function(set1);
    // print_vector_APInts(test1);
    //----------------------------
    // Abstraction function tests
    // vector<APInt> set = {APInt(4, 1),APInt(4, 2),APInt(4, 9),APInt(4, 10)};
    // vector<ConstantRange> test = abstraction_function(set);
    // for(auto t: test)
    // dump_unsigned(t);

    // vector<APInt> set = {APInt(4, 1),APInt(4, 2),APInt(4, 3),APInt(4, 4)};
    // vector<ConstantRange> test = abstraction_function(set);
    // for(auto t: test)
    // dump_unsigned(t);

    // vector<APInt> set = {APInt(4, 1)};
    // vector<ConstantRange> test = abstraction_function(set);
    // for(auto t: test)
    // dump_unsigned(t);
    //-----------------------
    // Getting all abstract values test
    // vector<APInt> test = all_abstract_values(4);
    // print_vector_APInts(test);
    //-----------------------

    // Define a bit-width for APInt (e.g., 4 bits)
    // unsigned bitWidth = 4;

    // Initialize Mask and C as APInt values
    // APInt Mask(bitWidth, 0b0011); // Example Mask (only lower 2 bits matter)
    // APInt C(bitWidth, 0b0001);    // Example C (comparison constant)

    /*
   Initialize a range containing all values X that satisfy `(X & Mask) != C`.
   Note that the range returned may contain values where `(X & Mask) == C`
   holds, making it less precise, but still conservative.
   */
    // Create the ConstantRange using makeMaskNotEqualRange
    // ConstantRange range = ConstantRange::makeMaskNotEqualRange(Mask, C);
    // vector<APInt> josh_range = josh_transfer_function(Mask, C);
    // print_vector_APInts(josh_range);

    /*
        compare both,
        ie
    APInt Mask(bitWidth, 0b0011);
    APInt C(bitWidth, 0b0001);

    for Mask = [0-4]
    for C = [0-4]

        total number of abstract values= size_of(apints in a set)?
        total number of times the composite transfer function is more precise= when naive_transfer < llvm's?
        the total number of times the decomposed transfer function is more precise=  when naive_transfer > llvm's?
        number of times the two transfer functions return incomparable results= should be 0
    */

    /*

    Walk through of llvm's transfer function
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
