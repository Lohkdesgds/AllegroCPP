#include <Graphics.h>

#include <thread>
#include <chrono>

using namespace AllegroCPP;

const std::string imgtest = "test.jpg";

int main()
{
	Display disp({ 1600,900 }, "Funny window", ALLEGRO_OPENGL);
	Bitmap bmp(imgtest);
	Transform trans;
		
	trans.scale({ disp.get_width() * 1.0f / bmp.get_width(), disp.get_height() * 1.0f / bmp.get_height() });
	trans.use();

	bmp.draw({ 0,0 });
	disp.flip();

	for (double nn = al_get_time(); al_get_time() - nn < 10.0;)
	{
		bmp.draw({ 0,0 });
		disp.flip();
	}

	return 0;
}