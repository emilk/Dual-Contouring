#ifndef CONTOURING_HPP
#define CONTOURING_HPP

#include <util/Array3D.hpp>
#include <array>


namespace vol
{	
	/* For all p so that
	 dot(p, normal) == dist
	 */
	struct Plane {
		math::real dist;    // Signed distance to closest feature
		math::Vec3 normal;  // Unit-length normal of the feature
		
		bool valid() const { return normal != math::Zero; }
	};

	typedef util::Array3D<Plane> Field;
	
	typedef std::array<unsigned, 3> Triangle;
	
	// CW <-> CCW
	inline Triangle flip(Triangle t) {
		return {{ t[0], t[2], t[1] }};
	}
	

	struct TriMesh {
		std::vector<math::Vec3>  vecs;
		std::vector<Triangle>    triangles;  // Indices into vecs
	};


	/*
	Implementation of:

		Dual Contouring on Hermite Data
		Proceedings of ACM SIGGRAPH, 2002
		Tao Ju, Frank Losasso, Scott Schaefer and Joe Warren 
		http://www.cs.wustl.edu/~taoju/research/dualContour.pdf

	Will use the same resolution as the field for the dual contouring.

	No simplification.
	*/
	TriMesh dualContouring(const Field& field);
}

#endif
