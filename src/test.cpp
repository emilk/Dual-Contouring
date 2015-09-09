
// To get CATCH to complie with -pedantic:

// catch.hpp:4865:28: Private field 'm_impl' is not used
#pragma clang diagnostic ignored "-Wunused-private-field"

#define USE_EIGEN 0


#if USE_EIGEN
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-value"
#pragma clang diagnostic ignored "-Wunused-variable"
#include <Eigen/Dense>
#endif


#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <iostream>
#include <math/Solver.hpp>

using namespace math;
//using namespace std;



struct Plane {
	math::real dist;    // Signed distance to closest feature
	math::Vec3 normal;  // Unit-length normal
};


Vec3 intersectPlanes(std::vector<Plane> planes)
{
	std::vector<Vec3> A;
	std::vector<real> b;
	
	for (auto&& p : planes) {
		A.push_back(p.normal);
		b.push_back(p.dist);
	}
	
	return leastSquares(A.size(), A.data(), b.data());
}


TEST_CASE( "solve3x3/overdetermined", "Testing 3x3 linear algebra solver" )
{
	auto v = intersectPlanes(
									 {
										 {1, {1,0,0}},
										 {2, {1,0,0}},
										 
										 {2, {0,1,0}},
										 {3, {0,1,0}},
										 
										 {3, {0,0,1}},
										 {4, {0,0,1}}
									 } );
	
	
	REQUIRE(dist(v, Vec3{1.5, 2.5, 3.5})  <  1e-5);
}


TEST_CASE( "solve3x3/underdetermined", "Testing 3x3 linear algebra solver" )
{
	/*
	 When conturing we always have a weak push towards voxel center,
	 in case of an underdetermines system.
	 
	 This way we can constrain less than 3 dimension without
	 the solver blowing up.
	 */
	const real W = 0.01;  // Weight of weak push towards [2,2,2]
	
	auto v = intersectPlanes(
									 {
										 {2*W, {W,0,0}},
										 {2*W, {0,W,0}},
										 {2*W, {0,0,W}},
										 
										 {0.3*std::sqrt(2.0), normalized({1,1,0})},
									 } );
	
	
	REQUIRE(dist(v, Vec3{0.3, 0.3, 2})  <  1e-3);
}


#if USE_EIGEN

using namespace Eigen;

typedef Matrix<real, Dynamic, 3> Mat_A;

template<class Mat>
void nukeZeros(Mat& m, real eps)
{
	auto rows = m.rows();
	auto cols = m.cols();
	
	for (int c=0; c<cols; ++c) {
		for (int r=0; r<rows; ++r) {
			if (std::abs(m(r,c)) < eps) {
				m(r,c) = 0;
			}
		}
	}
}

void doSVD(const Mat_A& A, const VectorXd& b)
{
	SVD<Mat_A> svd( A );
	auto U = svd.matrixU();
	auto S = svd.singularValues();
	auto V = svd.matrixV();
	
	std::cout << "SVD: " << std::endl;
	std::cout << "U: " << std::endl << U << std::endl << std::endl;
	std::cout << "S: " << std::endl << S << std::endl << std::endl;
	std::cout << "V: " << std::endl << V << std::endl << std::endl << std::endl;
	
	MatrixXd S_d = DiagonalMatrix<decltype(S)>( S );
	
	//nukeZeros(S_d, 1e-2);
	
	MatrixXd A_R = U * S_d * V.transpose();
	nukeZeros(A_R, 1e-6);
	
	std::cout << "Reconstructed: " << std::endl << A_R << std::endl << std::endl << std::endl;
}


void doQR(const Mat_A& A, const VectorXd& b)
{
	
	auto qr = A.qr();
	
	std::cout << "QR: " << std::endl;
	std::cout << "rank: " << qr.rank() << std::endl << std::endl;
	std::cout << "Q: " << std::endl << qr.matrixQ() << std::endl << std::endl << std::endl;
	std::cout << "R: " << std::endl << qr.matrixR() << std::endl << std::endl;
}


void inspectPlanes(std::vector<Plane> planes)
{
	std::cout << "--------------------------" << std::endl;
	
	const auto N = planes.size();
	
	Mat_A A( N, 3 );
	VectorXd b( N );
	
	for (size_t r=0; r<N; ++r) {
		auto&& p = planes[r];
		//auto n = normalized(p.normal);
		auto n = p.normal;
		A(r, 0) = n[0];
		A(r, 1) = n[1];
		A(r, 2) = n[2];
		b(r) = p.dist;
	}
	
	doSVD(A, b);
	doQR(A, b);
}



TEST_CASE( "eigen", "Testing SVD" )
{
	using namespace Eigen;
	
	const real W = 0.01;
	
	inspectPlanes(
				 {
					 {1, {1,1,1}},
					 {2, {0,1,0}},
					 {2.1, {0.01,1,0}},
					 {W, {0,0,W}},
				 } );
	
	
	inspectPlanes(
				 {
					 {1, {1,0,0}},
					 {2, {1,0,0}},
					 
					 {2, {0,1,0}},
					 {3, {0,1,0}},
					 
					 {3, {0,0,1}},
					 {4, {0,0,1}},
					 
					 {0, {1,1,1}}
				 } );
}
#endif
