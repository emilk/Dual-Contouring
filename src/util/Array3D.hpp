#ifndef UTIL_ARRAY_3D_HPP
#define UTIL_ARRAY_3D_HPP

#include <memory>
#include <math/Vec3.hpp>
#include <algorithm> // fill_n


namespace util
{
	template<typename T>
	class Array3D
	{
	public:
		typedef math::Vec3u pos_t;
		typedef math::Vec3u size_t;


		Array3D() {}

		Array3D(size_t size) :
			m_size(size),
			m_array(new T[m_size.volume()])
		{

		}

		Array3D(size_t size, T fill) :
			m_size(size),
			m_array(new T[m_size.volume()])
		{
			std::fill_n(m_array.get(), m_size.volume(), fill);
		}

		Array3D(Array3D&& other) :
			m_size(other.m_size),
			m_array(std::move(other.m_array))
		{
			other.m_size = math::Zero;
		}


		size_t size() const { return m_size; }


		T& operator[](const pos_t& pos) {
			assert(pos.x < m_size.x  &&  pos.y < m_size.y  &&  pos.z < m_size.z);
			/*
			return m_array[  pos.z * m_size.x * m_size.y
								+ pos.y * m_size.x
								+ poz.x];
			 */
			return m_array[((pos.z * m_size.y) + pos.y) * m_size.x + pos.x];
		}

		const T& operator[](const pos_t& pos) const {
			assert(pos.x < m_size.x  &&  pos.y < m_size.y  &&  pos.z < m_size.z);
			return m_array[((pos.z * m_size.y) + pos.y) * m_size.x + pos.x];
		}


	private:
		size_t m_size = math::Zero;
		std::unique_ptr<T[]> m_array;
	};
	
	
	/*
	 Vec2u size = {2,3};
	 foreach2D(size, [](Vec2u pos) {
	 ...
	 });
	 
	 */
	template<typename Fun>
	void foreach2D(math::Vec2u min,
						math::Vec2u max,
						const Fun& fun)
	{
		for (unsigned y=min.y; y<max.y; ++y)
			for (unsigned x=min.x; x<max.x; ++x)
				fun(math::Vec2u(x,y));
	}
	
	template<typename Fun>
	void foreach2D(math::Vec2u max, const Fun& fun) {
		foreach2D({0,0}, max, fun);
	}
	
	
	/*
	Vec3u size = {2,3,4};
	foreach3D(size, [](Vec3u pos) {
		...
	});

	 */
	template<typename Fun>
	void foreach3D(math::Vec3u min,
						math::Vec3u max,
						const Fun& fun)
	{
		for (unsigned z=min.z; z<max.z; ++z)
			for (unsigned y=min.y; y<max.y; ++y)
				for (unsigned x=min.x; x<max.x; ++x)
					fun(math::Vec3u(x,y,z));
	}
	
	template<typename Fun>
	void foreach3D(math::Vec3u max, const Fun& fun) {
		foreach3D({0,0,0}, max, fun);
	}
}

#endif
