
namespace driver
{
	namespace utils
	{
		NTSTATUS process_by_name( CHAR* process_name, PEPROCESS* process );
		int GetProcessInfo( HANDLE* processId );
		void sleep(int ms) { LARGE_INTEGER time;  time.QuadPart =- (ms) * 10 * 1000; KeDelayExecutionThread(KernelMode, TRUE, &time); }
	}
}