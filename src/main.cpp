#include <vol/Contouring.hpp>
#include <Settings.hpp>
#include <fstream>
#include <iostream>
#include <random>


using namespace math;
using namespace std;
using namespace util;
using namespace vol;


void addCylinder(Field& field)
{
	const real rad    = field.size().x / 10.0f;
	const auto center = Vec2(field.size().xy) * 0.5f;  // in the X-Y-plane

	foreach3D(field.size(), [&](Vec3u upos) {
		auto& cell = field[upos];
		auto  vec  = Vec2(upos.xy) - center;
		auto  d    = vec.len() - rad;

		if (d < cell.dist) {
			cell.dist    = d;
			cell.normal  = Vec3(normalized(vec), 0);
		}
	});
}

void addCube(Field& field, Vec3 center, real rad) {
	const Vec3 size(rad);
	
	foreach3D(field.size(), [&](Vec3u p) {
		auto& cell = field[p];
		auto  r    = Vec3(p) - center;
		
		auto a      = r.maxAbsAxis();
		auto dist   = std::abs(r[a]) - size[a];
		auto normal = sign(r[a]) * Vec3::Axes[a];
		
		if (dist < cell.dist) {
			cell.dist    = dist;
			cell.normal  = normal;
		}
	});
}

void addSphere(Field& field, Vec3 center, real rad) {
	foreach3D(field.size(), [&](Vec3u upos) {
		auto& cell = field[upos];
		auto  vec  = Vec3(upos) - center;
		auto  d    = vec.len() - rad;
		
		if (d < cell.dist) {
			cell.dist    = d;
			cell.normal  = normalized(vec);
		}
	});
}

void removeSphere(Field& field, Vec3 center, real rad) {
	foreach3D(field.size(), [&](Vec3u upos) {
		auto& cell = field[upos];
		auto  vec  = Vec3(upos) - center;
		auto  d    = vec.len() - rad;
		
		if (-d > cell.dist) {
			cell.dist    = -d;
			cell.normal  = -normalized(vec);
		}
	});
}


// Add random jitter to emulate noisy input
void perturbField(Field& field)
{
	if (DistJitter <= 0 && NormalJitter <= 0)
		return;
	
	std::mt19937 r;
#if true
	std::normal_distribution<> distJitter  (0, DistJitter);
	std::normal_distribution<> normalJitter(0, NormalJitter);
#else
	std::uniform_real_distribution<> distJitter  (-DistJitter, +DistJitter);
	std::uniform_real_distribution<> normalJitter(-NormalJitter, +NormalJitter);
#endif
	
	foreach3D(field.size(), [&](Vec3u upos) {
		auto& cell = field[upos];
		cell.dist += distJitter(r);
		cell.normal += Vec3{ normalJitter(r), normalJitter(r), normalJitter(r) };
		cell.normal.normalize();
	});
}


// Ensure periphery has dist>0
void closeField(Field& field)
{
	const int  oa[3][2]  = {{1,2}, {0,2}, {0,1}};
	const auto fs        = field.size();
	
	for (int a=0; a<3; ++a)
	{
		auto a0 = oa[a][0];
		auto a1 = oa[a][1];
		Vec2u sideSize = { fs[a0], fs[a1] };
		
		foreach2D(sideSize, [&](Vec2u p2) {
			Vec3u p3 = Zero;
			p3[a0] = p2[0];
			p3[a1] = p2[1];
			p3[a] = 0;
			
			if (field[p3].dist <= 0) {
				field[p3].dist = 0.5f;
				field[p3].normal = -Vec3::Axes[a];
			}
			
			p3[a] = fs[a]-1;
			
			if (field[p3].dist <= 0) {
				field[p3].dist = 0.5f;
				field[p3].normal = +Vec3::Axes[a];
			}
		});
	}
}


Field generateField()
{
	const auto Size  = Vec3u( FieldSize );
	Field field(Size, Plane{+INF, Zero});

	/* Similar test case to the original Dual Contouring paper:
	 * A cylinder with an added box and a sphere subracted from that box.
	*/

	addCylinder(field);
	
	{
		const auto center = Vec3(Size) * 0.5f;
		const real rad = Size.x * 3 / 16.0f;
		addCube(field, center, rad);
	}
	
	if (SubtractSphere)
	{
		const auto center = Vec3(Size) * 0.5f + Vec3(0, 0, Size.z) * 2.0 / 16.0;
		const real rad    = Size.x * 3.5 / 16.0;
		removeSphere(field, center, rad);
	}
	else
	{
		const auto center = Vec3(Size) * 0.5f + Vec3(0, 0, Size.z) * 2.0 / 16.0;
		const real rad    = Size.x * 3.5 / 16.0;
		addSphere(field, center, rad);
	}
	
	if (PerturbField) {
		perturbField(field);
	}
	
	
	if (ClosedField) {
		// Ensure we get a closed mesh:
		closeField(field);
	}

	return field;
}


int main() {
	cout << "Generating " << FieldSize << "^3 field..." << endl;
	const auto field = generateField();

	cout << "Contouring..." << endl;
	const auto mesh = dualContouring(field);
	
	cout << mesh.vecs.size() << " vertices in " << mesh.triangles.size() << " triangles" << endl;

	auto fileName = "mesh.obj";
	cout << "Saving as " << fileName << "... " << endl;
	ofstream of(fileName);

	of << "# Vertices:" << endl;
	for (auto&& v_orig : mesh.vecs) {
		auto v = v_orig - 0.5*Vec3(field.size()); // Center
		of << "v " << v.x << " " << v.y << " " << v.z << endl;
	}

	of << endl << "# Triangles:" << endl;
	for (auto&& tri : mesh.triangles) {
		of << "f";
		for (auto&& ix : tri) {
			of << " " << (ix + 1);
		}
		of << endl;
	}
	
	cout << "Done!" << endl;
}
