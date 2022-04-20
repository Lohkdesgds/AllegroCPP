#include "../include/transform.h"

namespace AllegroCPP {

	Transform::Transform()
	{
		if (!al_is_system_installed()) al_init();
		al_identity_transform(&m_t);
	}

	Transform::Transform(const ALLEGRO_TRANSFORM* _t)
	{
		if (!_t) throw std::invalid_argument("Pointer was null");
		al_copy_transform(&m_t, _t);
	}

	Transform::Transform(const Transform& oth)
	{
		al_copy_transform(&m_t, &oth.m_t);
	}

	Transform& Transform::operator=(const ALLEGRO_TRANSFORM* _t)
	{
		if (!_t) throw std::invalid_argument("Pointer was null");
		al_copy_transform(&m_t, _t);
		return *this;
	}

	Transform& Transform::operator=(const Transform& oth)
	{
		al_copy_transform(&m_t, &oth.m_t);
		return *this;
	}

	void Transform::use() const
	{
		al_use_transform(&m_t);
	}

	void Transform::use_projection() const
	{
		al_use_projection_transform(&m_t);
	}

	void Transform::invert()
	{
		al_invert_transform(&m_t);
	}

	void Transform::transpose()
	{
		al_transpose_transform(&m_t);
	}

	int Transform::check_inverse(float tolerance)
	{
		return al_check_inverse(&m_t, tolerance);
	}

	void Transform::identity()
	{
		al_identity_transform(&m_t);
	}

	void Transform::build(std::pair<float, float> pos, std::pair<float, float> scale, float theta)
	{
		al_build_transform(&m_t, pos.first, pos.second, scale.first, scale.second, theta);
	}

	void Transform::build_camera(transform_position3d pos, transform_position3d look, transform_position3d up)
	{
		al_build_camera_transform(&m_t, pos.x, pos.y, pos.z, look.x, look.y, look.z, up.x, up.y, up.z);
	}

	void Transform::translate(std::pair<float, float> post)
	{
		al_translate_transform(&m_t, post.first, post.second);
	}

	void Transform::rotate(float thetat)
	{
		al_rotate_transform(&m_t, thetat);
	}

	void Transform::scale(std::pair<float, float> scalet)
	{
		al_scale_transform(&m_t, scalet.first, scalet.second);
	}

	void Transform::transform_coordinates(float& x, float& y)
	{
		al_transform_coordinates(&m_t, &x, &y);
	}

	void Transform::transform_coordinates(float& x, float& y, float& z)
	{
		al_transform_coordinates_3d(&m_t, &x, &y, &z);
	}

	void Transform::transform_coordinates(float& x, float& y, float& z, float& w)
	{
		al_transform_coordinates_4d(&m_t, &x, &y, &z, &w);
	}

	void Transform::transform_coordinates_projective(float& x, float& y, float& z)
	{
		al_transform_coordinates_3d_projective(&m_t, &x, &y, &z);
	}

	void Transform::compose(const Transform& oth)
	{
		al_compose_transform(&m_t, &oth.m_t);
	}

	void Transform::orthographic_transform(float left, float top, float n, float right, float bottom, float f)
	{
		al_orthographic_transform(&m_t, left, top, n, right, bottom, f);
	}

	void Transform::perspective_transform(float left, float top, float n, float right, float bottom, float f)
	{
		al_perspective_transform(&m_t, left, top, n, right, bottom, f);
	}

	void Transform::translate(transform_position3d post)
	{
		al_translate_transform_3d(&m_t, post.x, post.y, post.z);
	}

	void Transform::scale(transform_position3d scalet)
	{
		al_scale_transform_3d(&m_t, scalet.x, scalet.y, scalet.z);
	}

	void Transform::rotate(transform_position3d rotpos, float thetat)
	{
		al_rotate_transform_3d(&m_t, rotpos.x, rotpos.y, rotpos.z, thetat);
	}

	void Transform::horizontal_shear(float theta)
	{
		al_horizontal_shear_transform(&m_t, theta);
	}

	void Transform::vertical_shear(float theta)
	{
		al_vertical_shear_transform(&m_t, theta);
	}

}