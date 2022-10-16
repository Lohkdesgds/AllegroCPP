#include <Graphics.h>
#include <System.h>

#include <thread>
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>

#undef min
#undef max

using namespace AllegroCPP;

const std::string imgtest = "test.jpg";

void err_handler(char const* expr, char const* file, int line, char const* func)
{
	std::cout << "[FATAL ERROR ASSERT] @ '" << file << "'L'" << line << "'F'" << func << "' issued: " << expr << std::endl;
}

void trace_handler(char const* str)
{
	std::string blockname;
	std::string kind;
	std::string pathfull;
	double timeelap = 0.0;
	std::string extra;
	
	std::stringstream ss(str);
	std::string token;

	size_t _c = 0;
	while (std::getline(ss, token, ' '))
	{
		if (token.empty()) continue;

		switch (_c++) {
		case 0:
			std::cout << "__T <" << token;
			break;
		case 1:
			std::cout << ">[" << token;
			break;
		case 2:
			std::cout << "](" << token;
			break;
		case 3:
			std::cout << ")> '" << token;
			break;
		case 4:
			std::cout << "' #";
			break;
		case 5:
			while (token.size() && !std::isdigit(token.front())) token.erase(token.begin());
			while (token.size() && !std::isdigit(token.back())) token.pop_back();
			std::cout << token << "# >\t";
			break;
		default:
			std::cout << " " << token;
		}
	}
}



void _test()
{
	Event_queue qu;
	Event_custom custom;

	qu << custom;
	std::jthread thr([&] {
		Event ev = qu.wait_for_event();

		auto* ptr = ev.get_data();
		std::cout << "Shared value: " << (ptr ? std::any_cast<int>(*ptr) : -1) << std::endl;
		std::cout << "ID: " << ev.get().type << std::endl;
		std::cout << "Final: " << (*(std::string*)ev.get().user.data1) << std::endl;

		ev = qu.wait_for_event();

		ptr = ev.get_data();
		std::cout << "Shared value: " << (ptr ? std::any_cast<int>(*ptr) : -1) << std::endl;
		std::cout << "ID: " << ev.get().type << std::endl;
		std::cout << "Final: " << (*(std::string*)ev.get().user.data1) << std::endl;
	});

	custom.set_data((int)200);
	custom.emit("Hello there!", 513, [] {std::cout << "__SENT TRIGGER LOG\n"; });
	custom.emit("Second message here", 513, [] {std::cout << "__SENT TRIGGER LOG2\n"; });

	al_rest(3.0);
}

int main()
{
	Config_system confsys;

	al_register_assert_handler(&err_handler);
	al_register_trace_handler(&trace_handler);
#ifdef _DEBUG
	confsys.set("trace", "level", "warn");
#endif
	confsys.set("image", "jpeg_quality_level", "95");
	confsys.set("image", "png_compression_level ", "9");
	//_test();

	std::cout << "System values:" << std::endl;
	for (const auto& eac : Config_system{})
	{
		std::cout << "[" << eac.m_currsec << " > " << eac.m_currkey << "] = " << eac.m_currval << std::endl;
	}
	std::cout << "End of system values." << std::endl;

	const float zoomin = 2.0f;
	File log("log.txt");

	log << "Started log." << std::endl;
	log << "Creating display and stuff..." << std::endl;

	Monitor_info moninfo;
	Display* disp = new Display({ moninfo.get_width() * 0.8f, moninfo.get_height() * 0.8f }, "Funny window", ALLEGRO_OPENGL | ALLEGRO_RESIZABLE, display_undefined_position, 0, { display_option{ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST} });
	Event_queue queue;
	Bitmap bmp(imgtest);
	Font basicfont;
	Transform trans;
	//ALLEGRO_TIMER* tima = al_create_timer(1.0 / 30);
	Timer tima(1.0 / 30);
	Menu menn({
		Menu_each_menu("File", 0, {
			Menu_each_default("Show Exit button below", 1)
		})
	});

	log << "Preparing stuff..." << std::endl;

	queue << *disp;
	queue << tima;
	queue << Event_keyboard();
	queue << menn;

	menn >> *disp;
		
	trans.scale({ zoomin, zoomin });
	trans.use();

	double _last = Time::get_time();
	double _smooth_fps = 1.0;
	double _fps_cpy = 0.0;
	const double _elap_calc = al_get_time();

	bool _is_minimized = false;

	log << "Running." << std::endl;

	for(bool runn = true; runn;)
	{
		if (queue.has_event() || _is_minimized) {
			const auto ev = queue.wait_for_event();
			if (!ev) continue;

			switch (ev.get().type) {
			case ALLEGRO_EVENT_MENU_CLICK:
			{
				Menu_event mev(ev);
				switch (mev.get_id()) {
				case 1:
					mev.get_source()["File"].push(Menu_each_default("You'll have to press this one", 2));
					mev.patch_caption("This is no longer available.");
					mev.patch_toggle_flag(menu_flags::DISABLED);
					break;
				case 2:
					mev.get_source()["File"].push(Menu_each_default("Ok, click this one to exit then", 3));
					mev.patch_caption("Please don't kill me");
					mev.patch_toggle_flag(menu_flags::DISABLED);
					mev.get_source()["File"].remove(0);
					break;
				case 3:
					runn = false;
					break;
				}
			}
				break;
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				runn = false;
				continue;
			case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
				_is_minimized = true;
				tima.start();
				continue;
			case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
				tima.stop();
				_is_minimized = false;
				continue;
			case ALLEGRO_EVENT_DISPLAY_RESIZE:
				disp->acknowledge_resize();
				//trans.use(); // not needed
				continue;
			case ALLEGRO_EVENT_TIMER:
				break;
			case ALLEGRO_EVENT_KEY_DOWN:
				runn = false;
				continue;
			}
		}

		disp->clear_to_color(al_map_rgb_f(0.4f + 0.3f * sinf(al_get_time() * 0.3f + 0.5f), 0.4f + 0.3f * sinf(al_get_time() * 1.1f + 1.2f), 0.4f + 0.3f * sinf(al_get_time() * 0.7f + 2.1f)));

		bmp.draw(
			{ disp->get_width() * 0.125f, disp->get_height() * 0.125f },
			{ bitmap_scale{ disp->get_width() * 0.5f / (bmp.get_width() * zoomin), disp->get_height() * 0.5f / (bmp.get_height() * zoomin) }, al_map_rgba_f(0.7f,0.7f,0.7f,0.7f) }
		);

		{
			const double __cst = (fabs(al_get_time() - _last) + 1e-100);
			const double __qo = (pow(2.0 * (1.0 / __cst) + 1e-6, 0.57));

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

	log << "Destroying stuff..." << std::endl;

	delete disp;
	tima.stop();

	log << "Testing TCP/UDP..." << std::endl;

	std::cout << "Testing TCP with threads..." << std::endl;

	const file_protocol prt = file_protocol::UDP;

	File_host host(50000, prt);
	bool keep = true;

	Thread thrr([&] {
		log << "-- Host started." << std::endl;

		while (keep) {
			std::cout << "HOST: Listening..." << std::endl;

			auto client = host.listen(2000);
			if (!client) continue;

			std::cout << "HOST: Client connected successfully" << std::endl;

			client.set_timeout_read(200);
						
			while (client.valid()) {
				std::string buf;

				client >> buf;
				std::cout << "HOST <- CLIENT: " << buf << std::endl;

				if (buf.empty() && !client.valid()) break;

				std::cout << "HOST -> CLIENT: " << buf << std::endl;
				client << buf;

				if (!client) break;
			}

			if (!client.valid()) {
				auto err = client.get_error();
				log << "HOST log: " << err.msg << std::endl;
				//std::cout << "HOST: got message: " << err.msg << std::endl;
			}
		}
		log << "-- Host closed." << std::endl;
		return false;
	});

	std::this_thread::sleep_for(std::chrono::seconds(1));

	{
		std::cout << "CLIENT: Connecting..." << std::endl;

		File_client cli("localhost", 50000, prt);
		if (!cli) {
			std::cout << "CLIENT: Can't connect" << std::endl;
			log << "CLIENT: Failed to connect (client)." << std::endl;
			return 0;
		}

		std::cout << "CLIENT: Connected" << std::endl;

		std::string buf;

		std::cout << "CLIENT -> HOST: Hello" << std::endl;
		cli << "Hello";

		cli >> buf;

		std::cout << "CLIENT <- HOST: " << buf << std::endl;

		keep = false;
	}

	log << "Ended successfully." << std::endl;

	return 0;
}