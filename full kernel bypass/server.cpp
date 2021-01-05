#include <ntifs.h>
#include <ntimage.h>
#include <ntddk.h>
#include <wdf.h>
#include "memory/memory.h"
#include "server_shared.h"
#include "sockets.h"
#include "defs.h"
#include "log.hpp"
#include "utils/utils.h"

using namespace driver;


extern "C"
NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS		SourceProcess,
	PVOID			SourceAddress,
	PEPROCESS		TargetProcess,
	PVOID			TargetAddress,
	SIZE_T			BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T			ReturnSize
);

// 枚举
// 读取内存，并获取内容
static uint64_t handle_copy_memory(const PacketCopyMemory& packet)
{
	// 目标进程
	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.src_process_id), &process::process)))
	{
		ObDereferenceObject(process::process);
		return uint64_t(STATUS_INVALID_CID);
	}

	// MID 中间件 进程
	PEPROCESS dest_process = nullptr;
	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.dest_process_id), &dest_process)))
	{
		return uint64_t(STATUS_INVALID_CID);
	}

	//uint64_t value = memory::read<uint64_t>(packet.src_address);

	//NTSTATUS status = memory::read_virtual_memory(packet.src_process_id, process::process, (void*)packet.src_address, (void*)packet.dest_address, packet.size);

	SIZE_T   return_size = 0;
	NTSTATUS status = MmCopyVirtualMemory(
		process::process,
		(void*)packet.src_address,
		dest_process,
		(void*)packet.dest_address,
		packet.size,
		UserMode,
		&return_size
	);

	ObDereferenceObject(process::process);
	ObDereferenceObject(dest_process);

	return uint64_t(status);
}

// 获取基址
static uint64_t handle_get_base_address(const PacketGetBaseAddress& packet)
{


	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.process_id), &process::process)))
	{
		ObDereferenceObject(process::process);
		return uint64_t(STATUS_INVALID_CID);
	}
	const auto base_address = uint64_t(PsGetProcessSectionBaseAddress(process::process));
	ObDereferenceObject(process::process);

	return base_address;
}
// 枚举结束


uint64_t handle_incoming_packet(const Packet& packet)
{
	switch (packet.header.type)
	{
	case PacketType::packet_copy_memory:
		return handle_copy_memory(packet.data.copy_memory);

	case PacketType::packet_get_base_address:
		return handle_get_base_address(packet.data.get_base_address);

	default:
		break;
	}

	return uint64_t(STATUS_NOT_IMPLEMENTED);
}


// Send completion packet.
bool complete_request(const SOCKET client_connection, const uint64_t result)
{
	Packet packet{ };

	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_completed;
	packet.data.completed.result = result;

	return send(client_connection, &packet, sizeof(packet), 0) != SOCKET_ERROR;
}

static SOCKET create_listen_socket()
{
	SOCKADDR_IN address{ };

	address.sin_family = AF_INET;
	address.sin_port = htons(server_port);

	const auto listen_socket = socket_listen(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		log("Failed to create listen socket.");
		return INVALID_SOCKET;
	}

	if (bind(listen_socket, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		log("Failed to bind socket.");

		closesocket(listen_socket);
		return INVALID_SOCKET;
	}

	if (listen(listen_socket, 10) == SOCKET_ERROR)
	{
		log("Failed to set socket mode to listening.");

		closesocket(listen_socket);
		return INVALID_SOCKET;
	}

	return listen_socket;
}


// Connection handling thread.
static void NTAPI connection_thread(void* connection_socket)
{
	const auto client_connection = SOCKET(ULONG_PTR(connection_socket));
	log("New connection.");

	Packet packet{ };
	while (true)
	{
		const auto result = recv(client_connection, (void*)&packet, sizeof(packet), 0);
		if (result <= 0)
			break;

		if (result < sizeof(PacketHeader))
			continue;

		if (packet.header.magic != packet_magic)
			continue;

		const auto packet_result = handle_incoming_packet(packet);
		if (!complete_request(client_connection, packet_result))
			break;
	}

	log("Connection closed.");
	closesocket(client_connection);
}

void server_thread()
{
	auto status = KsInitialize();
	if (!NT_SUCCESS(status))
	{
		log("Failed to initialize KSOCKET. Status code: %X.", status);
		return;
	}

	const auto listen_socket = create_listen_socket();
	if (listen_socket == INVALID_SOCKET)
	{
		log("Failed to initialize listening socket.");

		KsDestroy();
		return;
	}

	log("Listening on port %d.", server_port);



	while (true)
	{
		sockaddr  socket_addr{ };
		socklen_t socket_length{ };

		const auto client_connection = accept(listen_socket, &socket_addr, &socket_length);
		if (client_connection == INVALID_SOCKET)
		{
			log("Failed to accept client connection.");
			break;
		}

		HANDLE thread_handle = nullptr;

		// Create a thread that will handle connection with client.
		// TODO: Limit number of threads.
		status = PsCreateSystemThread(
			&thread_handle,
			GENERIC_ALL,
			nullptr,
			nullptr,
			nullptr,
			connection_thread,
			(void*)client_connection
		);

		if (!NT_SUCCESS(status))
		{
			log("Failed to create thread for handling client connection.");

			closesocket(client_connection);
			break;
		}

		ZwClose(thread_handle);
	}
	
	// 1 秒后 关闭 SOCKET
	utils::sleep(1000);
	if (NT_SUCCESS(closesocket(listen_socket)))
	{
		log("成功关闭 SOCKET %d.", server_port);
	}
}
