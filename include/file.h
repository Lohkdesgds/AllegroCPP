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
#define SocketWOULDBLOCK EWOULDBLOCK
#define SocketNETRESET ENETRESET
#define SocketCONNRESET ECONNRESET
#define SocketPOLLIN POLLIN
#endif

#endif // ALLEGROCPP_DISABLE_FILESOCKET

#include <allegro5/allegro.h>
#include <allegro5/allegro_memfile.h>

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>

namespace AllegroCPP {

	struct file_error_report {
		std::string msg;
		int err;
	};

	class File {
	protected:
		ALLEGRO_FILE* m_fp = nullptr;
		std::string m_curr_path;
		File() = default;
	public:
		File(const std::string& path, const std::string& mode = "rwb");
		File(const std::string& path, const ALLEGRO_FILE_INTERFACE* interfac, const std::string& mode = "rwb");
		File(int fd, const std::string& mode = "wb+");
		File(File& fp, size_t slice_initial_size, const std::string& mode);
		virtual ~File();

		bool empty() const;
		bool valid() const;
		operator bool() const;
		operator ALLEGRO_FILE* ();

		File(const File&) = delete;
		File(File&&) noexcept;
		void operator=(const File&) = delete;
		void operator=(File&&) noexcept;

		size_t read(void*, size_t);
		size_t write(const void*, size_t);
		bool flush();
		int64_t tell() const;
		bool seek(int64_t offset, int whence);
		bool eof() const;
		bool has_error() const;
		file_error_report get_error() const;
		int ungetc(int);
		int64_t size() const;
		int getc();
		int putc(int);
		int printformat(const char* format, ...);
		int vprintformat(const char* format, va_list args);

		int16_t read16le();
		int16_t read16be();
		size_t write16le(int16_t);
		size_t write16be(int16_t);
		int32_t read32le();
		int32_t read32be();
		size_t write32le(int32_t);
		size_t write32be(int32_t);

		std::string gets(size_t);
		char* gets(char* const buf, size_t max);
		std::shared_ptr<ALLEGRO_USTR> get_ustr(); // autofree
		bool puts(char const* str);
		bool puts(const std::string&);

		const std::string& get_filepath() const;

		// Reset internal ALLEGRO_FILE*. Valid only if not a memory file (else throw)
		ALLEGRO_FILE* drop();
	};

	class FileTmp : public File {
		using File::drop; // remove access
	public:
		FileTmp(const std::string& tmppath = "XXXXXXXX.tmp");
		~FileTmp();

		FileTmp(const FileTmp&) = delete;
		FileTmp(FileTmp&&) noexcept;
		void operator=(const FileTmp&) = delete;
		void operator=(FileTmp&&) noexcept;
	};

	class FileMem : public File {
		char* m_mem = nullptr;

		using File::drop; // remove access
	public:
		FileMem(size_t memlen);
		~FileMem();

		FileMem(const FileMem&) = delete;
		FileMem(FileMem&&) noexcept;
		void operator=(const FileMem&) = delete;
		void operator=(FileMem&&) noexcept;
	};

#ifndef ALLEGROCPP_DISABLE_FILESOCKET
	namespace AllegroCPP_socketmap {

		enum class socket_errors : int32_t {
			SOCKET_INVALID				= 1 << 0,
			RECV_FAILED					= 1 << 1,
			SEND_FAILED					= 1 << 2,
			GETADDR_FAILED				= 1 << 3,
			ADDR_CANT_FIND				= 1 << 4,
			MODE_WAS_INVALID			= 1 << 5
		};

		struct socket_user_data {
			std::vector<SocketType> m_socks;
			int32_t badflag = 0;
			int lastprot = 0;
			bool is_host = false;
			bool is_udp_shared = false;
		};

		struct socket_config {
			socket_user_data* prealloc = nullptr; // if set, that's it.
			std::string addr; // can be null for host or localhost
			int protocol;
			int family;
			uint16_t port = 0;
			bool host;
		};

		// socket_config* and &(sizeof(socket_config)) (as uint64_t)
		void* sock_open(const char* addr, const char* intptr);
		bool sock_close(ALLEGRO_FILE* fp);
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

		SocketType sock_listen(std::vector<SocketType>& servers, const long timeout);
		
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
	}

	class FileSocket : protected File {
#ifdef _WIN32 
		struct _winsock_start { WSADATA wsaData = WSADATA(); _winsock_start(); ~_winsock_start(); };
		static _winsock_start __winsock;
#endif
		// used in host type, when host listen() or something and them boom, child.
		FileSocket(AllegroCPP_socketmap::socket_user_data* absorb);

		bool _listen(long timeout, SocketType* sok = nullptr, AllegroCPP_socketmap::socket_user_data** = nullptr) const;
	public:
		// client
		FileSocket(const std::string& addr, uint16_t port, int protocol = SOCK_STREAM, int family = PF_UNSPEC);
		// host
		FileSocket(uint16_t port, int protocol = SOCK_STREAM, int family = PF_UNSPEC);
		// ~FileSocket: destructor is the same.

		FileSocket(FileSocket&&) = default;

		FileSocket(const FileSocket&) = delete;
		void operator=(const FileSocket&) = delete;

		// only hosts are combinable (move managed sockets from that to this)
		bool combine(FileSocket&&);

		bool has_listen() const;
		FileSocket listen(long timeout) const;

		using File::operator=;

		using File::empty;
		using File::valid;
		using File::operator bool;
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

		using File::gets;
		using File::get_ustr;
		using File::puts;
	};

#endif // ALLEGROCPP_DISABLE_FILESOCKET
}