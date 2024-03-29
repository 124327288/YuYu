#ifndef _GAME_AABB_H_
#define _GAME_AABB_H_

#include "Math/vec.h"
#include "Math/ray.h"

#include <float.h>
#include <algorithm>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

	// Oriented Bounding Box
class Obb
{
public:
	Obb(){ }
	v4f v1, v2, v3, v4, v5, v6, v7, v8;			
};
		// Axis-Aligned Bounding Box
class Aabb
{
public:

	Aabb()
	:
		m_min(v4f(FLT_MAX)),
		m_max(v4f(-FLT_MAX))
	{}
	Aabb( const v4f& min, const v4f& max ):m_min( min ), m_max( max ){ }

	void add( const v4f& point )
	{
		if( point.x < m_min.x ) m_min.x = point.x;
		if( point.y < m_min.y ) m_min.y = point.y;
		if( point.z < m_min.z ) m_min.z = point.z;

		if( point.x > m_max.x ) m_max.x = point.x;
		if( point.y > m_max.y ) m_max.y = point.y;
		if( point.z > m_max.z ) m_max.z = point.z;
	}

	void add( const v3f& point )
	{
		if( point.x < m_min.x ) m_min.x = point.x;
		if( point.y < m_min.y ) m_min.y = point.y;
		if( point.z < m_min.z ) m_min.z = point.z;

		if( point.x > m_max.x ) m_max.x = point.x;
		if( point.y > m_max.y ) m_max.y = point.y;
		if( point.z > m_max.z ) m_max.z = point.z;
	}

	void add( const Aabb& box )
	{
		if( box.m_min.x < m_min.x ) m_min.x = box.m_min.x;
		if( box.m_min.y < m_min.y ) m_min.y = box.m_min.y;
		if( box.m_min.z < m_min.z ) m_min.z = box.m_min.z;

		if( box.m_max.x > m_max.x ) m_max.x = box.m_max.x;
		if( box.m_max.y > m_max.y ) m_max.y = box.m_max.y;
		if( box.m_max.z > m_max.z ) m_max.z = box.m_max.z;
	}

	bool rayTest( const yyRay& r )
	{
		float t1 = (m_min.x - r.m_origin.x)*r.m_invDir.x;
		float t2 = (m_max.x - r.m_origin.x)*r.m_invDir.x;
		float t3 = (m_min.y - r.m_origin.y)*r.m_invDir.y;
		float t4 = (m_max.y - r.m_origin.y)*r.m_invDir.y;
		float t5 = (m_min.z - r.m_origin.z)*r.m_invDir.z;
		float t6 = (m_max.z - r.m_origin.z)*r.m_invDir.z;

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
		
		if (tmax < 0 || tmin > tmax) return false;

		return true;
	}

	void center( v4f& v ) const
	{
		v = v4f( m_min + m_max );
		v *= 0.5f;
	}

	f32 radius(const v4f& aabb_center )
	{
		return aabb_center.distance(m_max);
	}

	void extent( v4f& v ) { v = v4f( m_max - m_min ); }
	bool isEmpty() const  { return (m_min == m_max); }
	void reset()          { m_min = v4f(FLT_MAX); m_max = v4f(-FLT_MAX); }

	v4f m_min;
	v4f m_max;
};

#endif