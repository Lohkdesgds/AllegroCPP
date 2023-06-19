#include "../include/gif.h"

namespace AllegroCPP {
    
    int GIF::read_code(ALLEGRO_FILE* const& file, char* buf, int* bit_pos, int bit_size)
    {
        int i;
        int code = 0;
        int pos = 1;

        for (i = 0; i < bit_size; i++)
        {
            int byte_pos = (*bit_pos >> 3) & 255;

            if (byte_pos == 0)
            {
                int data_len = al_fgetc(file);

                if (data_len == 0)
                {
                    //printf ("Fatal. Errorneous GIF stream.\n");
                    //abort ();
                    return -1;
                }
                al_fread(file, buf + 256 - data_len, data_len);
                byte_pos = 256 - data_len;
                *bit_pos = byte_pos << 3;
            }
            if (buf[byte_pos] & (1 << (*bit_pos & 7)))
                code += pos;
            pos += pos;
            (*bit_pos)++;
        }
        return code;
    }

    int GIF::LZW_decode(ALLEGRO_FILE* const& file, ALGIF_BITMAP* bmp)
    {
        struct __tmp {
            int prefix;
            int c;
            int len;
        };
        int orig_bit_size{};
        char buf[256]{};
        int bit_size{};
        int bit_pos{};
        int clear_marker{};
        int end_marker{};
        //codes[4096]{};
        auto codes = std::make_unique<__tmp[]>(4096); /* Maximum bit size is 12. */
        int n{};
        int i{}, prev{}, code{}, c{};
        int out_pos = 0;

        orig_bit_size = al_fgetc(file);
        n = 2 + (1 << orig_bit_size);

        for (i = 0; i < n; i++)
        {
            codes[i].c = i;
            codes[i].len = 0;
        }

        clear_marker = n - 2;
        end_marker = n - 1;

        bit_size = orig_bit_size + 1;

        bit_pos = 0;

        /* Expect to read clear code as first code here. */
        prev = read_code(file, buf, &bit_pos, bit_size);
        //printf("%d %d = %d\n", bit_pos, bit_size, prev);
        if (prev == -1)
            return -1;
        do
        {
            code = read_code(file, buf, &bit_pos, bit_size);
            //printf("%d %d = %d\n", bit_pos, bit_size, code);
            if (code == -1)
                return -1;
            if (code == clear_marker)
            {
                bit_size = orig_bit_size;
                n = 1 << bit_size;
                n += 2;
                bit_size++;
                prev = code;
                continue;
            }

            if (code == end_marker)
                break;

            /* Known code: ok. Else: must be doubled char. */
            if (code < n)
                c = code;
            else
                c = prev;

            /* Output the code. */
            out_pos += codes[c].len;

            i = 0;
            do
            {
                bmp->data[out_pos - i] = codes[c].c;
                if (codes[c].len)
                    c = codes[c].prefix;
                else
                    break;
                i++;
            } while (1);

            out_pos++;

            /* Unknown code -> must be double char. */
            if (code >= n)
            {
                bmp->data[out_pos] = codes[c].c;
                out_pos++;
            }

            /* Except after clear marker, build new code. */
            if (prev != clear_marker)
            {
                codes[n].prefix = prev;
                codes[n].len = codes[prev].len + 1;
                codes[n].c = codes[c].c;
                n++;
            }

            /* Out of bits? Increase. */
            if (n == (1 << bit_size))
            {
                if (bit_size < 12)
                    bit_size++;
            }

            prev = code;
        } while (1);
        return 0;
    }

    void GIF::read_palette(ALLEGRO_FILE* const& file, ALGIF_PALETTE* const& palette) {
        for (int i = 0; i < palette->colors_count; i++)
        {
            palette->colors[i].r = al_fgetc(file);
            palette->colors[i].g = al_fgetc(file);
            palette->colors[i].b = al_fgetc(file);
        }
    }

    void GIF::deinterlace(ALGIF_BITMAP* const& bmp)
    {
        ALGIF_BITMAP *n = algif_create_bitmap(bmp->w, bmp->h);
        int i = 0;
        for (int y = 0; y < n->h; y += 8)
        {
            algif_blit(bmp, n, 0, i++, 0, y, n->w, 1);
        }
        for (int y = 4; y < n->h; y += 8)
        {
            algif_blit(bmp, n, 0, i++, 0, y, n->w, 1);
        }
        for (int y = 2; y < n->h; y += 4)
        {
            algif_blit(bmp, n, 0, i++, 0, y, n->w, 1);
        }
        for (int y = 1; y < n->h; y += 2)
        {
            algif_blit(bmp, n, 0, i++, 0, y, n->w, 1);
        }
        algif_blit(n, bmp, 0, 0, 0, 0, n->w, n->h);
        algif_destroy_bitmap(n);
    }

    bool GIF::algif_load_raw(ALLEGRO_FILE*& file, const bool del_fp) {
        if (!file)
            return false;

        al_fseek(file, 0, ALLEGRO_SEEK_SET);

        int version;
        ALGIF_BITMAP *bmp = nullptr;
        int i, j;
        
        algif_destroy_animation();
        m_anim = (ALGIF_ANIMATION*)calloc(1, sizeof(ALGIF_ANIMATION));
        
        ALGIF_FRAME frame;

        const auto freeup = [&, del_fp] {
            if (file && del_fp) {
                al_fclose(file);
                file = nullptr;
            }
            if (m_anim) {
                algif_destroy_animation();
            }
            if (bmp) {
                algif_destroy_bitmap(bmp);
                bmp = nullptr;
            }
        };

        m_anim->frames_count = 0;

        /* is it really a GIF? */
        if (al_fgetc(file) != 'G') { freeup(); return false; }
        if (al_fgetc(file) != 'I') { freeup(); return false; }
        if (al_fgetc(file) != 'F') { freeup(); return false; }
        if (al_fgetc(file) != '8') { freeup(); return false; }

        /* '7' or '9', for 87a or 89a. */
        version = al_fgetc(file);
        if (version != '7' && version != '9') { freeup(); return false; }
        if (al_fgetc(file) != 'a') { freeup(); return false; }

        m_anim->width = al_fread16le(file);
        m_anim->height = al_fread16le(file);
        i = al_fgetc(file);
        /* Global color table? */
        if (i & 128)
            m_anim->palette.colors_count = 1 << ((i & 7) + 1);
        else
            m_anim->palette.colors_count = 0;
        /* Background color is only valid with a global palette. */
        m_anim->background_index = al_fgetc(file);

        /* Skip aspect ratio. */
        al_fseek(file, 1, ALLEGRO_SEEK_CUR);

        if (m_anim->palette.colors_count)
        {
            read_palette(file, &m_anim->palette);
        }

        memset(&frame, 0, sizeof(frame)); /* For first frame. */
        frame.transparent_index = -1;

        do
        {
            i = al_fgetc(file);

            switch (i)
            {
            case 0x2c:         /* Image Descriptor */
            {
                int w, h;
                int interlaced = 0;

                frame.xoff = al_fread16le(file);
                frame.yoff = al_fread16le(file);
                w = al_fread16le(file);
                h = al_fread16le(file);
                bmp = algif_create_bitmap(w, h);
                if (!bmp) { freeup(); return false; }
                i = al_fgetc(file);

                /* Local palette. */
                if (i & 128)
                {
                    frame.palette.colors_count = 1 << ((i & 7) + 1);
                    read_palette(file, &frame.palette);
                }
                else
                {
                    frame.palette.colors_count = 0;
                }

                if (i & 64)
                    interlaced = 1;

                if (LZW_decode(file, bmp)) { freeup(); return false; }

                if (interlaced)
                    deinterlace(bmp);

                frame.bitmap_8_bit = bmp;
                bmp = nullptr;

                m_anim->frames_count++;
                auto nframes = (ALGIF_FRAME*)realloc((void*)m_anim->frames, m_anim->frames_count * sizeof * m_anim->frames);
                if (!nframes) {
                    freeup();
                    throw std::bad_alloc();
                }
                else m_anim->frames = nframes;
                m_anim->frames[m_anim->frames_count - 1] = frame;

                memset(&frame, 0, sizeof frame); /* For next frame. */
                frame.transparent_index = -1;

                break;
            }
            case 0x21: /* Extension Introducer. */
                j = al_fgetc(file); /* Extension Type. */
                i = al_fgetc(file); /* Size. */
                if (j == 0xf9) /* Graphic Control Extension. */
                {
                    /* size must be 4 */
                    if (i != 4) { freeup(); return false; }

                    i = al_fgetc(file);
                    frame.disposal_method = (i >> 2) & 7;
                    frame.duration = al_fread16le(file);
                    if (i & 1)  /* Transparency? */
                    {
                        frame.transparent_index = al_fgetc(file);
                    }
                    else
                    {
                        al_fseek(file, 1, ALLEGRO_SEEK_CUR);
                        frame.transparent_index = -1;
                    }
                    i = al_fgetc(file); /* Size. */
                }
                /* Application Extension. */
                else if (j == 0xff)
                {
                    if (i == 11)
                    {
                        char name[12];
                        al_fread(file, name, 11);
                        i = al_fgetc(file); /* Size. */
                        name[11] = '\0';
                        if (!strcmp(name, "NETSCAPE2.0"))
                        {
                            if (i == 3)
                            {
                                j = al_fgetc(file);
                                m_anim->loop = al_fread16le(file);
                                if (j != 1)
                                    m_anim->loop = 0;
                                i = al_fgetc(file); /* Size. */
                            }
                        }
                    }
                }

                /* Possibly more blocks until terminator block (0). */
                while (i)
                {
                    al_fseek(file, i, ALLEGRO_SEEK_CUR);
                    i = al_fgetc(file);
                }
                break;
            case 0x3b:
                /* GIF Trailer. */
                if (del_fp) {
                    al_fclose(file);
                    file = nullptr;
                }
                return true;
            }
        } while (true);

        freeup();
        return false;
    }

    bool GIF::algif_load_animation_f(ALLEGRO_FILE*& file) {
        if (!algif_load_raw(file, false)) return false;        

        al_init_primitives_addon();

        m_anim->duration = 0;
        ALLEGRO_STATE s;
        al_store_state(&s, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

        for (int i = 0; i < m_anim->frames_count; i++) {
            ALGIF_FRAME* f = &m_anim->frames[i];
            if (i == 0)
                f->rendered = al_create_bitmap(m_anim->width, m_anim->height);
            else
                f->rendered = al_clone_bitmap(m_anim->frames[i - 1].rendered);
            al_set_target_bitmap(f->rendered);
            algif_render_frame(i, 0, 0);
            m_anim->duration += f->duration;
        }

        al_restore_state(&s);
        return true;
    }

    bool GIF::algif_load_animation(char const* filename) {
        ALLEGRO_FILE* file = al_fopen(filename, "rb");
        const auto res = algif_load_animation_f(file);
        if (file) al_fclose(file);
        return res;
    }

	/* Renders the next frame in a GIF animation to the given position.
     * You need to call this in order on the same destination for frames
     * [0..gif->frames_count - 1] to properly render all the frames in the GIF.
     * The current target bitmap should have the same height as the animation,
     * and blending should be set to fully copy RGBA.
     */
    void GIF::algif_render_frame(int frame, int xpos, int ypos) {
        int x, y, w, h;
        ALGIF_FRAME *f = (m_anim->frames + frame);
        ALGIF_PALETTE *pal;
        if (frame == 0) {
            al_draw_filled_rectangle(xpos, ypos, xpos + m_anim->width,
                  ypos + m_anim->height, al_map_rgba_f(0, 0, 0, 0));
        }
        else {
            ALGIF_FRAME *p = (m_anim->frames + (frame - 1));
            if (p->disposal_method == 2) {
                al_draw_filled_rectangle(xpos + p->xoff, ypos + p->yoff,
                    xpos + p->xoff + p->bitmap_8_bit->w,
                    ypos + p->yoff + p->bitmap_8_bit->h,
                    al_map_rgba_f(0, 0, 0, 0));
            }
            else if (p->disposal_method == 3 && m_anim->store) {
                al_draw_bitmap_region(m_anim->store, xpos + p->xoff, ypos + p->yoff,
                    p->bitmap_8_bit->w,
                    p->bitmap_8_bit->h,
                    xpos + p->xoff, ypos + p->yoff, 0);
                al_destroy_bitmap(m_anim->store);
                m_anim->store = nullptr;
            }
        }
        w = f->bitmap_8_bit->w;
        h = f->bitmap_8_bit->h;
        if (f->disposal_method == 3) {
            if (m_anim->store)
                al_destroy_bitmap(m_anim->store);
            m_anim->store = al_clone_bitmap(al_get_target_bitmap());
        }
        pal = &((m_anim->frames + frame)->palette);
        if (pal->colors_count == 0)
            pal = &(m_anim->palette);

        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                int c = f->bitmap_8_bit->data[x + y * f->bitmap_8_bit->w];
                if (c != f->transparent_index) {
                    al_draw_pixel(xpos + f->xoff + x, ypos + f->yoff + y,
                        al_map_rgb(pal->colors[c].r, pal->colors[c].g,
                            pal->colors[c].b));
                }
            }
        }
    }

    /* Destroy a complete gif, including all frames. */
    void GIF::algif_destroy_animation() {
        if (!m_anim) return;

        for (int i = 0; i < m_anim->frames_count; i++)
        {
            ALGIF_FRAME* frame = m_anim->frames + i;

            if (frame->bitmap_8_bit)
                algif_destroy_bitmap(frame->bitmap_8_bit);
            if (frame->rendered)
                al_destroy_bitmap(frame->rendered);
        }
        if (m_anim->store)
            al_destroy_bitmap(m_anim->store);
        free(m_anim->frames);
        free(m_anim);
        m_anim = nullptr;
    }

    GIF::ALGIF_BITMAP* GIF::algif_create_bitmap(int w, int h) {
        ALGIF_BITMAP* bitmap = (ALGIF_BITMAP*)calloc(1, sizeof(ALGIF_BITMAP));
        if (!bitmap) throw std::bad_alloc();
        bitmap->w = w;
        bitmap->h = h;
        bitmap->data = (uint8_t*)calloc(1, static_cast<size_t>(w * h));
        return bitmap;
    }

    void GIF::algif_destroy_bitmap(ALGIF_BITMAP*& bitmap) {
        free(bitmap->data);
        free(bitmap);
        bitmap = nullptr;
    }

    void GIF::algif_blit(ALGIF_BITMAP* const& from, ALGIF_BITMAP* const& to, int xf, int yf, int xt, int yt,
        int w, int h) {

        if (w <= 0 || h <= 0)
            return;

        /* source clipping */
        if (xf < 0) {
            w += xf;
            xt -= xf;
            xf = 0;
        }
        if (yf < 0) {
            h += yf;
            yt -= yf;
            yf = 0;
        }
        int wf = from->w;
        int hf = from->h;
        if (xf + w > wf) {
            w = wf - xf;
        }
        if (yf + h > hf) {
            h = hf - yf;
        }

        /* destination clipping */
        if (xt < 0) {
            w += xt;
            xf -= xt;
            xt = 0;
        }
        if (yt < 0) {
            h += yt;
            yf -= yt;
            yt = 0;
        }
        int wt = to->w;
        int ht = to->h;
        if (xt + w > wt) {
            w = wt - xt;
        }
        if (yt + h > ht) {
            h = ht - yt;
        }

        if (w <= 0 || h <= 0)
            return;

        /* copy row by row */
        uint8_t* pf = from->data + yf * from->w;
        uint8_t* pt = to->data + yt * to->w;
        for (int i = 0; i < h; i++) {
            memmove(pt + xt, pf + xf, w);
            pf += from->w;
            pt += to->w;
        }
    }

    ALLEGRO_BITMAP* GIF::algif_get_bitmap(double seconds) const {
        int n = m_anim->frames_count;
        seconds = fmod(seconds, m_anim->duration / 100.0);
        double d = 0.0;
        for (int i = 0; i < n; i++) {
            d += m_anim->frames[i].duration / 100.0;
            if (seconds < d)
                return m_anim->frames[i].rendered;
        }
        return m_anim->frames[0].rendered;
    }

    ALLEGRO_BITMAP* GIF::algif_get_frame_bitmap(int i) const {
        return m_anim->frames[i].rendered;
    }

    double GIF::algif_get_frame_duration(int i) const {
        return m_anim->frames[i].duration * 1.0 / 100.0;
    }

    ALLEGRO_BITMAP* GIF::get_for_draw() const
    {
        return m_anim ? algif_get_bitmap(al_get_time() - m_start_time) : nullptr;
    }

    GIF::GIF(GIF&& oth) noexcept
        : m_anim(std::exchange(oth.m_anim, nullptr))
    {
    }

    void GIF::operator=(GIF&& oth) noexcept
    {
        destroy();
        m_anim = std::exchange(oth.m_anim, nullptr);
    }

    GIF::GIF(const std::string& path, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0)
    {
        if (!load(path, flags, format))
            throw std::runtime_error("Cannot load GIF!");
    }

    GIF::GIF(ALLEGRO_FILE* fp)
    {
        if (!load(fp))
            throw std::runtime_error("Cannot load GIF from FILE!");
    }

    bool GIF::load(const std::string& path, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0)
    {
        if (!al_is_system_installed()) al_init();

        destroy();

        if (path.empty()) return false;

        al_set_new_bitmap_flags(flags);
        al_set_new_bitmap_format(format);

        if (!algif_load_animation(path.c_str())) return false;

        m_start_time = al_get_time();

        return m_anim != nullptr;
    }

    bool GIF::load(ALLEGRO_FILE* fp)
    {
        if (!al_is_system_installed()) al_init();

        destroy();

        if (!fp) return false;

        if (!algif_load_animation_f(fp)) return false;
        //fileref->modify_no_destroy(true); // unhappy

        m_start_time = al_get_time();

        return m_anim != nullptr;
    }

    int GIF::get_width() const
    {
        return m_anim ? m_anim->width : 0;
    }

    int GIF::get_height() const
    {
        return m_anim ? m_anim->height : 0;
    }

    ALLEGRO_BITMAP* GIF::get_raw_bitmap() const
    {
        return m_anim ? algif_get_bitmap(al_get_time() - m_start_time) : nullptr;
    }

    GIF::operator ALLEGRO_BITMAP* () const
    {
        return get_raw_bitmap();
    }

    bool GIF::valid() const
    {
        return m_anim != nullptr;
    }

    GIF::operator bool() const
    {
        return m_anim != nullptr;
    }

    bool GIF::empty() const
    {
        return !valid();
    }

    void GIF::destroy()
    {
        algif_destroy_animation();
    }

    double GIF::get_interval_average() const
    {
        if (!m_anim) return -1.0;
        if (m_anim->frames_count == 0) return -1.0;

        double total = 0.0;

        for (int p = 0; p < m_anim->frames_count; p++) {
            total += algif_get_frame_duration(p);
        }

        total /= 1.0 * m_anim->frames_count;
        return total;
    }

    double GIF::get_interval_longest() const
    {
        if (!m_anim) return -1.0;
        if (m_anim->frames_count == 0) return -1.0;

        double total = 0.0;

        for (int p = 0; p < m_anim->frames_count; p++) {
            const double rn = algif_get_frame_duration(p);
            if (rn > total) total = rn;
        }

        return total;
    }

    double GIF::get_interval_shortest() const
    {
        if (!m_anim) return -1.0;
        if (m_anim->frames_count == 0) return -1.0;

        double total = algif_get_frame_duration(0);

        for (int p = 0; p < m_anim->frames_count; p++) {
            const double rn = algif_get_frame_duration(p);
            if (rn < total) total = rn;
        }

        return total;
    }

    size_t GIF::get_amount_of_frames() const
    {
        return m_anim ? static_cast<size_t>(m_anim->frames_count > 0 ? m_anim->frames_count : 0) : 0;
    }

    ALLEGRO_BITMAP* GIF::index_raw(const size_t p) const
    {
        if (p >= get_amount_of_frames()) return nullptr;
        return algif_get_frame_bitmap(static_cast<int>(p));
    }

    Bitmap GIF::index(const size_t p) const
    {
        return make_const_bitmap_of(index_raw(p));
    }

    double GIF::get_interval_of_index(const size_t p) const
    {
        if (!m_anim) return -1.0;
        if (get_amount_of_frames() <= p) return -1.0;

        return algif_get_frame_duration(static_cast<int>(p));
    }
}