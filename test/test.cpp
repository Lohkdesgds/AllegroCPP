#include <Graphics.h>

#include <thread>
#include <chrono>
#include <iostream>
#include <string>

#undef min
#undef max

using namespace AllegroCPP;

const std::string imgtest = "test.jpg";

int main()
{
	const float zoomin = 2.0f;
	Display* disp = new Display({ 1280,720 }, "Funny window", ALLEGRO_OPENGL);
	//Bitmap bmp(imgtest);
	Font basicfont;
	Transform trans;
		
	trans.scale({ zoomin, zoomin });
	trans.use();

	double _last = al_get_time();
	double _smooth_fps = 1.0;
	double _fps_cpy = 0.0;

	for (double nn = al_get_time() + 5.0; al_get_time() < nn;)
	{
		//bmp.draw({ 0,0 }, { bitmap_scale{ disp.get_width() * 1.0f / (bmp.get_width() * zoomin), disp.get_height() * 1.0f / (bmp.get_height() * zoomin) } });

		disp->clear_to_color(al_map_rgb_f(0.4f + 0.3f * sinf(al_get_time() * 0.3f + 0.5f), 0.4f + 0.3f * sinf(al_get_time() * 1.1f + 1.2f), 0.4f + 0.3f * sinf(al_get_time() * 0.7f + 2.1f)));

		_smooth_fps = ((_smooth_fps * 19.0 + (al_get_time() - _last)) / 20.0);

		if (static_cast<unsigned long long>(al_get_time() * 100.0) % 10 == 0) _fps_cpy = 1.0 / (fabs(_smooth_fps) + 1e-100);

		basicfont.draw_multiline({ 0.5f,0.5f },
			"Fancy line - AllegroCPP test\nFPS: " + std::to_string(static_cast<int>(_fps_cpy)) + "." + std::to_string(static_cast<int>(10000 * _fps_cpy) % 10000) + "\nFrametime: " + std::to_string(_smooth_fps * 1e3) + " ms\nRemaining time: " + std::to_string(nn - al_get_time()) + " s",
			-1.0f, -1.0f, al_map_rgb(0, 255, 255)
		);
		_last = al_get_time();

		//bmp.draw({ 0,0 });
		disp->flip();
	}

	delete disp;

	std::cout << "Testing TCP..." << std::endl;

	const file_protocol prt = file_protocol::UDP;

	FileHost host(50000, prt);
	bool keep = true;

	std::jthread thr([&] {
		while (keep) {
			std::cout << "HOST: WAIT" << std::endl;

			auto client = host.listen(2000);
			if (!client) continue;

			std::cout << "HOST: HAS" << std::endl;

			client.set_timeout_read(200);
						
			while (client) {
				char buf[256]{};

				auto red = client.read(buf, 256);
				std::cout << "HOST: READ: " << red << std::endl;

				if (red == 0 && client.has_error()) break;

				red = client.write(buf, red);
				std::cout << "HOST: WRITE: " << red << std::endl;

				if (red == 0) break;
			}

			if (client.has_error()) {
				auto err = client.get_error();
				std::cout << "HOST: got error: " << err.msg << std::endl;
			}
		}
	});

	std::this_thread::sleep_for(std::chrono::seconds(1));

	{
		std::cout << "Connecting..." << std::endl;

		FileClient cli("localhost", 50000, prt);
		if (!cli) {
			std::cout << "Can't connect" << std::endl;
			return 0;
		}

		std::cout << "Connected" << std::endl;

		char buf[256]{};
		cli.write("Hello", 6);
		cli.read(buf, 256);

		std::cout << "client read: " << buf << std::endl;

		keep = false;
	}

	return 0;
}