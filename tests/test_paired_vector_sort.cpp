#define BOOST_TEST_MODULE PairedVectorSortTest
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include "../libs/utilities.cpp"

// auxilary functions
bool vectorsEqual(const std::vector<double>& v1, const std::vector<double>& v2, double tolerance = 1e-9) {
    if (v1.size() != v2.size()) return false;
    for (size_t i = 0; i < v1.size(); ++i) {
        if (std::abs(v1[i] - v2[i]) > tolerance) return false;
    }
    return true;
}

bool isSorted(const std::vector<double>& A) {
    return std::is_sorted(A.begin(), A.end());
}

BOOST_AUTO_TEST_SUITE(SortPairedVectorsTests)

BOOST_AUTO_TEST_CASE(BasicSortTest) {
    std::vector<double> A = {3.5, 1.2, 4.8, 2.1, 5.0};
    std::vector<double> B = {10.0, 20.0, 30.0, 40.0, 50.0};
    
    std::vector<double> expectedA = {1.2, 2.1, 3.5, 4.8, 5.0};
    std::vector<double> expectedB = {20.0, 40.0, 10.0, 30.0, 50.0};
    
    Utils::sortPairedVectors(A, B);
    
    BOOST_CHECK(vectorsEqual(A, expectedA));
    BOOST_CHECK(vectorsEqual(B, expectedB));
    BOOST_CHECK(isSorted(A));
}

BOOST_AUTO_TEST_CASE(EmptyVectorsTest) {
    std::vector<double> A, B;
    
    BOOST_CHECK_NO_THROW(Utils::sortPairedVectors(A, B));
    BOOST_CHECK(A.empty());
    BOOST_CHECK(B.empty());
}

BOOST_AUTO_TEST_CASE(SingleElementTest) {
    std::vector<double> A = {42.0};
    std::vector<double> B = {100.0};
    
    Utils::sortPairedVectors(A, B);
    
    BOOST_CHECK_CLOSE(A[0], 42.0, 1e-9);
    BOOST_CHECK_CLOSE(B[0], 100.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(AlreadySortedTest) {
    std::vector<double> A = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> B = {10.0, 20.0, 30.0, 40.0, 50.0};
    
    std::vector<double> expectedA = A;
    std::vector<double> expectedB = B;
    
    Utils::sortPairedVectors(A, B);
    
    BOOST_CHECK(vectorsEqual(A, expectedA));
    BOOST_CHECK(vectorsEqual(B, expectedB));
}

BOOST_AUTO_TEST_CASE(ReverseSortedTest) {
    std::vector<double> A = {5.0, 4.0, 3.0, 2.0, 1.0};
    std::vector<double> B = {50.0, 40.0, 30.0, 20.0, 10.0};
    
    std::vector<double> expectedA = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> expectedB = {10.0, 20.0, 30.0, 40.0, 50.0};
    
    Utils::sortPairedVectors(A, B);
    
    BOOST_CHECK(vectorsEqual(A, expectedA));
    BOOST_CHECK(vectorsEqual(B, expectedB));
}

BOOST_AUTO_TEST_CASE(DuplicateValuesTest) {
    std::vector<double> A = {3.0, 1.0, 3.0, 2.0, 1.0};
    std::vector<double> B = {30.0, 10.0, 35.0, 20.0, 15.0};
    
    Utils::sortPairedVectors(A, B);
    
    BOOST_CHECK(isSorted(A));
    BOOST_CHECK_EQUAL(A.size(), B.size());
    
    BOOST_CHECK_CLOSE(A[0], 1.0, 1e-9);
    BOOST_CHECK_CLOSE(A[1], 1.0, 1e-9);
    BOOST_CHECK_CLOSE(A[2], 2.0, 1e-9);
    BOOST_CHECK_CLOSE(A[3], 3.0, 1e-9);
    BOOST_CHECK_CLOSE(A[4], 3.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(NegativeValuesTest) {
    std::vector<double> A = {-1.5, 2.3, -3.7, 0.0, 1.2};
    std::vector<double> B = {10.0, 20.0, 30.0, 40.0, 50.0};
    
    Utils::sortPairedVectors(A, B);
    
    BOOST_CHECK(isSorted(A));
    BOOST_CHECK_CLOSE(A[0], -3.7, 1e-9);
    BOOST_CHECK_CLOSE(B[0], 30.0, 1e-9);
    BOOST_CHECK_CLOSE(A[4], 2.3, 1e-9);
    BOOST_CHECK_CLOSE(B[4], 20.0, 1e-9);
}

BOOST_AUTO_TEST_CASE(LargeDataTest) {
    const size_t size = 100;
    std::vector<double> A(size), B(size);
    
    // create large vectors for testing
    for (size_t i = 0; i < size; ++i) {
        A[i] = static_cast<double>(size - i);  // vector to be sorted, in descending order
        B[i] = static_cast<double>(i * 2);     // the paired vector
    }
    
    Utils::sortPairedVectors(A, B);
    
    BOOST_CHECK(isSorted(A));
    BOOST_CHECK_EQUAL(A.size(), size);
    BOOST_CHECK_EQUAL(B.size(), size);
    
    // check first and final elements
    BOOST_CHECK_CLOSE(A[0], 1.0, 1e-9);
    BOOST_CHECK_CLOSE(B[0], (size - 1) * 2, 1e-9);
    BOOST_CHECK_CLOSE(A[size-1], static_cast<double>(size), 1e-9);
    BOOST_CHECK_CLOSE(B[size-1], 0.0, 1e-9);
}

BOOST_AUTO_TEST_SUITE_END()