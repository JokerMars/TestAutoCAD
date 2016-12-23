#include "CallbackRoutines.h"

static int cnt = 0;

FLT_PREOP_CALLBACK_STATUS
PreCleanup(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	NTSTATUS status;
	PSTREAM_CONTEXT pStreamCtx = NULL;
	KIRQL oldIrql;
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;

	try
	{
		//
		// get the current stream context
		//

		status = FltGetStreamContext(iopb->TargetInstance, iopb->TargetFileObject, &pStreamCtx);
		if (!NT_SUCCESS(status) || pStreamCtx == NULL)
		{
			leave;
		}
		
		//
		// Get Process Name
		//

		PCHAR procName = GetProcessName();
		if (procName == NULL)
		{
			leave;
		}
		if (strncmp(procName, MONITOR_PROCESS, strlen(MONITOR_PROCESS)) != 0)
		{
			leave;
		}

		DbgPrint("\nIO_PRE_CLEANUP\n");
		DbgPrint("    Process Name: %s\n", procName);
		DbgPrint("    File Name: %wZ\n", &(pStreamCtx->fileName));

		

	}
	finally
	{
		if (pStreamCtx != NULL)
		{
			FltReleaseContext(pStreamCtx);
		}
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}
