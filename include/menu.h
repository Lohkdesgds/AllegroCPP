#pragma once


#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>

#include <stdexcept>
#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <shared_mutex>
#include <map>
#include <limits>
#include <vector>

#undef max

namespace AllegroCPP {

	enum class menu_flags {
		DISABLED = ALLEGRO_MENU_ITEM_DISABLED,
		AS_CHECKBOX = ALLEGRO_MENU_ITEM_CHECKBOX,
		CHECKED = ALLEGRO_MENU_ITEM_CHECKED
	};

	// easy operators to combine flags
	constexpr menu_flags operator|(const menu_flags& a, const menu_flags& b) { return static_cast<menu_flags>(static_cast<std::underlying_type_t<menu_flags>>(a) | static_cast<std::underlying_type_t<menu_flags>>(b)); }
	// easy way to get the flags it has
	constexpr menu_flags operator&(const menu_flags& a, const menu_flags& b) { return static_cast<menu_flags>(static_cast<std::underlying_type_t<menu_flags>>(a) & static_cast<std::underlying_type_t<menu_flags>>(b)); }

	/// <summary>
	/// <para>Easier way to just call insert or autoremove from list before inserting. It'll search for the pos value, then remove (if exists), then add.</para>
	/// </summary>
	/// <param name="{ALLEGRO_MENU*}">Parent.</param>
	/// <param name="{int}">Position (negative) or ID.</param>
	/// <param name="{char* const}">New name.</param>
	/// <param name="{uint16_t}">New ID.</param>
	/// <param name="{int}">New flags.</param>
	/// <param name="{ALLEGRO_BITMAP*}">Icon.</param>
	/// <param name="{ALLEGRO_MENU*}">Submenu.</param>
	/// <returns></returns>
	bool set_menu_item(ALLEGRO_MENU*, int, char const*, uint16_t, int, ALLEGRO_BITMAP*, ALLEGRO_MENU*);

	/// <summary>
	/// <para>This is the Menu structure used by Menu to handle stuff and keep things working.</para>
	/// <para>You won't use this directly.</para>
	/// </summary>
	struct __menu_structure {
		ALLEGRO_MENU* Menu = nullptr;
		ALLEGRO_MENU* parent = nullptr; // the one that has this one
		int idx = 0; // copy, index, >= 0 if valid or expected non empty
		uint16_t id = 0; // copy
		std::map<int, __menu_structure> sub_menus;

		__menu_structure() = default;
		__menu_structure(__menu_structure&&) noexcept;
		void operator=(__menu_structure&&) noexcept;

		/// <summary>
		/// <para>Rebuild all ALLEGRO Menu stuff of sub_menus to the Menu.</para>
		/// </summary>
		/// <param name="{int}">Forced flag (0 does nothing).</param>
		void rebuild(const int = 0);

		/// <summary>
		/// <para>Return itself as pointer</para>
		/// </summary>
		/// <returns></returns>
		__menu_structure* get_self() const;
	private:
		ALLEGRO_MENU* recursive_rebuild(const std::map<int, __menu_structure>&);
	};

	/// <summary>
	/// <para>Menu_quick, as the name suggests, is a quick way to build menus.</para>
	/// <para>When creating a brand new Menu or adding new options to a Menu, you should use this.</para>
	/// <para>You can create on the fly without this, but the steps to update name, ID, flags and other stuff are slower.</para>
	/// <para>So why not use this? This is faster and pretty powerful. Create menus with this today!</para>
	/// <para>NOTE: You should use the derived guys like Menu_each_menu, Menu_each_empty and menu_aech_default. This one directly works too, but yeah...</para>
	/// </summary>
	struct Menu_quick {
		std::string caption;
		uint16_t id = static_cast<uint16_t>(-1);
		menu_flags flags = static_cast<menu_flags>(0);
		std::vector<Menu_quick> lst;

		/// <summary>
		/// <para>Create Menu with name, ID, flags and optionally a list of sub-menus.</para>
		/// </summary>
		/// <param name="{std::string}">The caption to show.</param>
		/// <param name="{uint16_t}">The ID for events.</param>
		/// <param name="{menu_flags}">A combo of flags.</param>
		/// <param name="{vector}">Combo of sub-menus.</param>
		Menu_quick(const std::string&, const uint16_t, const menu_flags, const std::vector<Menu_quick> = {});

		/// <summary>
		/// <para>Build Menu_quick back from an existing __menu_structure (useful for rebuilds).</para>
		/// </summary>
		/// <param name="{__menu_structure}">A Menu structure.</param>
		Menu_quick(const __menu_structure&);
	};

	struct Menu_each_menu : public Menu_quick {
		/// <summary>
		/// <para>Create Menu with name, ID, flags and a list of sub-menus.</para>
		/// </summary>
		/// <param name="{std::string}">The caption to show.</param>
		/// <param name="{uint16_t}">The ID for events.</param>
		/// <param name="{menu_flags}">A combo of flags.</param>
		/// <param name="{vector}">Combo of sub-menus.</param>
		Menu_each_menu(const std::string&, const uint16_t, const menu_flags, const std::vector<Menu_quick>);

		/// <summary>
		/// <para>Create Menu with name, ID and a list of sub-menus (flag defaults to none).</para>
		/// </summary>
		/// <param name="{std::string}">The caption to show.</param>
		/// <param name="{uint16_t}">The ID for events.</param>
		/// <param name="{vector}">Combo of sub-menus.</param>
		Menu_each_menu(const std::string&, const uint16_t, const std::vector<Menu_quick>);

		/// <summary>
		/// <para>Create a Menu with no value set yet.</para>
		/// <para>You can use the set_** stuff to setup this object on the go.</para>
		/// </summary>
		Menu_each_menu();

		/// <summary>
		/// <para>Set object caption/name.</para>
		/// </summary>
		/// <param name="{std::string}">Caption/name</param>
		/// <returns>{Menu_each_menu&amp;} Itself</returns>
		Menu_each_menu& set_name(const std::string&);

		/// <summary>
		/// <para>Set object caption/name.</para>
		/// </summary>
		/// <param name="{std::string}">Caption/name</param>
		/// <returns>{Menu_each_menu&amp;} Itself</returns>
		Menu_each_menu& set_caption(const std::string&);

		/// <summary>
		/// <para>Set object ID (for events).</para>
		/// </summary>
		/// <param name="{uint16_t}">ID value.</param>
		/// <returns>{Menu_each_menu&amp;} Itself</returns>
		Menu_each_menu& set_id(const uint16_t);

		/// <summary>
		/// <para>Set object flags.</para>
		/// </summary>
		/// <param name="{menu_flags}">Combo of flags.</param>
		/// <returns>{Menu_each_menu&amp;} Itself</returns>
		Menu_each_menu& set_flags(menu_flags);

		/// <summary>
		/// <para>Set object flags.</para>
		/// </summary>
		/// <param name="{int}">Combo of flags (Allegro way).</param>
		/// <returns>{Menu_each_menu&amp;} Itself</returns>
		Menu_each_menu& set_flags(const int);

		/// <summary>
		/// <para>Add new sub-Menu to the list.</para>
		/// </summary>
		/// <param name="{Menu_quick}">Any sub-Menu type.</param>
		/// <returns>{Menu_each_menu&amp;} Itself</returns>
		Menu_each_menu& push(const Menu_quick&);
	};

	struct Menu_each_empty : public Menu_quick {
		Menu_each_empty();
	};

	struct Menu_each_default : public Menu_quick {
		Menu_each_default(const std::string&, const uint16_t, const menu_flags = static_cast<menu_flags>(0));
		Menu_each_default();

		/// <summary>
		/// <para>Set object caption/name.</para>
		/// </summary>
		/// <param name="{std::string}">Caption/name</param>
		/// <returns>{Menu_each_default&amp;} Itself</returns>
		Menu_each_default& set_name(const std::string&);

		/// <summary>
		/// <para>Set object caption/name.</para>
		/// </summary>
		/// <param name="{std::string}">Caption/name</param>
		/// <returns>{Menu_each_default&amp;} Itself</returns>
		Menu_each_default& set_caption(const std::string&);

		/// <summary>
		/// <para>Set object ID (for events).</para>
		/// </summary>
		/// <param name="{uint16_t}">ID value.</param>
		/// <returns>{Menu_each_default&amp;} Itself</returns>
		Menu_each_default& set_id(const uint16_t);

		/// <summary>
		/// <para>Set object flags.</para>
		/// </summary>
		/// <param name="{menu_flags}">Combo of flags.</param>
		/// <returns>{Menu_each_default&amp;} Itself</returns>
		Menu_each_default& set_flags(menu_flags);

		/// <summary>
		/// <para>Set object flags.</para>
		/// </summary>
		/// <param name="{int}">Combo of flags (Allegro way).</param>
		/// <returns>{Menu_each_default&amp;} Itself</returns>
		Menu_each_default& set_flags(const int);
	};

	/// <summary>
	/// <para>This is the one doing what you want. It'll update the flags and data internally as you say.</para>
	/// <para>This represents a Menu, combo of menus or even an empty space! You should know what you're doing before changing data around though.</para>
	/// </summary>
	class Menu_handler {
		__menu_structure& curr;
		std::atomic<uint16_t>& _counter;
	public:
		Menu_handler(const Menu_handler&) = delete;
		void operator=(const Menu_handler&) = delete;

		Menu_handler(Menu_handler&&) noexcept;
		Menu_handler(__menu_structure&, std::atomic<uint16_t>&);

		/// <summary>
		/// <para>Set object caption/name.</para>
		/// </summary>
		/// <param name="{std::string}">Caption/name</param>
		void set_name(const std::string&);

		/// <summary>
		/// <para>Set object caption/name.</para>
		/// </summary>
		/// <param name="{std::string}">Caption/name</param>
		void set_caption(const std::string&);

		/// <summary>
		/// <para>Add object flags (OR).</para>
		/// </summary>
		/// <param name="{menu_flags}">Combo of flags.</param>
		void set_flags(const menu_flags);

		/// <summary>
		/// <para>Remove object flags (~AND).</para>
		/// </summary>
		/// <param name="{menu_flags}">Combo of flags.</param>
		void unset_flags(const menu_flags);

		/// <summary>
		/// <para>Toggle object flags (XOR).</para>
		/// </summary>
		/// <param name="{menu_flags}">Combo of flags.</param>
		void toggle_flags(const menu_flags);

		/// <summary>
		/// <para>Get and load object name/caption.</para>
		/// </summary>
		/// <returns>{std::string} Name/caption.</returns>
		std::string get_name() const;

		/// <summary>
		/// <para>Get and load object name/caption.</para>
		/// </summary>
		/// <returns>{std::string} Name/caption.</returns>
		std::string get_caption() const;

		/// <summary>
		/// <para>Get current flags of this object.</para>
		/// </summary>
		/// <returns>{menu_flags} Combo of flags.</returns>
		menu_flags get_flags() const;

		/// <summary>
		/// <para>Get object ID (for events).</para>
		/// </summary>
		/// <returns>{uint16_t} Object's ID</returns>
		uint16_t get_id() const;

		/// <summary>
		/// <para>Get which row this object is.</para>
		/// </summary>
		/// <returns>{int} Row [0, ...).</returns>
		int get_index_pos() const;

		/// <summary>
		/// <para>Rebuild and change object ID.</para>
		/// <para>This is expensive, you should do this only if you really need that.</para>
		/// </summary>
		/// <param name="{uint16_t}">New object ID.</param>
		void reset_id(const uint16_t);

		/// <summary>
		/// <para>Make this a division (remove sub-menus and clean up).</para>
		/// </summary>
		void reset_to_division();

		/// <summary>
		/// <para>Get Menu index or create new one (if one ahead of the last one).</para>
		/// <para>This will throw if you skip one step (example: if there's 0 and 1 and you tried 3 (skipped 2)).</para>
		/// </summary>
		/// <param name="{int}">Index value.</param>
		/// <returns>{Menu_handler} A handler to this object.</returns>
		Menu_handler operator[](const int);

		/// <summary>
		/// <para>Search for a Menu by its name. If not found, one is created at the end with this name.</para>
		/// </summary>
		/// <param name="{std::string}">Caption/name of the Menu in the same level.</param>
		/// <returns>{Menu_handler} A handler to this object.</returns>
		Menu_handler operator[](const std::string&);

		/// <summary>
		/// <para>Remove an item by index.</para>
		/// </summary>
		/// <param name="{int}">Index, [0, ...).</param>
		/// <returns>{bool} True if removed something.</returns>
		bool remove(const int);

		/// <summary>
		/// <para>Remove all at once.</para>
		/// </summary>
		/// <returns>{bool} True if removed something.</returns>
		bool remove_all();

		/// <summary>
		/// <para>Remove an item by caption/name.</para>
		/// </summary>
		/// <param name="{std::string}">The object caption/name.</param>
		/// <returns>{bool} True if removed something.</returns>
		bool remove(const std::string&);

		/// <summary>
		/// <para>Remove the back object, if any object in list.</para>
		/// </summary>
		/// <returns>{bool} True if removed something.</returns>
		bool pop_back();

		/// <summary>
		/// <para>Remove the front object, if any object in list.</para>
		/// </summary>
		/// <returns>{bool} True if removed something.</returns>
		bool pop_front();

		/// <summary>
		/// <para>Get the amount of sub-menus in this Menu.</para>
		/// </summary>
		/// <returns>{size_t} Amount of sub-menus.</returns>
		size_t size() const;

		/// <summary>
		/// <para>Insert a new sub-Menu to the bottom of the list.</para>
		/// </summary>
		/// <param name="{Menu_quick}">A Menu_quick item.</param>
		/// <returns>{int} The index given to this new item.</returns>
		int push(const Menu_quick&);

		/// <summary>
		/// <para>Insert many new sub-menus in order to the bottom of the list.</para>
		/// </summary>
		/// <param name="{vector}">A vector of Menu_quick items.</param>
		void push(const std::vector<Menu_quick>&);
	};

	/// <summary>
	/// <para>Menu can be either a bar-styled Menu or a right-click-styled Menu.</para>
	/// <para>Menus are used for easy control on a display directly. Sometimes this is easier to use too.</para>
	/// </summary>
	class Menu {
	public:
		enum class menu_type {
			BAR,	// Expected bar within display 
			POPUP	// Pop up right next to the mouse
		};
	private:
		__menu_structure curr;
		std::atomic<uint16_t> _counter = 0;
		ALLEGRO_EVENT_SOURCE* ev_source = nullptr;
		const menu_type mmt = menu_type::BAR;
		ALLEGRO_DISPLAY* last_applied_display = nullptr;

		uint16_t find_greatest_id(const __menu_structure&) const;
		__menu_structure* find_anywhere(const std::map<int, __menu_structure>&, const std::function<bool(const std::pair<const int, __menu_structure>&)>&);
		bool make_self_as();
	public:
		Menu(const Menu&) = delete;
		void operator=(const Menu&) = delete;

		/// <summary>
		/// <para>Create a Menu of this type (defaults to BAR).</para>
		/// </summary>
		/// <param name="{menu_type}">What style of Menu?</param>
		Menu(const menu_type = menu_type::BAR);

		/// <summary>
		/// <para>Create a Menu of this type (defaults to BAR) with a pre-defined ALLEGRO_DISPLAY*.</para>
		/// <para>This is useful if you don't want to remember what display you want to show this on. The show/hide with no arguments will do things on this display.</para>
		/// <para>The target display can be modified with those functions anytime.</para>
		/// </summary>
		/// <param name="{ALLEGRO_DISPLAY*}">A display to put the Menu on (you have to show() manually when ready).</param>
		/// <param name="{menu_type}">What style of Menu?</param>
		Menu(ALLEGRO_DISPLAY*, const menu_type = menu_type::BAR);

		/// <summary>
		/// <para>Move the Menu around.</para>
		/// </summary>
		/// <param name="{Menu&amp;&amp;}">Menu being moved.</param>
		Menu(Menu&&) noexcept;

		/// <summary>
		/// <para>Directly create a Menu with items. Default type is BAR.</para>
		/// </summary>
		/// <param name="{initializer_list}">List of Menu_quick.</param>
		Menu(const std::initializer_list<Menu_quick>);

		/// <summary>
		/// <para>Directly create a Menu with a type and items.</para>
		/// </summary>
		/// <param name="{menu_type}">What style of Menu?</param>
		/// <param name="{initializer_list}">List of Menu_quick.</param>
		Menu(const menu_type, const std::initializer_list<Menu_quick>);

		/// <summary>
		/// <para>Directly create a Menu with items and a pre-defined ALLEGRO_DISPLAY*. Default type is BAR.</para>
		/// <para>This is useful if you don't want to remember what display you want to show this on. The show/hide with no arguments will do things on this display.</para>
		/// <para>The target display can be modified with those functions anytime.</para>
		/// </summary>
		/// <param name="{ALLEGRO_DISPLAY*}">A display to put the Menu on (you have to show() manually when ready).</param>
		/// <param name="{initializer_list}">List of Menu_quick.</param>
		Menu(ALLEGRO_DISPLAY*, const std::initializer_list<Menu_quick>);

		/// <summary>
		/// <para>Directly create a Menu with a pre-defined ALLEGRO_DISPLAY*, type and items ready to go.</para>
		/// <para>This is useful if you don't want to remember what display you want to show this on. The show/hide with no arguments will do things on this display.</para>
		/// <para>The target display can be modified with those functions anytime.</para>
		/// </summary>
		/// <param name="{ALLEGRO_DISPLAY*}">A display to put the Menu on (you have to show() manually when ready).</param>
		/// <param name="{menu_type}">What style of Menu?</param>
		/// <param name="{initializer_list}">List of Menu_quick.</param>
		Menu(ALLEGRO_DISPLAY*, const menu_type, const std::initializer_list<Menu_quick>);

		// Destroy the Menu.
		~Menu();

		void operator=(Menu&&) = delete; // Menu type may not be the same

		/// <summary>
		/// <para>Get Menu index or create new one (if one ahead of the last one).</para>
		/// <para>This will throw if you skip one step (example: if there's 0 and 1 and you tried 3 (skipped 2)).</para>
		/// </summary>
		/// <param name="{int}">Index value.</param>
		/// <returns>{Menu_handler} A handler to this object.</returns>
		Menu_handler operator[](const int);

		/// <summary>
		/// <para>Search for a Menu by its name. If not found, one is created at the end with this name.</para>
		/// </summary>
		/// <param name="{std::string}">Caption/name of the Menu in the same level.</param>
		/// <returns>{Menu_handler} A handler to this object.</returns>
		Menu_handler operator[](const std::string&);

		/// <summary>
		/// <para>Remove an item by index.</para>
		/// </summary>
		/// <param name="{int}">Index, [0, ...).</param>
		/// <returns>{bool} True if removed something.</returns>
		bool remove(const int);

		/// <summary>
		/// <para>Remove all at once.</para>
		/// </summary>
		/// <returns>{bool} True if removed something.</returns>
		bool remove_all();

		/// <summary>
		/// <para>Remove an item by caption/name.</para>
		/// </summary>
		/// <param name="{std::string}">The object caption/name.</param>
		/// <returns>{bool} True if removed something.</returns>
		bool remove(const std::string&);

		/// <summary>
		/// <para>Remove the back object, if any object in list.</para>
		/// </summary>
		/// <returns>{bool} True if removed something.</returns>
		bool pop_back();

		/// <summary>
		/// <para>Remove the front object, if any object in list.</para>
		/// </summary>
		/// <returns>{bool} True if removed something.</returns>
		bool pop_front();

		/// <summary>
		/// <para>Get the amount of sub-menus in this Menu.</para>
		/// </summary>
		/// <returns>{size_t} Amount of sub-menus.</returns>
		size_t size() const;

		/// <summary>
		/// <para>If there's no items, empty it is.</para>
		/// </summary>
		/// <returns>{bool} Does this have NO items?</returns>
		bool empty() const;

		/// <summary>
		/// <para>If there's an item in the Menu (at least one), it's valid.</para>
		/// </summary>
		/// <returns>{bool} Does this have any item to show?</returns>
		bool valid() const;

		/// <summary>
		/// <para>Get the items in this Menu and recreate as another Menu of any type.</para>
		/// </summary>
		/// <param name="{emnu_type}">A Menu type.</param>
		/// <returns>{Menu} New Menu.</returns>
		Menu duplicate_as(const menu_type) const;

		/// <summary>
		/// <para>Find in any depth an item with this ID.</para>
		/// <para>Throws out of range if no item is found.</para>
		/// </summary>
		/// <param name="{uint16_t}">Item ID.</param>
		/// <returns>{Menu_handler} If no exception, the Menu_handler for this ID.</returns>
		Menu_handler find_id(const uint16_t);

		/// <summary>
		/// <para>Find in any depth an item with this name/caption.</para>
		/// <para>Throws out of range if no item is found.</para>
		/// </summary>
		/// <param name="{std::string}">Item name/caption.</param>
		/// <returns>{Menu_handler} If no exception, the Menu_handler for this name/caption.</returns>
		Menu_handler find(const std::string&);

		/// <summary>
		/// <para>Pop up the Menu on the screen.</para>
		/// <para>You can have ONE DISPLAY per MENU!</para>
		/// <para>The display pointer is stored for easier show/hide (if no parameter, it uses the latest one defined).</para>
		/// </summary>
		/// <param name="{ALLEGRO_DISPLAY*}">If set, show Menu on this display.</param>
		void show(ALLEGRO_DISPLAY* = nullptr);

		/// <summary>
		/// <para>Remove Menu from screen.</para>
		/// <para>You can have ONE DISPLAY per MENU!</para>
		/// <para>The display pointer is stored for easier show/hide (if no parameter, it uses the latest one defined).</para>
		/// </summary>
		/// <param name="{ALLEGRO_DISPLAY*}">If set, hide Menu of this display.</param>
		void hide(ALLEGRO_DISPLAY* = nullptr);

		/// <summary>
		/// <para>Insert a new sub-Menu to the bottom of the list.</para>
		/// </summary>
		/// <param name="{Menu_quick}">A Menu_quick item.</param>
		/// <returns>{int} The index given to this new item.</returns>
		int push(const Menu_quick&);

		/// <summary>
		/// <para>Insert many new sub-menus in order to the bottom of the list.</para>
		/// </summary>
		/// <param name="{vector}">A vector of Menu_quick items.</param>
		void push(const std::vector<Menu_quick>&);

		/// <summary>
		/// <para>Operator used for events.</para>
		/// </summary>
		operator ALLEGRO_EVENT_SOURCE* () const;

		/// <summary>
		/// <para>Get the Menu event source.</para>
		/// </summary>
		/// <returns>{ALLEGRO_EVENT_SOURCE*} Menu event source.</returns>
		ALLEGRO_EVENT_SOURCE* get_event_source() const;

		/// <summary>
		/// <para>Get the Menu itself.</para>
		/// <para>WARN: THIS SHOULD NOT BE CHANGED DIRECTLY!</para>
		/// </summary>
		/// <returns>{ALLEGRO_MENU*} Menu itself.</returns>
		operator ALLEGRO_MENU* () const;

		/// <summary>
		/// <para>Put menu into display</para>
		/// </summary>
		/// <param name="{ALLEGRO_DISPLAY*}">A display to show the menu on.</param>
		void operator>>(ALLEGRO_DISPLAY*);
	};

	/// <summary>
	/// <para>Menu_event is the Menu event, but way easier than that ALLEGRO_EVENT thing.</para>
	/// <para>This has everything you need to know and probably all the tools you've ever need.</para>
	/// </summary>
	class Menu_event {
		ALLEGRO_MENU* source = nullptr;
		std::unique_ptr<Menu_handler> handl;
		Menu& _ref;
	public:
		Menu_event(const Menu_event&) = delete;
		Menu_event(Menu_event&&) = delete;
		void operator=(const Menu_event&) = delete;
		void operator=(Menu_event&&) = delete;

		/// <summary>
		/// <para>Build from event and internal map.</para>
		/// </summary>
		/// <param name="{ALLEGRO_EVENT}">The raw event.</param>
		Menu_event(const ALLEGRO_EVENT&);

		/// <summary>
		/// <para>Directly interface with the Menu that triggered this.</para>
		/// </summary>
		/// <returns>{Menu*} The Menu from where the event came from.</returns>
		Menu* operator->();

		/// <summary>
		/// <para>Directly interface with the Menu that triggered this.</para>
		/// </summary>
		/// <returns>{Menu*} The Menu from where the event came from.</returns>
		Menu* operator->() const;

		/// <summary>
		/// <para>Is this a valid event? Does it have the information you need?</para>
		/// </summary>
		/// <returns>{bool} True if good to go.</returns>
		bool valid() const;

		/// <summary>
		/// <para>Is this NOT valid?</para>
		/// </summary>
		/// <returns>{bool} Empty means invalid.</returns>
		bool empty() const;

		/// <summary>
		/// <para>Get the name/caption of the triggered item.</para>
		/// </summary>
		/// <returns>{std::string} Its name/caption.</returns>
		std::string get_name() const;

		/// <summary>
		/// <para>Get the name/caption of the triggered item.</para>
		/// </summary>
		/// <returns>{std::string} Its name/caption.</returns>
		std::string get_caption() const;

		/// <summary>
		/// <para>Get the ID of the triggered item.</para>
		/// </summary>
		/// <returns>{uint16_t} The ID of this thing.</returns>
		uint16_t get_id() const;

		/// <summary>
		/// <para>Get the flags of the triggered item.</para>
		/// </summary>
		/// <returns>{menu_flags} Combo of flags.</returns>
		menu_flags get_flags() const;

		/// <summary>
		/// <para>Direct check for flag.</para>
		/// </summary>
		/// <param name="{menu_flags}">Flag(s) to check.</param>
		/// <returns>{bool} True if flag(s) is/are set.</returns>
		bool has_flag(const menu_flags) const;

		/// <summary>
		/// <para>Is this a checkbox type?</para>
		/// </summary>
		/// <returns>{bool} True if yes.</returns>
		bool is_checkbox() const;

		/// <summary>
		/// <para>Is this a checkbox type and is it checked?</para>
		/// </summary>
		/// <returns>{bool} True if yes.</returns>
		bool is_checked() const;

		/// <summary>
		/// <para>Is this enabled (selectable)?</para>
		/// </summary>
		/// <returns>{bool} True if yes.</returns>
		bool is_enabled() const;

		/// <summary>
		/// <para>Is this disabled (unselectable)?</para>
		/// </summary>
		/// <returns>{bool} True if yes.</returns>
		bool is_disabled() const;

		/// <summary>
		/// <para>Rename the item to a new name/caption.</para>
		/// </summary>
		/// <param name="{std::string}">New name/caption.</param>
		void patch_name(const std::string&);

		/// <summary>
		/// <para>Rename the item to a new name/caption.</para>
		/// </summary>
		/// <param name="{std::string}">New name/caption.</param>
		void patch_caption(const std::string&);

		/// <summary>
		/// <para>Set the flags of this item.</para>
		/// </summary>
		/// <param name="{menu_flags}">The new flags.</param>
		void patch_flags(const menu_flags);

		/// <summary>
		/// <para>Toggle a flag or flags directly.</para>
		/// </summary>
		/// <param name="{menu_flags}">The flag or flags to toggle.</param>
		void patch_toggle_flag(const menu_flags);

		/// <summary>
		/// <para>Get the Menu that has this item.</para>
		/// </summary>
		/// <returns>{Menu} The source Menu.</returns>
		Menu& get_source();
	};
}