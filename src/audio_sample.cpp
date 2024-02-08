#include "../include/audio_sample.h"

#include <stdexcept>

namespace AllegroCPP {
	
	extern void __audio_allegro_start(int);

	Sample::Sample(Sample&& oth)
		: m_sampl(std::move(oth.m_sampl)), m_fileload(std::move(oth.m_fileload))
	{
	}

	void Sample::operator=(Sample&& oth)
	{
		destroy();
		m_sampl = std::move(oth.m_sampl);
		m_fileload = std::move(oth.m_fileload);
	}

	Sample::Sample(const std::string& path)
	{
		__audio_allegro_start(0);
		if (!load(path)) throw std::invalid_argument("Invalid path for sample load");
	}

	Sample::Sample(File_shareable_ptr fp)
	{
		__audio_allegro_start(0);
		if (!load(fp)) throw std::invalid_argument("Invalid file for sample load");
	}

	Sample::Sample(File_shareable_ptr fp, const std::string& ext)
	{
		__audio_allegro_start(0);
		if (!load(fp, ext)) throw std::invalid_argument("Invalid file for sample load");
	}

	Sample::Sample(unsigned samples, const unsigned freq, const ALLEGRO_AUDIO_DEPTH depth, const ALLEGRO_CHANNEL_CONF conf)
	{
		__audio_allegro_start(0);
		if (!(create(samples, freq, depth, conf))) throw std::invalid_argument("Cannot create sample with those settings!");
	}

	bool Sample::load(const std::string& path)
	{
		destroy();
		return (m_sampl = Sample_shared_sptr(al_load_sample(path.c_str()), [](ALLEGRO_SAMPLE* s) { al_destroy_sample(s); })).operator bool();
	}

	bool Sample::load(File_shareable_ptr fp)
	{
		destroy();
		m_fileload = fp;
		if ((m_sampl = Sample_shared_sptr(al_load_sample_f(fp->get(), al_identify_sample_f(fp->get())), [](ALLEGRO_SAMPLE* s) { al_destroy_sample(s); })).operator bool()) m_fileload = fp;
		return m_sampl.operator bool();
	}

	bool Sample::load(File_shareable_ptr fp, const std::string& ext)
	{
		destroy();
		m_fileload = fp;
		if ((m_sampl = Sample_shared_sptr(al_load_sample_f(fp->get(), ext.c_str()), [](ALLEGRO_SAMPLE* s) { al_destroy_sample(s); })).operator bool()) m_fileload = fp;
		return m_sampl.operator bool();
	}

	void Sample::destroy()
	{
		if (m_sampl) {
			m_sampl.reset();
			m_fileload.reset();
		}
	}

	bool Sample::create(const unsigned samples, const unsigned freq, const ALLEGRO_AUDIO_DEPTH depth, const ALLEGRO_CHANNEL_CONF conf)
	{
		destroy();

		size_t sample_size = al_get_channel_count(conf) * al_get_audio_depth_size(depth);
		size_t bytes = samples * sample_size;
		void* buffer = al_malloc(bytes);

		if (!(m_sampl = Sample_shared_sptr(al_create_sample(buffer, samples, freq, depth, conf, true), [](ALLEGRO_SAMPLE* s) { al_destroy_sample(s); })).operator bool()) {
			al_free(buffer);
			return false;
		}
		return true;
	}

	bool Sample::save(const std::string& path)
	{
		if (!m_sampl) return false;
		return al_save_sample(path.c_str(), m_sampl.get());
	}

	bool Sample::save(File_shareable_ptr fp)
	{
		if (!m_sampl || !fp || !fp->get()) return false;
		return al_save_sample_f(fp->get(), al_identify_sample_f(fp->get()), m_sampl.get());
	}

	bool Sample::save(File_shareable_ptr fp, const std::string& ext)
	{
		if (!m_sampl || !fp || !fp->get()) return false;
		return al_save_sample_f(fp->get(), ext.c_str(), m_sampl.get());
	}

	ALLEGRO_CHANNEL_CONF Sample::get_channels() const
	{
		if (m_sampl) return al_get_sample_channels(m_sampl.get());
		return static_cast<ALLEGRO_CHANNEL_CONF>(0);
	}

	ALLEGRO_AUDIO_DEPTH Sample::get_depth() const
	{
		if (m_sampl) return al_get_sample_depth(m_sampl.get());
		return static_cast<ALLEGRO_AUDIO_DEPTH>(0);
	}

	unsigned Sample::get_frequency() const
	{
		if (m_sampl) return al_get_sample_frequency(m_sampl.get());
		return 0;
	}

	unsigned Sample::get_length() const
	{
		if (m_sampl) return al_get_sample_length(m_sampl.get());
		return 0;
	}

	void* Sample::get_data()
	{
		if (m_sampl) return al_get_sample_data(m_sampl.get());
		return nullptr;
	}

	ALLEGRO_SAMPLE* Sample::get() const
	{
		return m_sampl.get();
	}

	Sample::operator ALLEGRO_SAMPLE* () const
	{
		return m_sampl.get();
	}

	Sample::operator Sample_shared_wptr() const
	{
		return m_sampl;
	}

}