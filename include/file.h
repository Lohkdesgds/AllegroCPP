#pragma once

// remember font.h has to be able to eat the ALLEGRO_FILE to itself.

#ifndef ALLEGROCPP_DISABLE_FILESOCKET

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
using SocketType = SOCKET;
using SocketStorage = SOCKADDR_STORAGE;
using SocketAddrInfo = ADDRINFO;
using SocketSockAddrPtr = LPSOCKADDR;
using SocketPollFD = WSAPOLLFD;
constexpr SocketType SocketInvalid = INVALID_SOCKET;
constexpr int SocketError = SOCKET_ERROR;
constexpr int SocketTimeout = 0;
#define closeSocket(...) ::closesocket(__VA_ARGS__)
#define ioctlSocket(...) ::ioctlsocket(__VA_ARGS__)
#define pollSocket(...) ::WSAPoll(__VA_ARGS__)
#define theSocketError WSAGetLastError()
#define SocketBUFFERSMALL WSAEMSGSIZE
#define SocketWOULDBLOCK WSAEWOULDBLOCK
#define SocketNETRESET WSAENETRESET
#define SocketCONNRESET WSAECONNRESET
#define SocketPOLLIN POLLRDNORM
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
using SocketType = int;
using SocketStorage = sockaddr_storage;
using SocketAddrInfo = addrinfo;
using SocketSockAddrPtr = sockaddr*;
using SocketPollFD = pollfd;
constexpr SocketType SocketInvalid = -1;
constexpr int SocketError = -1;
constexpr int SocketTimeout = 0;
#define closeSocket(...) ::close(__VA_ARGS__)
#define ioctlSocket(...) ::ioctl(__VA_ARGS__)
#define pollSocket(...) ::poll(__VA_ARGS__)
#define theSocketError errno
#define SocketBUFFERSMALL EMSGSIZE
#define SocketWOULDBLOCK EWOULDBLOCK
#define SocketNETRESET ENETRESET
#define SocketCONNRESET ECONNRESET
#define SocketPOLLIN POLLIN
#endif
#define SocketGood(X) [&]{ const auto xg = (X); return (xg != SocketInvalid && xg != SocketError);}()

#endif // ALLEGROCPP_DISABLE_FILESOCKET

#include <allegro5/allegro.h>
#include <allegro5/allegro_memfile.h>

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>

namespace AllegroCPP {

	struct file_error_report {
		std::string msg;
		int err;
	};

	using File_shareable_ptr = std::shared_ptr<std::unique_ptr<ALLEGRO_FILE,std::function<void(ALLEGRO_FILE*)>>>;

	class File {
	protected:
		//ALLEGRO_FILE* m_fp = nullptr;
		File_shareable_ptr m_fp;
		std::string m_curr_path;
		File() = default;
	public:
		File(const std::string& path, const std::string& mode = "wb+");
		File(const std::string& path, const ALLEGRO_FILE_INTERFACE* interfac, const std::string& mode = "wb+");
		File(const int fd, const std::string& mode = "wb+");
		File(File& fp, size_t slice_initial_size, const std::string& mode);
		virtual ~File();

		virtual bool empty() const;
		virtual bool valid() const;
		virtual operator bool() const;
		virtual operator ALLEGRO_FILE* ();
		virtual operator File_shareable_ptr() const;

		File(const File&) = delete;
		File(File&&) noexcept;
		void operator=(const File&) = delete;
		virtual void operator=(File&&) noexcept;

		virtual size_t read(void*, const size_t);
		virtual size_t write(const void*, const size_t);
		virtual bool flush();
		virtual int64_t tell() const;
		virtual bool seek(const int64_t offset, const int whence);
		virtual bool eof() const;
		virtual bool has_error() const;
		virtual file_error_report get_error() const;
		virtual int ungetc(const int);
		virtual int64_t size() const;
		virtual int getc();
		virtual int putc(const int);
		virtual int printformat(const char* format, ...);
		virtual int vprintformat(const char* format, va_list args);

		virtual int16_t read16le();
		virtual int16_t read16be();
		virtual size_t write16le(int16_t);
		virtual size_t write16be(int16_t);
		virtual int32_t read32le();
		virtual int32_t read32be();
		virtual size_t write32le(int32_t);
		virtual size_t write32be(int32_t);

		virtual File& operator<<(const char* val);
		virtual File& operator<<(const std::string& val);
		virtual File& operator<<(const std::vector<char>& val);
		virtual File& operator<<(bool val);
		virtual File& operator<<(short val);
		virtual File& operator<<(unsigned short val);
		virtual File& operator<<(int val);
		virtual File& operator<<(unsigned int val);
		virtual File& operator<<(long val);
		virtual File& operator<<(unsigned long val);
		virtual File& operator<<(long long val);
		virtual File& operator<<(unsigned long long val);
		virtual File& operator<<(float val);
		virtual File& operator<<(double val);
		virtual File& operator<<(long double val);
		virtual File& operator<<(std::streambuf* sb);
		virtual File& operator<<(std::ostream& (*pf)(std::ostream&));
		virtual File& operator<<(std::ios& (*pf)(std::ios&));
		virtual File& operator<<(std::ios_base& (*pf)(std::ios_base&));

		virtual File& operator>>(std::string& val);
		virtual File& operator>>(std::vector<char>& val);
		virtual File& operator>>(bool& val);
		virtual File& operator>>(short& val);
		virtual File& operator>>(unsigned short& val);
		virtual File& operator>>(int& val);
		virtual File& operator>>(unsigned int& val);
		virtual File& operator>>(long& val);
		virtual File& operator>>(unsigned long& val);
		virtual File& operator>>(long long& val);
		virtual File& operator>>(unsigned long long& val);
		virtual File& operator>>(float& val);
		virtual File& operator>>(double& val);
		virtual File& operator>>(long double& val);

		virtual std::string gets(const size_t);
		virtual char* gets(char* const buf, const size_t max);
		// https://github.com/liballeg/allegro5/blob/edcc50e85036e95474a31a7c75089b6b7653a4e5/src/file.c#L457
		virtual std::shared_ptr<ALLEGRO_USTR> get_ustr(); // autofree
		virtual bool puts(char const* str);
		virtual bool puts(const std::string&);

		virtual const std::string& get_filepath() const;

		// Reset internal ALLEGRO_FILE*. Valid only if not a memory file (else throw)
		//ALLEGRO_FILE* drop();

		File clone_for_read() const;

		void close();
	};

	class File_tmp : public File {
		//using File::drop; // remove access
	public:
		File_tmp(const std::string& tmppath = "XXXXXXXX.tmp", const std::string& mode = "wb+");
		~File_tmp();

		File_tmp(const File_tmp&) = delete;
		File_tmp(File_tmp&&) noexcept;
		void operator=(const File_tmp&) = delete;
		void operator=(File_tmp&&) noexcept;
	};

	class File_memory : public File {
		char* m_mem = nullptr;

		//using File::drop; // remove access
	public:
		File_memory(size_t memlen);
		~File_memory();

		File_memory(const File_memory&) = delete;
		File_memory(File_memory&&) noexcept;
		void operator=(const File_memory&) = delete;
		void operator=(File_memory&&) noexcept;
	};

#ifndef ALLEGROCPP_DISABLE_FILESOCKET
	namespace _socketmap {

		class non_implemented : public std::exception {
			std::string m_msg;
		public:
			non_implemented(const char*);
			non_implemented(const std::string&);
			
			virtual const char* what() const;
		};

		enum class socket_errors : int32_t {
			SOCKET_INVALID				= 1 << 0,
			RECV_FAILED					= 1 << 1,
			SEND_FAILED					= 1 << 2,
			GETADDR_FAILED				= 1 << 3,
			ADDR_CANT_FIND				= 1 << 4,
			MODE_WAS_INVALID			= 1 << 5,
			HOST_PTR_RECV_FAIL			= 1 << 6,
			CLOSED						= 1 << 7
		};
		enum class socket_type : uint8_t {
			INVALID,
			TCP_HOST,
			TCP_CLIENT,
			UDP_HOST,
			UDP_HOST_CLIENT,
			UDP_CLIENT
		};

		struct socket_user_data {
			struct _eachsock {
				SocketType sock = SocketInvalid;
				SocketAddrInfo info{};
				socket_type type = socket_type::INVALID; // UDP_HOST_CLIENT is the only one that DOES NOT CLOSE SOCKET!
				std::string src_ip;
				_eachsock(SocketType, SocketAddrInfo, socket_type, std::string);
			};
			std::vector<_eachsock> m_socks; // SocketAddrInfo for UDP has last recv all the time.
			int32_t badflag = 0;
			std::string original_addr;
			uint16_t original_port;
			bool has_host() const;
			void close_auto();
		};

		struct new_socket_user_data {
			socket_user_data* ptr = nullptr;
			long timeout = 0; // used on hosts only
		};

		struct socket_config {
			socket_user_data* prealloc = nullptr; // if set, that's it.
			std::string addr; // can be null for host or localhost
			int protocol;
			int family;
			uint16_t port = 0;
			bool host;
			bool opt_broadcast;
		};

		// socket_config* and &(sizeof(socket_config)) (as uint64_t)
		void* sock_open(const char* addr, const char* intptr);
		bool sock_close(ALLEGRO_FILE* fp);
		// tcp client and udp client return like recv. host expects void* to be socket_user_data and size == sizeof that (gets final value there)
		size_t sock_read(ALLEGRO_FILE* fp, void* ptr, size_t size);
		size_t sock_write(ALLEGRO_FILE* fp, const void* ptr, size_t size);
		bool sock_flush(ALLEGRO_FILE* fp);
		int64_t sock_tell(ALLEGRO_FILE* fp);
		bool sock_seek(ALLEGRO_FILE* fp, int64_t offset, int whence);
		bool sock_eof(ALLEGRO_FILE* fp);
		int sock_error(ALLEGRO_FILE* fp);
		const char* sock_errmsg(ALLEGRO_FILE* fp);
		void sock_clearerr(ALLEGRO_FILE* fp);
		int sock_ungetc(ALLEGRO_FILE* fp, int c);
		off_t sock_size(ALLEGRO_FILE*);

		std::vector<socket_user_data::_eachsock>::const_iterator sock_listen(const std::vector<socket_user_data::_eachsock>& servers, const long timeout);
		SocketType sock_listen(const std::vector<SocketType>& servers, const long timeout);
		void setsocktimeout_auto(SocketType, unsigned long ms);
		
		static ALLEGRO_FILE_INTERFACE socket_interface =
		{
		   sock_open,
		   sock_close,
		   sock_read,
		   sock_write,
		   sock_flush,
		   sock_tell,
		   sock_seek,
		   sock_eof,
		   sock_error,
		   sock_errmsg,
		   sock_clearerr,
		   sock_ungetc,
		   sock_size
		};

		class _FileSocket : public File {
#ifdef _WIN32 
			struct _winsock_start { WSADATA wsaData = WSADATA(); _winsock_start(); ~_winsock_start(); };
			static _winsock_start __winsock;
#endif
		protected:
			// used in host type, when host listen() or something and them boom, child.
			void set(_socketmap::socket_user_data* absorb);

			_FileSocket();
		public:
			_FileSocket(_FileSocket&&) noexcept;
			void operator=(_FileSocket&&) noexcept;
			_FileSocket(const _FileSocket&) = delete;
			void operator=(const _FileSocket&) = delete;

			bool empty() const;
			bool valid() const;
			operator bool() const;

			const std::string& get_filepath() const;
			const std::string& get_ipaddr_of(size_t = 0) const;

			using File::operator=;

			using File::operator ALLEGRO_FILE*;

			using File::read;
			using File::write;
			using File::eof;
			using File::has_error;
			using File::get_error;
			using File::getc;
			using File::putc;
			using File::printformat;
			using File::vprintformat;
			using File::read16le;
			using File::read16be;
			using File::write16le;
			using File::write16be;
			using File::read32le;
			using File::read32be;
			using File::write32le;
			using File::write32be;

			using File::operator<<;
			using File::operator>>;

			std::string gets(const size_t);
			char* gets(char* const buf, const size_t max);
			std::shared_ptr<ALLEGRO_USTR> get_ustr();

			bool puts(char const* str);
			bool puts(const std::string&);

			// udp only
			void set_broadcast(const bool);
		};
	}

	class File_host;

	enum class file_protocol{TCP = SOCK_STREAM, UDP = SOCK_DGRAM};
	enum class file_family{ANY = PF_UNSPEC, IPV4 = PF_INET, IPV6 = PF_INET6};

	class File_client : public _socketmap::_FileSocket {
		friend class File_host; // so host can gen File_client

		File_client(_socketmap::socket_user_data* absorb);
	public:
		File_client(const std::string& addr, const uint16_t port, const int protocol, const int family = PF_UNSPEC, const bool broadcast = false);
		File_client(const std::string& addr, const uint16_t port, const file_protocol protocol = file_protocol::TCP, const file_family family = file_family::ANY, const bool broadcast = false);

		File_client(const File_client&) = delete;
		File_client(File_client&&) noexcept;
		void operator=(const File_client&) = delete;
		void operator=(File_client&&) noexcept;

		bool set_timeout_read(const unsigned long ms);
	};

	class File_host : public _socketmap::_FileSocket {
		using _FileSocket::write; // hide
		using _FileSocket::read; // hide
	public:
		File_host(const uint16_t port, const int protocol, const int family = PF_UNSPEC);
		File_host(const uint16_t port, const file_protocol protocol = file_protocol::TCP, const file_family family = file_family::ANY);

		File_host(const File_host&) = delete;
		File_host(File_host&&) noexcept;
		void operator=(const File_host&) = delete;
		void operator=(File_host&&) noexcept;

		bool combine(File_host&&);

		File_client listen(const long timeout = 500);
	};

#endif // ALLEGROCPP_DISABLE_FILESOCKET

#ifdef _WIN32
	File_memory file_load_resource_name_in_memory(int defined_name, const char* type_name);
	File_tmp file_load_resource_name_to_temp_file(int defined_name, const char* type_name, const std::string& file_extension_including_dot = ".tmp");
	File file_load_resource_name_to_file(const char* file_name, int defined_name, const char* type_name);
#endif
}