#include "../include/audio_sample.h"

namespace AllegroCPP {
	
	extern void __audio_allegro_start(int);


	Sample::Sample(Sample&& oth)
		: m_sampl(std::exchange(oth.m_sampl, nullptr)), m_fileload(std::move(oth.m_fileload))
	{
	}

	void Sample::operator=(Sample&& oth)
	{
		destroy();
		m_sampl = std::exchange(oth.m_sampl, nullptr);
		m_fileload = std::move(oth.m_fileload);
	}

	Sample::Sample(const std::string& path)
	{
		__audio_allegro_start(-16);
		destroy();
		if (!load(path)) throw std::invalid_argument("Invalid path for sample load");
	}

	bool Sample::load(const std::string& path)
	{
		destroy();
		return (m_sampl = al_load_sample(path.c_str()));
	}

	bool Sample::load(File_shareable_ptr fp)
	{
		destroy();
		if (m_sampl = al_load_sample_f(fp->get(), al_identify_sample_f(fp->get()))) m_fileload = fp;
		return m_sampl;
	}

	bool Sample::load(File_shareable_ptr fp, const std::string& ext)
	{
		destroy();
		if (m_sampl = al_load_sample_f(fp->get(), ext.c_str())) m_fileload = fp;
		return m_sampl;
	}

	void Sample::destroy()
	{
		if (m_sampl) {
			al_destroy_sample(m_sampl);
			m_sampl = nullptr;
			m_fileload.reset();
		}
	}

	void Sample::create(unsigned samples, const unsigned freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF conf)
	{
		destroy();

		size_t sample_size = al_get_channel_count(conf) * al_get_audio_depth_size(depth);
		size_t bytes = samples * sample_size;
		void* buffer = al_malloc(bytes);

		m_sampl = al_create_sample(buffer, samples, freq, depth, conf, true);
	}

	bool Sample::save(const std::string& path)
	{
		if (!m_sampl) return false;
		return al_save_sample(path.c_str(), m_sampl);
	}

	bool Sample::save(File_shareable_ptr fp)
	{
		if (!m_sampl || !fp || !fp->get()) return false;
		return al_save_sample_f(fp->get(), al_identify_sample_f(fp->get()), m_sampl);
	}

	bool Sample::save(File_shareable_ptr fp, const std::string& ext)
	{
		if (!m_sampl || !fp || !fp->get()) return false;
		return al_save_sample_f(fp->get(), ext.c_str(), m_sampl);
	}

	ALLEGRO_CHANNEL_CONF Sample::get_channels() const
	{
		if (m_sampl) return al_get_sample_channels(m_sampl);
		return static_cast<ALLEGRO_CHANNEL_CONF>(0);
	}

	ALLEGRO_AUDIO_DEPTH Sample::get_depth() const
	{
		if (m_sampl) return al_get_sample_depth(m_sampl);
		return static_cast<ALLEGRO_AUDIO_DEPTH>(0);
	}

	unsigned Sample::get_frequency() const
	{
		if (m_sampl) return al_get_sample_frequency(m_sampl);
		return 0;
	}

	unsigned Sample::get_length() const
	{
		if (m_sampl) return al_get_sample_length(m_sampl);
		return 0;
	}

	void* Sample::get_data()
	{
		if (m_sampl) return al_get_sample_data(m_sampl);
		return nullptr;
	}

}