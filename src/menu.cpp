#include "menu.h"

#include <mutex>

namespace AllegroCPP {

    struct __magic_map_safe {
        std::shared_mutex smtx;
        std::unordered_map<ALLEGRO_EVENT_SOURCE*, Menu*> map;
    } __magic_map;

    void __display_menu_allegro_start()
    {
        if (!al_is_system_installed() && !al_init()) throw std::runtime_error("Can't start Allegro!");
        if (!al_is_native_dialog_addon_initialized() && !al_init_native_dialog_addon()) throw std::runtime_error("Can't start Native Dialog!");
    }

    Menu* __magic_find_menu_from_evsrc(ALLEGRO_EVENT_SOURCE* evsrc)
    {
        std::shared_lock<std::shared_mutex> l(__magic_map.smtx);

        auto it = __magic_map.map.find(evsrc);
        if (it != __magic_map.map.end()) return it->second;

        return nullptr;
    }

    void __magic_push_menu(Menu* self)
    {
        std::unique_lock<std::shared_mutex> l(__magic_map.smtx);

        __magic_map.map[*self] = self;
    }

    void __magic_pop_menu(ALLEGRO_EVENT_SOURCE* self)
    {
        std::unique_lock<std::shared_mutex> l(__magic_map.smtx);

        auto it = __magic_map.map.find(self);
        if (it != __magic_map.map.end()) __magic_map.map.erase(it);
    }

    bool set_menu_item(ALLEGRO_MENU* parent, int pos, char const* title, uint16_t id, int flags, ALLEGRO_BITMAP* icon, ALLEGRO_MENU* submenu)
    {
        if (!parent) return false;
        al_remove_menu_item(parent, pos); // may fail because item may not exist
        return al_insert_menu_item(parent, pos, title, id, flags, icon, submenu) >= 0;
    }

    __menu_structure::__menu_structure(__menu_structure&& oth) noexcept
    {
        Menu = oth.Menu;
        parent = oth.parent;
        idx = oth.idx;
        id = oth.id;
        sub_menus = std::move(oth.sub_menus);

        oth.Menu = nullptr;
        oth.parent = nullptr;
        oth.idx = -1;
        oth.id = 0;
        oth.sub_menus.clear();
    }

    void __menu_structure::operator=(__menu_structure&& oth) noexcept
    {
        if (parent && idx >= 0) {
            al_remove_menu_item(parent, -idx); // delete this and all related to this
        }

        Menu = oth.Menu;
        parent = oth.parent;
        idx = oth.idx;
        id = oth.id;
        sub_menus = std::move(oth.sub_menus);

        oth.Menu = nullptr;
        oth.parent = nullptr;
        oth.idx = -1;
        oth.id = 0;
        oth.sub_menus.clear();
    }

    void __menu_structure::rebuild(const int forced_flag)
    {
        if (!parent) return;

        if (id == static_cast<uint16_t>(-1)) {
            if (!al_remove_menu_item(parent, -idx)) throw std::runtime_error("Can't find itself!");
            if (!set_menu_item(parent, -idx, nullptr, 0, 0, nullptr, nullptr)) throw std::bad_alloc();
            sub_menus.clear(); // no sub Menu anymore
            return;
        }

        std::string _nam = "<empty>";
        if (const char* __tmp = al_get_menu_item_caption(parent, -idx); __tmp) _nam = __tmp;

        int _flags = forced_flag;
        if (_flags == 0) _flags = al_get_menu_item_flags(parent, -idx);
        if (_flags < 0) _flags = 0;

        //if (!al_remove_menu_item(parent, -idx)) throw std::runtime_error("Can't find itself.");

        if (!sub_menus.empty()) {
            Menu = recursive_rebuild(sub_menus);
        }
        else Menu = nullptr;

        if (!set_menu_item(parent, -idx, _nam.c_str(), id, _flags, nullptr, Menu)) throw std::bad_alloc();
    }

    __menu_structure* __menu_structure::get_self() const
    {
        return (__menu_structure*)this;
    }

    ALLEGRO_MENU* __menu_structure::recursive_rebuild(const std::map<int, __menu_structure>& vc)
    {
        if (vc.empty()) return nullptr;

        ALLEGRO_MENU* bottom = al_create_menu();
        if (!bottom) throw std::runtime_error("Failed to create menu! FATAL ERROR!");

        for (const auto& it : vc) {

            if (it.second.idx < 0) { // empty space
                if (!set_menu_item(
                    bottom,
                    -it.first,
                    nullptr,
                    it.second.id,
                    0,
                    nullptr,
                    nullptr)) throw std::bad_alloc();
            }
            else {
                if (!set_menu_item(
                    bottom,
                    -it.first,
                    al_get_menu_item_caption(it.second.parent, -it.second.idx),
                    it.second.id,
                    al_get_menu_item_flags(it.second.parent, -it.second.idx),
                    nullptr,
                    it.second.sub_menus.size() ? recursive_rebuild(it.second.sub_menus) : nullptr)) throw std::bad_alloc();
            }
        }

        return bottom;
    }

    Menu_quick::Menu_quick(const std::string& s, const uint16_t i, const menu_flags f, const std::vector<Menu_quick> l)
        : caption(s), id(i), flags(f), lst(l)
    {
    }

    Menu_quick::Menu_quick(const __menu_structure& ms) :
        caption((ms.id == static_cast<uint16_t>(-1) || ms.idx < 0) ? std::string{} : (ms.parent ? (al_get_menu_item_caption(ms.parent, -ms.idx)) : std::string{})),
        id(ms.id),
        flags(static_cast<menu_flags>((ms.id == static_cast<uint16_t>(-1) || ms.idx < 0) ? 0 : (ms.parent ? (al_get_menu_item_flags(ms.parent, -ms.idx)) : 0))),
        lst(ms.sub_menus.empty() ? std::vector<Menu_quick>{} : [&ms]() { std::vector<Menu_quick> mq; for (const auto& m : ms.sub_menus) { mq.push_back(Menu_quick{ m.second }); } return mq; }())
    {
    }

    Menu_each_menu::Menu_each_menu(const std::string& s, const uint16_t i, const menu_flags f, const std::vector<Menu_quick> l)
        : Menu_quick(s, i, f, l)
    {
    }

    Menu_each_menu::Menu_each_menu(const std::string& s, const uint16_t i, const std::vector<Menu_quick> l)
        : Menu_quick(s, i, static_cast<menu_flags>(0), l)
    {
    }

    Menu_each_menu::Menu_each_menu()
        : Menu_quick("<empty>", 0, static_cast<menu_flags>(0), std::vector<Menu_quick>{})
    {
    }

    Menu_each_menu& Menu_each_menu::set_name(const std::string& s)
    {
        caption = s;
        return *this;
    }

    Menu_each_menu& Menu_each_menu::set_caption(const std::string& s)
    {
        caption = s;
        return *this;
    }

    Menu_each_menu& Menu_each_menu::set_id(const uint16_t i)
    {
        id = i;
        return *this;
    }

    Menu_each_menu& Menu_each_menu::set_flags(menu_flags f)
    {
        flags = f;
        return *this;
    }

    Menu_each_menu& Menu_each_menu::set_flags(const int f)
    {
        flags = static_cast<menu_flags>(f);
        return *this;
    }

    Menu_each_menu& Menu_each_menu::push(const Menu_quick& m)
    {
        lst.push_back(m);
        return *this;
    }

    Menu_each_empty::Menu_each_empty()
        : Menu_quick(std::string{}, static_cast<uint16_t>(-1), static_cast<menu_flags>(0), std::vector<Menu_quick>{})
    {
    }

    Menu_each_default::Menu_each_default(const std::string& s, const uint16_t i, const menu_flags f)
        : Menu_quick(s, i, f, std::vector<Menu_quick>{})
    {
    }

    Menu_each_default::Menu_each_default()
        : Menu_quick("<empty>", 0, static_cast<menu_flags>(0), std::vector<Menu_quick>{})
    {
    }

    Menu_each_default& Menu_each_default::set_name(const std::string& s)
    {
        caption = s;
        return *this;
    }

    Menu_each_default& Menu_each_default::set_caption(const std::string& s)
    {
        caption = s;
        return *this;
    }

    Menu_each_default& Menu_each_default::set_id(const uint16_t i)
    {
        id = i;
        return *this;
    }

    Menu_each_default& Menu_each_default::set_flags(const menu_flags f)
    {
        flags = f;
        return *this;
    }

    Menu_each_default& Menu_each_default::set_flags(const int f)
    {
        flags = static_cast<menu_flags>(f);
        return *this;
    }

    Menu_handler::Menu_handler(Menu_handler&& oth) noexcept
        : curr(oth.curr), _counter(oth._counter)
    {
    }

    Menu_handler::Menu_handler(__menu_structure& s, std::atomic<uint16_t>& i)
        : curr(s), _counter(i)
    {
    }

    void Menu_handler::set_name(const std::string& str)
    {
        if (!str.empty())
            al_set_menu_item_caption(curr.parent, -curr.idx, str.c_str());
    }

    void Menu_handler::set_caption(const std::string& str)
    {
        if (!str.empty())
            al_set_menu_item_caption(curr.parent, -curr.idx, str.c_str());
    }

    void Menu_handler::set_flags(const menu_flags flg)
    {
        const bool need_reload = (static_cast<int>(flg) & ALLEGRO_MENU_ITEM_CHECKBOX) && !(al_get_menu_item_flags(curr.parent, -curr.idx) & ALLEGRO_MENU_ITEM_CHECKBOX);
        al_set_menu_item_flags(curr.parent, -curr.idx, al_get_menu_item_flags(curr.parent, -curr.idx) | static_cast<int>(flg));
        if (need_reload)
            curr.rebuild(al_get_menu_item_flags(curr.parent, -curr.idx) | static_cast<int>(flg));
    }

    void Menu_handler::unset_flags(const menu_flags flg)
    {
        const bool need_reload = (static_cast<int>(flg) & ALLEGRO_MENU_ITEM_CHECKBOX) && !(al_get_menu_item_flags(curr.parent, -curr.idx) & ALLEGRO_MENU_ITEM_CHECKBOX);
        al_set_menu_item_flags(curr.parent, -curr.idx, al_get_menu_item_flags(curr.parent, -curr.idx) & (~static_cast<int>(flg)));
        if (need_reload)
            curr.rebuild(al_get_menu_item_flags(curr.parent, -curr.idx) & (~static_cast<int>(flg)));
    }

    void Menu_handler::toggle_flags(const menu_flags flg)
    {
        const bool need_reload = (static_cast<int>(flg) & ALLEGRO_MENU_ITEM_CHECKBOX) && !(al_get_menu_item_flags(curr.parent, -curr.idx) & ALLEGRO_MENU_ITEM_CHECKBOX);
        al_set_menu_item_flags(curr.parent, -curr.idx, al_get_menu_item_flags(curr.parent, -curr.idx) ^ static_cast<int>(flg));
        if (need_reload)
            curr.rebuild(al_get_menu_item_flags(curr.parent, -curr.idx) ^ static_cast<int>(flg));
    }

    std::string Menu_handler::get_name() const
    {
        if (curr.parent) {
            if (const char* uknow = al_get_menu_item_caption(curr.parent, -curr.idx); uknow) return uknow;
        }
        return {};
    }

    std::string Menu_handler::get_caption() const
    {
        if (curr.parent) {
            if (const char* uknow = al_get_menu_item_caption(curr.parent, -curr.idx); uknow) return uknow;
        }
        return {};
    }

    menu_flags Menu_handler::get_flags() const
    {
        return static_cast<menu_flags>(curr.parent ? al_get_menu_item_flags(curr.parent, -curr.idx) : 0);
    }

    uint16_t Menu_handler::get_id() const
    {
        return curr.id;
    }

    int Menu_handler::get_index_pos() const
    {
        return curr.idx;
    }

    void Menu_handler::reset_id(const uint16_t nid)
    {
        if (curr.id == nid) return;
        curr.id = nid;
        curr.rebuild();
    }

    void Menu_handler::reset_to_division()
    {
        reset_id(static_cast<uint16_t>(-1));
    }

    Menu_handler Menu_handler::operator[](const int ix)
    {
        if (ix < 0) throw std::out_of_range("Index pos can't be lower than 0");
        if (curr.id == static_cast<uint16_t>(-1)) throw std::runtime_error("An empty space cannot have any menus.");
        if (!curr.sub_menus.empty() && (curr.sub_menus.rbegin()->first < (ix - 1))) throw std::out_of_range("Please do not increase 2 steps at a time! One by one, always.");
        __menu_structure& c2 = curr.sub_menus[ix];

        if (!c2.parent) {
            c2.id = ++_counter;
            c2.idx = ix;

            if (!curr.Menu) {
                curr.Menu = al_create_menu();
                if (!curr.Menu) throw std::bad_alloc();

                std::string _nam = "<empty>";
                if (const char* __tmp = al_get_menu_item_caption(curr.parent, -curr.idx); __tmp) _nam = __tmp;
                int _flags = al_get_menu_item_flags(curr.parent, -curr.idx);
                if (_flags < 0) _flags = 0;

                al_remove_menu_item(curr.parent, -curr.idx);

                if (!set_menu_item(curr.parent, -curr.idx, _nam.c_str(), curr.id, _flags, nullptr, curr.Menu)) throw std::bad_alloc();
            }

            if (!set_menu_item(curr.Menu, -c2.idx, "<empty>", c2.id, 0, nullptr, nullptr)) throw std::bad_alloc();

            c2.Menu = nullptr;
            c2.parent = curr.Menu;
        }

        return Menu_handler{ c2, _counter };
    }

    Menu_handler Menu_handler::operator[](const std::string& key)
    {
        for (const auto& it : curr.sub_menus)
        {
            if (it.second.parent) {
                if (const char* uknow = al_get_menu_item_caption(it.second.parent, -it.second.idx); uknow && key == uknow) {
                    return this->operator[](it.first);
                }
            }
        }
        if (curr.sub_menus.empty()) {
            Menu_handler nh = this->operator[](0);
            nh.set_caption(key);
            return nh;
        }
        else {
            Menu_handler nh = this->operator[](curr.sub_menus.rbegin()->first + 1);
            nh.set_caption(key);
            return nh;
        }
    }

    bool Menu_handler::remove(const int ix)
    {
        if (ix < 0) return false;
        if (curr.sub_menus.empty() || (curr.sub_menus.rbegin()->first < ix)) return false;

        al_remove_menu_item(curr.Menu, -ix);
        curr.sub_menus.erase(ix);

        if (curr.sub_menus.empty()) {
            curr.rebuild();
            return true;
        }

        std::vector<int> mov;
        for (const auto& i : curr.sub_menus) {
            if (i.first > ix) mov.push_back(i.first);
        }

        for (const auto& j : mov) {
            curr.sub_menus[j - 1] = std::move(curr.sub_menus[j]);
            curr.sub_menus[j - 1].idx = j - 1;
            curr.sub_menus.erase(j);
        }

        return true;
    }

    bool Menu_handler::remove_all()
    {
        if (curr.sub_menus.empty()) return false;

        curr.sub_menus.clear();
        while (al_remove_menu_item(curr.Menu, 0));

        curr.rebuild();
        return true;
    }

    bool Menu_handler::remove(const std::string& key)
    {
        for (const auto& it : curr.sub_menus)
        {
            if (it.second.parent) {
                if (const char* uknow = al_get_menu_item_caption(it.second.parent, -it.second.idx); uknow && key == uknow) {
                    return remove(it.first);
                }
            }
        }
        return false;
    }

    bool Menu_handler::pop_back()
    {
        if (size() > 0) {
            return remove(static_cast<int>(size()) - 1);
        }
        return false;
    }

    bool Menu_handler::pop_front()
    {
        if (size() > 0) {
            return remove(0);
        }
        return false;
    }

    size_t Menu_handler::size() const
    {
        return curr.sub_menus.size();
    }

    int Menu_handler::push(const Menu_quick& mq)
    {
        const int ix = (curr.sub_menus.size() > 0) ? (curr.sub_menus.rbegin()->first + 1) : 0;
        __menu_structure& c2 = curr.sub_menus[ix];

        if (!curr.Menu) { // must have Menu
            curr.Menu = al_create_menu();
            if (!curr.Menu) throw std::bad_alloc();

            std::string _nam = "<empty>";
            if (const char* __tmp = al_get_menu_item_caption(curr.parent, -curr.idx); __tmp) _nam = __tmp;
            int _flags = al_get_menu_item_flags(curr.parent, -curr.idx);
            if (_flags < 0) _flags = 0;

            if (!set_menu_item(curr.parent, -curr.idx, _nam.c_str(), curr.id, _flags, nullptr, curr.Menu)) throw std::bad_alloc();
        }


        c2.id = mq.id;
        c2.parent = curr.Menu;
        c2.idx = ix;

        if (c2.id == static_cast<uint16_t>(-1)) {
            if (!set_menu_item(c2.parent, -c2.idx, nullptr, 0, 0, nullptr, nullptr)) throw std::bad_alloc();
            return ix;
        }

        if (mq.lst.size() > 0) {
            c2.Menu = al_create_menu();
        }
        else c2.Menu = nullptr;

        if (!set_menu_item(c2.parent, -c2.idx, mq.caption.c_str(), c2.id, static_cast<int>(mq.flags), nullptr, c2.Menu)) throw std::bad_alloc();

        Menu_handler mh{ c2, _counter };
        if (mq.lst.size()) mh.push(mq.lst);

        return ix;
    }

    void Menu_handler::push(const std::vector<Menu_quick>& mq)
    {
        for (const auto& it : mq) push(it);
    }

    uint16_t Menu::find_greatest_id(const __menu_structure& ms) const
    {
        uint16_t v = 0;
        for (const auto& i : ms.sub_menus) {
            if (i.second.id != static_cast<uint16_t>(-1) && i.second.id > v) v = i.second.id;
            if (!i.second.sub_menus.empty()) {
                if (uint16_t ct = find_greatest_id(i.second); ct > v) v = ct;
            }
        }
        return v;
    }

    __menu_structure* Menu::find_anywhere(const std::map<int, __menu_structure>& m, const std::function<bool(const std::pair<const int, __menu_structure>&)>& f)
    {
        for (auto e = m.cbegin(); e != m.cend(); ++e) {
            if (f(*e)) return e->second.get_self();
            if (!e->second.sub_menus.empty()) {
                if (__menu_structure* ptr = find_anywhere(e->second.sub_menus, f); ptr) return ptr;
            }
        }

        return nullptr;
    }

    bool Menu::make_self_as()
    {
        if (curr.Menu) {
            ev_source = al_enable_menu_event_source(curr.Menu);
            __magic_push_menu(this);
            return true;
        }
        if ((curr.Menu = (mmt == menu_type::BAR ? al_create_menu() : al_create_popup_menu())) != nullptr)
        {
            ev_source = al_enable_menu_event_source(curr.Menu);
            __magic_push_menu(this);
            return true;
        }
        return false;
    }

    std::shared_ptr<ALLEGRO_DISPLAY> Menu::_get_display() const
    {
        const auto ptr = last_applied_display.lock();
        if (ptr.get()) return ptr;
        return {};
    }

    Menu::Menu(const menu_type mt)
        : mmt(mt)
    {
        __display_menu_allegro_start();
        if (!make_self_as()) throw std::runtime_error("Can't create menu!");
    }

    Menu::Menu(std::weak_ptr<ALLEGRO_DISPLAY> nd, const menu_type mt)
        : mmt(mt)
    {
        __display_menu_allegro_start();
        if (!make_self_as()) throw std::runtime_error("Can't create menu!");
        last_applied_display = nd;
    }

    Menu::Menu(Menu&& oth) noexcept
        : mmt(oth.mmt)
    {
        __display_menu_allegro_start();
        curr = std::move(oth.curr);
        last_applied_display = oth.last_applied_display;
        make_self_as();
        oth.curr.Menu = nullptr; // really really sure
        oth.ev_source = nullptr;
        oth.last_applied_display.reset();
    }

    Menu::Menu(const std::initializer_list<Menu_quick> lst)
        : mmt(menu_type::BAR)
    {
        __display_menu_allegro_start();
        if (!make_self_as()) throw std::runtime_error("Can't create menu!");
        for (const auto& each : lst) push(each);
    }

    Menu::Menu(const menu_type mt, const std::initializer_list<Menu_quick> lst)
        : mmt(mt)
    {
        __display_menu_allegro_start();
        if (!make_self_as()) throw std::runtime_error("Can't create menu!");
        for (const auto& each : lst) push(each);
    }

    Menu::Menu(std::weak_ptr<ALLEGRO_DISPLAY> nd, const std::initializer_list<Menu_quick> lst)
        : mmt(menu_type::BAR)
    {
        __display_menu_allegro_start();
        if (!make_self_as()) throw std::runtime_error("Can't create menu!");
        for (const auto& each : lst) push(each);
        last_applied_display = nd;
    }

    Menu::Menu(std::weak_ptr<ALLEGRO_DISPLAY> nd, const menu_type mt, const std::initializer_list<Menu_quick> lst)
        : mmt(mt)
    {
        __display_menu_allegro_start();
        if (!make_self_as()) throw std::runtime_error("Can't create menu!");
        for (const auto& each : lst) push(each);
        last_applied_display = nd;
    }

    Menu::~Menu()
    {
        if (!curr.Menu) return;
        if (mmt == menu_type::BAR) hide();

        __magic_pop_menu(ev_source);
        al_disable_menu_event_source(curr.Menu);
        ev_source = nullptr;
        al_destroy_menu(curr.Menu);
        curr.Menu = nullptr;
    }

    Menu_handler Menu::operator[](const int ix)
    {
        if (ix < 0) throw std::out_of_range("Index pos can't be lower than 0");
        if (!curr.sub_menus.empty() && (curr.sub_menus.rbegin()->first < (ix - 1))) throw std::out_of_range("Please do not increase 2 steps directly! One by one, always.");
        __menu_structure& c2 = curr.sub_menus[ix];
        int rs = 0;
        if (!c2.parent) {

            c2.id = ++_counter;
            c2.parent = curr.Menu;
            c2.idx = ix;
            c2.Menu = nullptr;// al_create_menu();

            if (!c2.Menu) throw std::bad_alloc();

            if (!set_menu_item(curr.Menu, -c2.idx, "<empty>", c2.id, 0, nullptr, c2.Menu)) throw std::bad_alloc();
        }

        return Menu_handler{ c2, _counter };
    }

    Menu_handler Menu::operator[](const std::string& key)
    {
        for (const auto& it : curr.sub_menus)
        {
            if (it.second.parent) {
                if (const char* uknow = al_get_menu_item_caption(it.second.parent, -it.second.idx); uknow && key == uknow) {
                    return this->operator[](it.first);
                }
            }
        }
        if (curr.sub_menus.empty()) {
            Menu_handler nh = this->operator[](0);
            nh.set_caption(key);
            return nh;
        }
        else {
            Menu_handler nh = this->operator[](curr.sub_menus.rbegin()->first + 1);
            nh.set_caption(key);
            return nh;
        }
    }

    bool Menu::remove(const int ix)
    {
        if (ix < 0) return false;
        if (curr.sub_menus.empty() || ix > curr.sub_menus.rbegin()->first) return false;
        al_remove_menu_item(curr.Menu, -ix);
        curr.sub_menus.erase(ix);

        std::vector<int> mov;
        for (const auto& i : curr.sub_menus) {
            if (i.first > ix) mov.push_back(i.first);
        }

        for (const auto& j : mov) {
            curr.sub_menus[j - 1] = std::move(curr.sub_menus[j]);
            curr.sub_menus[j - 1].idx = j - 1;
            curr.sub_menus.erase(j);
        }

        return true;
    }

    bool Menu::remove_all()
    {
        if (curr.sub_menus.empty()) return false;

        curr.sub_menus.clear();
        while (al_remove_menu_item(curr.Menu, 0));

        curr.rebuild();
        return true;
    }

    bool Menu::remove(const std::string& key)
    {
        for (const auto& it : curr.sub_menus)
        {
            if (it.second.parent) {
                if (const char* uknow = al_get_menu_item_caption(it.second.parent, -it.second.idx); uknow && key == uknow) {
                    return remove(it.first);
                }
            }
        }
        return false;
    }

    bool Menu::pop_back()
    {
        if (size() > 0) {
            remove(static_cast<int>(size()) - 1);
            return true;
        }
        return false;
    }

    bool Menu::pop_front()
    {
        if (size() > 0) {
            remove(0);
            return true;
        }
        return false;
    }

    size_t Menu::size() const
    {
        return curr.sub_menus.size();
    }

    bool Menu::empty() const
    {
        return curr.sub_menus.size() == 0;
    }

    bool Menu::valid() const
    {
        return curr.sub_menus.size() > 0 && curr.Menu;
    }

    Menu Menu::duplicate_as(const menu_type mt) const
    {
        Menu nmen(_get_display(), mt);
        Menu_quick mq(curr);
        for (const auto& m : mq.lst) nmen.push(m);
        return nmen;
    }

    Menu_handler Menu::find_id(const uint16_t ix)
    {
        __menu_structure* fnd = find_anywhere(curr.sub_menus, [&](const std::pair<const int, __menu_structure>& r) { return r.second.id == ix; });
        if (fnd) return Menu_handler{ *fnd, _counter };
        throw std::out_of_range("not found");
        return Menu_handler{ *fnd, _counter }; // never goes here
    }

    Menu_handler Menu::find(const std::string& ix)
    {
        __menu_structure* fnd = find_anywhere(curr.sub_menus, [&](const std::pair<const int, __menu_structure>& r) { if (const char* uknow = al_get_menu_item_caption(r.second.parent, -r.second.idx); uknow && ix == uknow) { return true; } return false; });
        if (fnd) return Menu_handler{ *fnd, _counter };
        throw std::out_of_range("not found");
        return Menu_handler{ *fnd, _counter }; // never goes here
    }

    void Menu::show(std::weak_ptr<ALLEGRO_DISPLAY> _d)
    {
        auto d = _d.lock();
        const auto last_cpy = _get_display();
        if (!d) d = last_cpy;
        if (!d) return;

        if (d && curr.Menu) {
            if (last_cpy.get() != d.get() && last_cpy) al_remove_display_menu(last_cpy.get());

            last_applied_display = d;

            if (mmt == menu_type::POPUP) al_popup_menu(curr.Menu, d.get());
            else al_set_display_menu(d.get(), curr.Menu);
        }
    }

    void Menu::hide(std::weak_ptr<ALLEGRO_DISPLAY> _d)
    {
        auto d = _d.lock();
        const auto last_cpy = _get_display();
        if (!d) d = last_cpy;
        if (!d) return;

        al_remove_display_menu(d.get());
    }

    int Menu::push(const Menu_quick& mq)
    {
        const int ix = (curr.sub_menus.size() > 0) ? (curr.sub_menus.rbegin()->first + 1) : 0;
        __menu_structure& c2 = curr.sub_menus[ix];

        c2.id = mq.id;
        c2.parent = curr.Menu;
        c2.idx = ix;
        c2.Menu = mq.lst.empty() ? nullptr : al_create_menu(); // must be Menu

        if (!set_menu_item(c2.parent, -c2.idx, mq.caption.c_str(), c2.id, static_cast<int>(mq.flags), nullptr, c2.Menu)) throw std::bad_alloc();

        if (mq.lst.size()) {
            Menu_handler mh{ c2, _counter };
            mh.push(mq.lst);
        }

        return ix;
    }

    void Menu::push(const std::vector<Menu_quick>& mq)
    {
        for (const auto& it : mq) push(it);
    }

    Menu::operator ALLEGRO_EVENT_SOURCE* () const
    {
        return ev_source;
    }

    ALLEGRO_EVENT_SOURCE* Menu::get_event_source() const
    {
        return ev_source;
    }

    Menu::operator ALLEGRO_MENU* () const
    {
        return curr.Menu;
    }

    void Menu::operator>>(std::weak_ptr<ALLEGRO_DISPLAY> d)
    {
        show(d);
    }

    Menu_event::Menu_event(const ALLEGRO_EVENT& ev)
        : _ref(*__magic_find_menu_from_evsrc(ev.any.source))
    {
        if (&_ref == nullptr) throw std::runtime_error("Menu was somehow invalid!");

        if (ev.type != ALLEGRO_EVENT_MENU_CLICK) return;

        if (!(source = (ALLEGRO_MENU*)ev.user.data3)) return; // set source


        uint16_t _id = static_cast<uint16_t>(ev.user.data1);
        std::string _from;
        if (const char* _f = al_get_menu_item_caption(source, _id); _f) _from = _f;

        if (_from.empty()) {
            try {
                handl = std::unique_ptr<Menu_handler>(new Menu_handler{ _ref.find(_from) });
            }
            catch (...) {} // not found means that is null
        }
        if (!handl) {
            try {
                handl = std::unique_ptr<Menu_handler>(new Menu_handler{ _ref.find_id(_id) });
            }
            catch (...) {} // not found means that is null
        }
    }

    Menu* Menu_event::operator->()
    {
        return &_ref;
    }

    Menu* Menu_event::operator->() const
    {
        return &_ref;
    }

    bool Menu_event::valid() const
    {
        return source && handl;
    }

    bool Menu_event::empty() const
    {
        return !valid();
    }

    std::string Menu_event::get_name() const
    {
        if (handl) return handl->get_caption();
        return {};
    }

    std::string Menu_event::get_caption() const
    {
        if (handl) return handl->get_caption();
        return {};
    }

    uint16_t Menu_event::get_id() const
    {
        if (handl) return handl->get_id();
        return 0;
    }

    menu_flags Menu_event::get_flags() const
    {
        if (handl) return handl->get_flags();
        return static_cast<menu_flags>(0);
    }

    bool Menu_event::has_flag(const menu_flags f) const
    {
        if (handl) return static_cast<int>(handl->get_flags() & f) != 0;
        return false;
    }

    bool Menu_event::is_checkbox() const
    {
        return has_flag(menu_flags::AS_CHECKBOX);
    }

    bool Menu_event::is_checked() const
    {
        return has_flag(menu_flags::AS_CHECKBOX) && has_flag(menu_flags::CHECKED);
    }

    bool Menu_event::is_enabled() const
    {
        return !has_flag(menu_flags::DISABLED);
    }

    bool Menu_event::is_disabled() const
    {
        return has_flag(menu_flags::DISABLED);
    }

    void Menu_event::patch_name(const std::string& str)
    {
        if (handl) handl->set_caption(str);
    }

    void Menu_event::patch_caption(const std::string& str)
    {
        if (handl) handl->set_caption(str);
    }

    void Menu_event::patch_flags(const menu_flags f)
    {
        if (handl) handl->set_flags(f);
    }

    void Menu_event::patch_toggle_flag(const menu_flags f)
    {
        if (handl) handl->toggle_flags(f);
    }

    Menu& Menu_event::get_source()
    {
        return _ref;
    }

}