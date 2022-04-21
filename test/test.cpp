#include <Graphics.h>

#include <thread>
#include <chrono>

#undef min
#undef max

using namespace AllegroCPP;

const std::string imgtest = "test.jpg";

int main()
{
	const float zoomin = 2.0f;
	Display disp({ 1280,720 }, "Funny window", ALLEGRO_OPENGL);
	//Bitmap bmp(imgtest);
	Font basicfont;
	Transform trans;
		
	trans.scale({ zoomin, zoomin });
	trans.use();

	double _last = al_get_time();
	double _smooth_fps = 1.0;
	double _fps_cpy = 0.0;

	for (double nn = al_get_time() + 30.0; al_get_time() < nn;)
	{
		//bmp.draw({ 0,0 }, { bitmap_scale{ disp.get_width() * 1.0f / (bmp.get_width() * zoomin), disp.get_height() * 1.0f / (bmp.get_height() * zoomin) } });

		disp.clear_to_color(al_map_rgb_f(0.4f + 0.3f * sinf(al_get_time() * 0.3f + 0.5f), 0.4f + 0.3f * sinf(al_get_time() * 1.1f + 1.2f), 0.4f + 0.3f * sinf(al_get_time() * 0.7f + 2.1f)));

		_smooth_fps = ((_smooth_fps * 19.0 + (al_get_time() - _last)) / 20.0);

		if (static_cast<unsigned long long>(al_get_time() * 100.0) % 10 == 0) _fps_cpy = 1.0 / (fabs(_smooth_fps) + 1e-100);

		basicfont.draw_multiline({ 0.5f,0.5f },
			"Fancy line - AllegroCPP test\nFPS: " + std::to_string(static_cast<int>(_fps_cpy)) + "." + std::to_string(static_cast<int>(10000 * _fps_cpy) % 10000) + "\nFrametime: " + std::to_string(_smooth_fps * 1e3) + " ms\nRemaining time: " + std::to_string(nn - al_get_time()) + " s",
			-1.0f, -1.0f, al_map_rgb(0, 255, 255)
		);
		_last = al_get_time();

		//bmp.draw({ 0,0 });
		disp.flip();
	}

	return 0;
}