#include "../include/audio_sample.h"
#include "audio_sample_instance.h"

namespace AllegroCPP {

	extern void __audio_allegro_start(int);

	Sample_shared_sptr Sample_instance::_lock() const
	{
		return m_sampl.lock();
	}

	AllegroCPP::Sample_instance::Sample_instance(const Sample& sampl)
		: m_sampl(sampl)
	{
		const auto sp = _lock();
		if (!sp || !(m_sampl_inst = al_create_sample_instance(sp.get()))) throw std::invalid_argument("Sample was null.");
	}

	Sample_instance::Sample_instance(Sample_instance&& oth)
		: m_sampl(std::move(oth.m_sampl)), m_sampl_inst(std::exchange(oth.m_sampl_inst, nullptr))
	{
	}

	void Sample_instance::operator=(Sample_instance&& oth)
	{
		destroy();
		m_sampl = std::move(oth.m_sampl);
		m_sampl_inst = std::exchange(oth.m_sampl_inst, nullptr);
	}

	Sample_instance::~Sample_instance()
	{
		destroy();
	}

	bool Sample_instance::create(const Sample& oth)
	{
		destroy();
		m_sampl = oth;
		const auto sp = _lock();
		return (sp && (m_sampl_inst = al_create_sample_instance(sp.get())));
	}

	bool Sample_instance::create_duplicate(const Sample_instance& oth)
	{
		destroy();
		const auto sp = oth._lock();
		m_sampl = sp;
		return (sp && (m_sampl_inst = al_create_sample_instance(sp.get())));
	}

	const Sample& Sample_instance::operator<<(const Sample& oth)
	{
		if (!create(oth)) throw std::invalid_argument("Sample was null.");
		return oth;
	}

	void Sample_instance::destroy()
	{
		if (m_sampl_inst) {
			al_destroy_sample_instance(std::exchange(m_sampl_inst, nullptr));
			m_sampl.reset();	
		}
	}

	void Sample_instance::detach() const
	{
		if (m_sampl_inst) al_detach_sample_instance(m_sampl_inst);
	}

	bool Sample_instance::play()
	{
		if (m_sampl_inst) return al_play_sample_instance(m_sampl_inst);
		return false;
	}

	bool Sample_instance::stop()
	{
		if (m_sampl_inst) return al_stop_sample_instance(m_sampl_inst);
		return false;
	}

	ALLEGRO_CHANNEL_CONF Sample_instance::get_channels() const
	{
		if (m_sampl_inst) return al_get_sample_instance_channels(m_sampl_inst);
		return ALLEGRO_CHANNEL_CONF();
	}

	ALLEGRO_AUDIO_DEPTH Sample_instance::get_depth() const
	{
		if (m_sampl_inst) return al_get_sample_instance_depth(m_sampl_inst);
		return ALLEGRO_AUDIO_DEPTH();
	}

	unsigned Sample_instance::get_frequency() const
	{
		if (m_sampl_inst) return al_get_sample_instance_frequency(m_sampl_inst);
		return 0;
	}

	unsigned Sample_instance::get_length() const
	{
		if (m_sampl_inst) return al_get_sample_instance_length(m_sampl_inst);
		return 0;
	}

	unsigned Sample_instance::get_position() const
	{
		if (m_sampl_inst) return al_get_sample_instance_position(m_sampl_inst);
		return 0;
	}

	float Sample_instance::get_speed() const
	{
		if (m_sampl_inst) return al_get_sample_instance_speed(m_sampl_inst);
		return 0.0f;
	}

	float Sample_instance::get_gain() const
	{
		if (m_sampl_inst) return al_get_sample_instance_gain(m_sampl_inst);
		return 0.0f;
	}

	float Sample_instance::get_pan() const
	{
		if (m_sampl_inst) return al_get_sample_instance_pan(m_sampl_inst);
		return 0.0f;
	}

	float Sample_instance::get_time() const
	{
		if (m_sampl_inst) return al_get_sample_instance_time(m_sampl_inst);
		return 0.0f;
	}

	ALLEGRO_PLAYMODE Sample_instance::get_playmode() const
	{
		if (m_sampl_inst) return al_get_sample_instance_playmode(m_sampl_inst);
		return ALLEGRO_PLAYMODE();
	}

	bool Sample_instance::get_playing() const
	{
		if (m_sampl_inst) return al_get_sample_instance_playing(m_sampl_inst);
		return false;
	}

	bool Sample_instance::get_attached() const
	{
		if (m_sampl_inst) return al_get_sample_instance_attached(m_sampl_inst);
		return false;
	}

	bool Sample_instance::set_length(const unsigned len)
	{
		if (m_sampl_inst) return al_set_sample_instance_length(m_sampl_inst, len);
		return false;
	}

	bool Sample_instance::set_position(const unsigned pos)
	{
		if (m_sampl_inst) return al_set_sample_instance_position(m_sampl_inst, pos);
		return false;
	}

	bool Sample_instance::set_speed(const float speed)
	{
		if (m_sampl_inst) return al_set_sample_instance_speed(m_sampl_inst, speed);
		return false;
	}

	bool Sample_instance::set_gain(const float gain)
	{
		if (m_sampl_inst) return al_set_sample_instance_gain(m_sampl_inst, gain);
		return false;
	}

	bool Sample_instance::set_pan(const float pan)
	{
		if (m_sampl_inst) return al_set_sample_instance_pan(m_sampl_inst, pan);
		return false;
	}

	bool Sample_instance::set_playmode(const ALLEGRO_PLAYMODE pm)
	{
		if (m_sampl_inst) return al_set_sample_instance_playmode(m_sampl_inst, pm);
		return false;
	}

	bool Sample_instance::set_playing(const bool pl)
	{
		if (m_sampl_inst) return al_set_sample_instance_playing(m_sampl_inst, pl);
		return false;
	}

	Sample_shared_wptr Sample_instance::get_sample() const
	{
		return m_sampl;
	}

	ALLEGRO_SAMPLE_INSTANCE* Sample_instance::get() const
	{
		return m_sampl_inst;
	}

	Sample_instance::operator ALLEGRO_SAMPLE_INSTANCE* () const
	{
		return m_sampl_inst;
	}

}