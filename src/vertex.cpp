#include "vertex.h"

#include <utility>
#include <mutex>
#include <cmath>

namespace AllegroCPP {
	
	void __vertex_allegro_start()
	{
		if (!al_is_system_installed() && !al_init()) throw std::runtime_error("Can't start Allegro!");
		if (!al_is_primitives_addon_initialized() && !al_init_primitives_addon()) throw std::runtime_error("Can't start Primitives!");
		if (!al_is_image_addon_initialized() && !al_init_image_addon()) throw std::runtime_error("Can't start Image!");
	}

	vertex_point::vertex_point(const float _x, const float _y, const ALLEGRO_COLOR& _c)
	{
		this->x = _x;
		this->y = _y;
		this->z = 0;
		this->u = 0;
		this->v = 0;
		this->color = _c;
	}

	vertex_point::vertex_point(const float _x, const float _y, const float _z, const ALLEGRO_COLOR& _c)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->u = 0;
		this->v = 0;
		this->color = _c;
	}

	vertex_point::vertex_point(const float _x, const float _y, const float _z, const float _u, const float _v, const ALLEGRO_COLOR& _c)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->u = _u;
		this->v = _v;
		this->color = _c;
	}

	Vertexes::Vertexes()
	{
		__vertex_allegro_start();
	}

	Vertexes& Vertexes::push_back(const vertex_point& v)
	{
		std::unique_lock<std::shared_mutex>(safe_mtx);
		points.push_back(v);
		return *this;
	}

	Vertexes& Vertexes::push_back(vertex_point&& v)
	{
		std::unique_lock<std::shared_mutex>(safe_mtx);
		points.push_back(std::move(v));
		return *this;
	}

	Vertexes& Vertexes::push_back(std::initializer_list<vertex_point> l)
	{
		std::unique_lock<std::shared_mutex>(safe_mtx);
		points.insert(points.end(), l.begin(), l.end());
		return *this;
	}

	Vertexes& Vertexes::set_texture(const Bitmap& t)
	{
		std::unique_lock<std::shared_mutex>(safe_mtx);
		textur = t.make_ref();
		return *this;
	}

	void Vertexes::remove_texture()
	{
		std::unique_lock<std::shared_mutex>(safe_mtx);
		textur.destroy();
	}

	size_t Vertexes::size() const
	{
		std::shared_lock<std::shared_mutex>(safe_mtx);
		return points.size();
	}

	vertex_point Vertexes::index(const size_t i) const
	{
		std::shared_lock<std::shared_mutex>(safe_mtx);
		if (i >= points.size()) throw std::out_of_range("index was too high on Vertexes.");
		return points[i];
	}

	void Vertexes::safe(std::function<void(std::vector<vertex_point>&)> f)
	{
		if (!f) return;
		std::unique_lock<std::shared_mutex>(safe_mtx);
		f(points);
	}

	void Vertexes::csafe(std::function<void(const std::vector<vertex_point>&)> f) const
	{
		if (!f) return;
		std::shared_lock<std::shared_mutex>(safe_mtx);
		f(points);
	}

	void Vertexes::generate_transformed()
	{
		std::unique_lock<std::shared_mutex>(safe_mtx);
		
		size_t counter = 0;
		for (const auto& it : points)
		{
			vertex_point cpy = it;
			latest_transform.transform_coordinates(cpy.x, cpy.y);
			if (std::isnan(cpy.x) || std::isnan(cpy.y)) throw std::runtime_error("Transformation got invalid state!");
			++counter;
			if (npts.size() < counter) {
				npts.push_back(cpy);
			}
			else {
				npts[counter - 1] = cpy;
			}
		}
		while (npts.size() > points.size()) npts.pop_back();
	}

	void Vertexes::csafe_transformed(std::function<void(const std::vector<vertex_point>&)> f) const
	{
		if (!f || npts.size() == 0) return;
		std::shared_lock<std::shared_mutex>(safe_mtx);
		f(npts);
	}

	void Vertexes::free_transformed()
	{
		if (npts.size() == 0) return;
		std::unique_lock<std::shared_mutex>(safe_mtx);
		npts.clear();
	}

	bool Vertexes::has_texture() const
	{
		std::shared_lock<std::shared_mutex>(safe_mtx);
		return textur.valid();
	}

	Vertexes& Vertexes::set_mode(types t)
	{
		type = t;
		return *this;
	}

	Vertexes::types Vertexes::get_mode() const
	{
		return type;
	}

	void Vertexes::draw()
	{
		std::shared_lock<std::shared_mutex>(safe_mtx);
		if (!points.size()) return;
		latest_transform = al_get_current_transform();
		al_draw_prim(points.data(), nullptr, textur.valid() ? textur : nullptr, 0, static_cast<int>(points.size()), static_cast<int>(type));
	}

	bool Vertexes::valid() const
	{
		switch (type) {
		case types::POINT_LIST:
			return points.size() > 0;
		case types::LINE_LIST:
			return (points.size() > 1) && ((points.size() % 2) == 0);
		case types::LINE_STRIP:
			return points.size() > 1;
		case types::LINE_LOOP:
			return points.size() > 1;
		case types::TRIANGLE_LIST:
			return (points.size() > 2) && ((points.size() % 3) == 0);
		case types::TRIANGLE_STRIP:
			return points.size() > 2;
		case types::TRIANGLE_FAN:
			return points.size() > 2;
		}
		return false;
	}

	bool Vertexes::empty() const
	{
		return points.size() == 0;
	}

	Transform Vertexes::copy_transform_in_use() const
	{
		return latest_transform;
	}

	void Vertexes::translate(const float x, const float y, const float z)
	{
		std::unique_lock<std::shared_mutex>(safe_mtx);
		for (auto& it : points) {
			it.x += x;
			it.y += y;
			it.z += z;
		}
	}
}