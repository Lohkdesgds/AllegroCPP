#pragma once

#include <allegro5/allegro.h>

#include <stdexcept>

namespace AllegroCPP {

	struct transform_position3d {
		float x, y, z;
		transform_position3d(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z) {}
	};

	class Transform {
		ALLEGRO_TRANSFORM m_t{};
	public:
		Transform();
		Transform(const ALLEGRO_TRANSFORM*);
		Transform(const Transform&);
		Transform(Transform&&) = delete;
		Transform& operator=(const ALLEGRO_TRANSFORM*);
		Transform& operator=(const Transform&);
		void operator=(Transform&&) = delete;

		void use() const;
		void use_projection() const;

		void invert();
		void transpose();
		int check_inverse(const float tolerance = 1e-7f);
		void identity();
		void build(const float pos_x, const float pos_y, const float scale_x = 1.0f, const float scale_y = 1.0f, float theta = 0);
		void build_camera(transform_position3d pos, transform_position3d look, transform_position3d up = { 0.0f, 1.0f, 0.0f });
		void translate(const float post_x, const float post_y);
		void rotate(const float thetat);
		void scale(const float scale);
		void scale(const float scale_x, const float scale_y);
		void transform_coordinates(float& x, float& y);
		void transform_coordinates(float& x, float& y, float& z);
		void transform_coordinates(float& x, float& y, float& z, float& w);
		void transform_coordinates_projective(float& x, float& y, float& z);
		void compose(const Transform&);
		void orthographic_transform(const float left, const float top, const float n, const float right, const float bottom, const float f);
		void perspective_transform(const float left, const float top, const float n, const float right, const float bottom, const float f);
		void translate(const transform_position3d post);
		void scale(const transform_position3d scalet);
		void rotate(const transform_position3d rotpos, const float thetat);
		void horizontal_shear(const float theta);
		void vertical_shear(const float theta);
	};

}