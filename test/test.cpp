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
	Monitor_info moninfo;
	Display* disp = new Display({ moninfo.get_width() * 0.8f, moninfo.get_height() * 0.8f }, "Funny window", ALLEGRO_OPENGL | ALLEGRO_RESIZABLE);
	Event_queue queue;
	Bitmap bmp(imgtest);
	Font basicfont;
	Transform trans;
	ALLEGRO_TIMER* tima = al_create_timer(1.0 / 30);

	queue << *disp;
	queue << al_get_timer_event_source(tima);
		
	trans.scale({ zoomin, zoomin });
	trans.use();

	double _last = al_get_time();
	double _smooth_fps = 1.0;
	double _fps_cpy = 0.0;
	const double _elap_calc = al_get_time();

	bool _is_minimized = false;

	for(bool runn = true; runn;)
	{
		if (queue.has_event() || _is_minimized) {
			const auto ev = queue.wait_for_event();
			if (!ev) continue;

			switch (ev.ev.type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				runn = false;
				continue;
			case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
				_is_minimized = true;
				al_start_timer(tima);
				continue;
			case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
				al_stop_timer(tima);
				_is_minimized = false;
				continue;
			case ALLEGRO_EVENT_DISPLAY_RESIZE:
				disp->acknowledge_resize();
				//trans.use(); // not needed
				continue;
			case ALLEGRO_EVENT_TIMER:
				break;
			}
		}

		disp->clear_to_color(al_map_rgb_f(0.4f + 0.3f * sinf(al_get_time() * 0.3f + 0.5f), 0.4f + 0.3f * sinf(al_get_time() * 1.1f + 1.2f), 0.4f + 0.3f * sinf(al_get_time() * 0.7f + 2.1f)));

		bmp.draw({ disp->get_width() * 0.125f, disp->get_height() * 0.125f }, { bitmap_scale{ disp->get_width() * 0.5f / (bmp.get_width() * zoomin), disp->get_height() * 0.5f / (bmp.get_height() * zoomin) }, al_map_rgba_f(0.7f,0.7f,0.7f,0.7f) });


		{
			const double __cst = (fabs(al_get_time() - _last) + 1e-100);
			const double __qo = (pow(2.0 * (1.0 / __cst) + 1e-6, 0.9));

			_smooth_fps = ((_smooth_fps * __qo + __cst) / (__qo + 1.0));

			_fps_cpy = 1.0 / (fabs(_smooth_fps) + 1e-100);
			_last = al_get_time();
		}

		basicfont.draw_multiline({ 0.5f,0.5f },
			"Fancy line - AllegroCPP test\n"
			"FPS: " + std::to_string(static_cast<int>(_fps_cpy)) + "." + std::to_string(static_cast<int>(10000 * _fps_cpy) % 10000) + "\n"
			"Frametime: " + std::to_string(_smooth_fps * 1e3) + " ms\n"
			//"Remaining time: " + std::to_string(nn - al_get_time()) + " s",
			"Elapsed time: " + std::to_string(al_get_time() - _elap_calc) + " s",
			-1.0f, -1.0f, al_map_rgb(0, 255, 255)
		);

		//bmp.draw({ 0,0 });
		disp->flip();
	}

	delete disp;
	al_destroy_timer(tima);

	std::cout << "Testing TCP..." << std::endl;

	const file_protocol prt = file_protocol::UDP;

	File_host host(50000, prt);
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

		File_client cli("localhost", 50000, prt);
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