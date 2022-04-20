#pragma once

#include <allegro5/allegro.h>

#include <stdexcept>

namespace AllegroCPP {

	struct transform_position3d {
		float x, y, z;
		transform_position3d(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
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
		int check_inverse(float tolerance = 1e-7f);
		void identity();
		void build(std::pair<float, float> pos, std::pair<float, float> scale = { 1.0f,1.0f }, float theta = 0);
		void build_camera(transform_position3d pos, transform_position3d look, transform_position3d up = { 0.0f, 1.0f, 0.0f });
		void translate(std::pair<float, float> post);
		void rotate(float thetat);
		void scale(std::pair<float, float> scalet);
		void transform_coordinates(float& x, float& y);
		void transform_coordinates(float& x, float& y, float& z);
		void transform_coordinates(float& x, float& y, float& z, float& w);
		void transform_coordinates_projective(float& x, float& y, float& z);
		void compose(const Transform&);
		void orthographic_transform(float left, float top, float n, float right, float bottom, float f);
		void perspective_transform(float left, float top, float n, float right, float bottom, float f);
		void translate(transform_position3d post);
		void scale(transform_position3d scalet);
		void rotate(transform_position3d rotpos, float thetat);
		void horizontal_shear(float theta);
		void vertical_shear(float theta);
	};

}