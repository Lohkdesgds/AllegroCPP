#include <Graphics.h>
#include <Audio.h>
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
const std::string msktest = "music.ogg";

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
	Text_log loggin("Logging system CO.");

	loggin << "Hello there" << std::endl;
	loggin << "This is some logging\nHopefully\nthis breaks lines." << std::endl;

	al_register_assert_handler(&err_handler);
	al_register_trace_handler(&trace_handler);
#ifdef _DEBUG
	confsys.set("trace", "level", "debug"); // Can be one of debug, info, warn, error, none or empty. // https://github.com/liballeg/allegro5/blob/master/allegro5.cfg#L186
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

	log << "Testing path stuff!" << std::endl;

	Path path("C:\\Program Files\\Adobe\\");

	path.make_canonical();

	std::cout << path.c_str() << std::endl;

	log << "Creating display and stuff..." << std::endl;

	Monitor_info moninfo;
	Display disp({ moninfo.get_width() * 0.8f, moninfo.get_height() * 0.8f }, "Funny window", ALLEGRO_DIRECT3D_INTERNAL | ALLEGRO_RESIZABLE, /*display_undefined_position*/ Mouse_cursor::get_pos(), 0, { display_option{ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST} }, {true, false});
	Event_queue queue;
	File_tmp tmp("LunarisTestXXXX.jpg");
	File fpload(imgtest, "rb");
	{
		char ch{};
		while (fpload.read(&ch, 1)) tmp.write(&ch, 1);
		tmp.flush();
		tmp.seek(0, ALLEGRO_SEEK_SET);
	}

	Bitmap bmp(tmp, 1024, 0, ".jpg");
	Font basicfont;
	Transform trans;
	Timer tima(1.0 / 30);
	Menu menn({
		Menu_each_menu("File", 0, {
			Menu_each_default("Show Exit button below", 1)
		})
	});
	Voice voice;
	Mixer mixer;
	Audio_stream astream(msktest, 4, 1 << 14);
	Vertexes vertx;
	Video vid("video.ogv");

	voice << mixer;
	mixer << astream;

	astream.set_playing(false);
	astream.set_playmode(ALLEGRO_PLAYMODE_LOOP);
	astream.set_gain(0.01f);

	log << "Preparing stuff..." << std::endl;

	queue << disp;
	queue << tima;
	queue << Event_keyboard();
	queue << menn;
	queue << astream;
	queue << vid;

	menn >> disp;

	trans.scale({ zoomin, zoomin });
	trans.use();

	vertx
		.push_back({ 510, 510, 0, al_map_rgb(200,200,200) })
		.push_back({ 550, 510, 0, al_map_rgb(200,200,200) })
		.push_back({ 530, 530, 0, al_map_rgb(200,200,200) })
		.push_back({ 570, 530, 0, al_map_rgb(200,255,200) })
		.push_back({ 550, 560, 0, al_map_rgb(25,255,200) })
		.set_mode(Vertexes::types::TRIANGLE_STRIP);

	vertx.generate_transformed();

	double _last = Time::get_time();
	double _smooth_fps = 1.0;
	double _fps_cpy = 0.0;
	const double _elap_calc = al_get_time();

	bool _is_minimized = false;

	log << "Running." << std::endl;

	astream.set_playing(true);
	vid.start(mixer);
	vid.set_playing(true);

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
				disp.acknowledge_resize();
				//trans.use(); // not needed
				continue;
			case ALLEGRO_EVENT_TIMER:
				break;
			case ALLEGRO_EVENT_KEY_DOWN:
				if (ev.get().keyboard.keycode == ALLEGRO_KEY_ESCAPE) runn = false;
				continue;
			//case ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT:
			//	loggin << "Got slice:\n";
			//	//{
			//	//	const char* frag = (char*)astream.get_fragment();
			//	//}
			//	break;
			}
		}

		disp.clear_to_color(al_map_rgb_f(0.4f + 0.3f * sinf(al_get_time() * 0.3f + 0.5f), 0.4f + 0.3f * sinf(al_get_time() * 1.1f + 1.2f), 0.4f + 0.3f * sinf(al_get_time() * 0.7f + 2.1f)));


		if (auto frame = vid.get_frame(); frame) {
			const float scale = 0.3f; /* Adjust this to fit your target bitmap dimensions. */
			float sw = al_get_bitmap_width(frame);
			float sh = al_get_bitmap_height(frame);
			float dw = scale * vid.get_scaled_width();
			float dh = scale * vid.get_scaled_height();
			al_draw_scaled_bitmap(frame, 0, 0, sw, sh, 120, 120, dw, dh, 0);
		}

		bmp.draw(
			{ disp.get_width() * 0.125f, disp.get_height() * 0.125f },
			{ bitmap_scale{ disp.get_width() * 0.5f / (bmp.get_width() * zoomin), disp.get_height() * 0.5f / (bmp.get_height() * zoomin) }, al_map_rgba_f(0.7f,0.7f,0.7f,0.7f) }
		);

		{
			const double __cst = (fabs(al_get_time() - _last) + 1e-100);
			const double __qo = (pow(2.0 * (1.0 / __cst) + 1e-6, 0.57));

			_smooth_fps = ((_smooth_fps * __qo + __cst) / (__qo + 1.0));

			_fps_cpy = 1.0 / (fabs(_smooth_fps) + 1e-100);
			_last = al_get_time();
		}

		const uint64_t music_t = astream.get_played_samples() * 100ULL / static_cast<uint64_t>(astream.get_frequency());

		basicfont.draw_multiline({ 0.5f,0.5f },
			"Fancy line - AllegroCPP test\n"
			"FPS: " + std::to_string(static_cast<int>(_fps_cpy)) + "." + std::to_string(static_cast<int>(10000 * _fps_cpy) % 10000) + "\n"
			"Frametime: " + std::to_string(_smooth_fps * 1e3) + " ms\n"
			"Elapsed time: " + std::to_string(al_get_time() - _elap_calc) + " s\n"
			"Music time: " + std::to_string(music_t / 100ULL) + "." + std::to_string(music_t % 100ULL) + " s | Samples: " + std::to_string(astream.get_played_samples())
			,
			-1.0f, -1.0f, al_map_rgb(0, 255, 255)
		);

		vertx.draw();

		disp.flip();
	}

	log << "Destroying stuff..." << std::endl;

	astream.set_playing(false);

	disp.destroy();
	tima.stop();

	astream.destroy();
	mixer.destroy();
	voice.destroy();

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

//#include <allegro5/allegro.h>
//#include <allegro5/allegro_audio.h>
//#include <allegro5/allegro_video.h>
//#include <allegro5/allegro_font.h>
//#include <allegro5/allegro_primitives.h>
//
//#include <stdio.h>
//
//#define log_printf(...) printf_s(__VA_ARGS__)
//#define abort_example(...) [&]{ printf_s(__VA_ARGS__); exit(0);} ()
//
//static ALLEGRO_DISPLAY *screen;
//static ALLEGRO_FONT *font;
//static char const *filename;
//static float zoom = 0;
//
//static void video_display(ALLEGRO_VIDEO *video)
//{
//   /* Videos often do not use square pixels - these return the scaled dimensions
//    * of the video frame.
//    */
//   float scaled_w = al_get_video_scaled_width(video);
//   float scaled_h = al_get_video_scaled_height(video);
//   /* Get the currently visible frame of the video, based on clock
//    * time.
//    */
//   ALLEGRO_BITMAP *frame = al_get_video_frame(video);
//   int w, h, x, y;
//   ALLEGRO_COLOR tc = al_map_rgba_f(0, 0, 0, 0.5);
//   ALLEGRO_COLOR bc = al_map_rgba_f(0.5, 0.5, 0.5, 0.5);
//   double p;
//
//   if (!frame)
//      return;
//
//   if (zoom == 0) {
//      /* Always make the video fit into the window. */
//      h = al_get_display_height(screen);
//      w = (int)(h * scaled_w / scaled_h);
//      if (w > al_get_display_width(screen)) {
//         w = al_get_display_width(screen);
//         h = (int)(w * scaled_h / scaled_w);
//      }
//   }
//   else {
//      w = (int)scaled_w;
//      h = (int)scaled_h;
//   }
//   x = (al_get_display_width(screen) - w) / 2;
//   y = (al_get_display_height(screen) - h) / 2;
//
//   /* Display the frame. */
//   al_draw_scaled_bitmap(frame, 0, 0,
//                         al_get_bitmap_width(frame),
//                         al_get_bitmap_height(frame), x, y, w, h, 0);
//
//   /* Show some video information. */
//   al_draw_filled_rounded_rectangle(4, 4,
//      al_get_display_width(screen) - 4, 4 + 14 * 4, 8, 8, bc);
//   p = al_get_video_position(video, ALLEGRO_VIDEO_POSITION_ACTUAL);
//   al_draw_textf(font, tc, 8, 8 , 0, "%s", filename);
//   al_draw_textf(font, tc, 8, 8 + 13, 0, "%3d:%02d (V: %+5.2f A: %+5.2f)",
//      (int)(p / 60),
//      ((int)p) % 60,
//      al_get_video_position(video, ALLEGRO_VIDEO_POSITION_VIDEO_DECODE) - p,
//      al_get_video_position(video, ALLEGRO_VIDEO_POSITION_AUDIO_DECODE) - p);
//   al_draw_textf(font, tc, 8, 8 + 13 * 2, 0,
//      "video rate %.02f (%dx%d, aspect %.1f) audio rate %.0f",
//         al_get_video_fps(video),
//         al_get_bitmap_width(frame),
//         al_get_bitmap_height(frame),
//         scaled_w / scaled_h,
//         al_get_video_audio_rate(video));
//   al_draw_textf(font, tc, 8, 8 + 13 * 3, 0,
//      "playing: %s", al_is_video_playing(video) ? "true" : "false");
//   al_flip_display();
//   al_clear_to_color(al_map_rgb(0, 0, 0));
//}
//
//
//int main(int argc, char *argv[])
//{
//   ALLEGRO_EVENT_QUEUE *queue;
//   ALLEGRO_EVENT event;
//   ALLEGRO_TIMER *timer;
//   ALLEGRO_VIDEO *video;
//   bool fullscreen = false;
//   bool redraw = true;
//   bool use_frame_events = false;
//   int filename_arg_idx = 1;
//
//   if (!al_init()) {
//      abort_example("Could not init Allegro.\n");
//   }
//
//   if (argc < 2) {
//      log_printf("This example needs to be run from the command line.\n"
//                 "Usage: %s [--use-frame-events] <file>\n", argv[0]);
//      goto done;
//   }
//
//   /* If use_frame_events is false, we use a fixed FPS timer. If the video is
//    * displayed in a game this probably makes most sense. In a
//    * dedicated video player you probably want to listen to
//    * ALLEGRO_EVENT_VIDEO_FRAME_SHOW events and only redraw whenever one
//    * arrives - to reduce possible jitter and save CPU.
//    */
//   if (argc == 3 && strcmp(argv[1], "--use-frame-events") == 0) {
//      use_frame_events = true;
//      filename_arg_idx++;
//   }
//
//   if (!al_init_video_addon()) {
//      abort_example("Could not initialize the video addon.\n");
//   }
//   al_init_font_addon();
//   al_install_keyboard();
//
//   al_install_audio();
//   al_reserve_samples(1);
//   al_init_primitives_addon();
//
//   timer = al_create_timer(1.0 / 60);
//
//   al_set_new_display_flags(ALLEGRO_RESIZABLE);
//   al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
//   screen = al_create_display(320, 240);
//   if (!screen) {
//      abort_example("Could not set video mode - exiting\n");
//   }
//   
//   font = al_create_builtin_font();
//   if (!font) {
//      abort_example("No font.\n");
//   }
//
//   al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
//
//   filename = argv[filename_arg_idx];
//   video = al_open_video(filename);
//   if (!video) {
//      abort_example("Cannot read %s.\n", filename);
//   }
//   log_printf("video FPS: %f\n", al_get_video_fps(video));
//   log_printf("video audio rate: %f\n", al_get_video_audio_rate(video));
//   log_printf(
//      "keys:\n"
//      "Space: Play/Pause\n"
//      "cursor right/left: seek 10 seconds\n"
//      "cursor up/down: seek one minute\n"
//      "F: toggle fullscreen\n"
//      "1: disable scaling\n"
//      "S: scale to window\n");
//
//   queue = al_create_event_queue();
//   al_register_event_source(queue, al_get_video_event_source(video));
//   al_register_event_source(queue, al_get_display_event_source(screen));
//   al_register_event_source(queue, al_get_timer_event_source(timer));
//   al_register_event_source(queue, al_get_keyboard_event_source());
//
//   al_start_video(video, al_get_default_mixer());
//   al_start_timer(timer);
//   for (;;) {
//      double incr;
//
//      if (redraw && al_event_queue_is_empty(queue)) {
//         video_display(video);
//         redraw = false;
//      }
//
//      al_wait_for_event(queue, &event);
//      switch (event.type) {
//         case ALLEGRO_EVENT_KEY_DOWN:
//            switch (event.keyboard.keycode) {
//               case ALLEGRO_KEY_SPACE:
//                  al_set_video_playing(video, !al_is_video_playing(video));
//                  break;
//               case ALLEGRO_KEY_ESCAPE:
//                  al_close_video(video);
//                  goto done;
//                  break;
//               case ALLEGRO_KEY_LEFT:
//                  incr = -10.0;
//                  goto do_seek;
//               case ALLEGRO_KEY_RIGHT:
//                  incr = 10.0;
//                  goto do_seek;
//               case ALLEGRO_KEY_UP:
//                  incr = 60.0;
//                  goto do_seek;
//               case ALLEGRO_KEY_DOWN:
//                  incr = -60.0;
//                  goto do_seek;
//
//               do_seek:
//                  al_seek_video(video, al_get_video_position(video, ALLEGRO_VIDEO_POSITION_ACTUAL) + incr);
//                  break;
//
//               case ALLEGRO_KEY_F:
//                  fullscreen = !fullscreen;
//                  al_set_display_flag(screen, ALLEGRO_FULLSCREEN_WINDOW,
//                     fullscreen);
//                  break;
//               
//               case ALLEGRO_KEY_1:
//                  zoom = 1;
//                  break;
//
//               case ALLEGRO_KEY_S:
//                  zoom = 0;
//                  break;
//               default:
//                  break;
//            }
//            break;
//         
//         case ALLEGRO_EVENT_DISPLAY_RESIZE:
//            al_acknowledge_resize(screen);
//            al_clear_to_color(al_map_rgb(0, 0, 0));
//            break;
//
//         case ALLEGRO_EVENT_TIMER:
//            /*
//            display_time += 1.0 / 60;
//            if (display_time >= video_time) {
//               video_time = display_time + video_refresh_timer(is);
//            }*/
//
//            if (!use_frame_events) {
//               redraw = true;
//            }
//            break;
//
//         case ALLEGRO_EVENT_DISPLAY_CLOSE:
//            al_close_video(video);
//            goto done;
//            break;
//
//         case ALLEGRO_EVENT_VIDEO_FRAME_SHOW:
//            if (use_frame_events) {
//               redraw = true;
//            }
//            break;
//
//         case ALLEGRO_EVENT_VIDEO_FINISHED:
//            log_printf("video finished\n");
//            break;
//         default:
//            break;
//      }
//   }
//done:
//   al_destroy_display(screen);
//   //close_log(true);
//   return 0;
//}