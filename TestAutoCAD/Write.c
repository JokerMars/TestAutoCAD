#include "CallbackRoutines.h"


FLT_PREOP_CALLBACK_STATUS
PreWrite(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
	NTSTATUS status;
	PSTREAM_CONTEXT pStreamCtx = NULL;

	PFLT_FILE_NAME_INFORMATION pfNameInfo = NULL;

	try
	{
		status = FltGetStreamContext(iopb->TargetInstance, iopb->TargetFileObject, &pStreamCtx);
		if (!NT_SUCCESS(status) || pStreamCtx == NULL)
		{
			leave;
		}
		

		PCHAR procName = GetProcessName();
		if (procName == NULL)
		{
			leave;
		}
		if (strncmp(procName, MONITOR_PROCESS, strlen(MONITOR_PROCESS)) != 0)
		{
			leave;
		}

		DbgPrint("\nIO_PRE_WRITE\n");
		DbgPrint("    File Name: %wZ\n", &(pStreamCtx->fileName));
		//DbgPrint("    File Name: %wZ\n", &(pfNameInfo->Name));
		DbgPrint("    Process Name: %s\n", procName);
		DbgPrint("    PreWrite Len: %d\n", iopb->Parameters.Write.Length);
		DbgPrint("    PreRead Len: %d\n", iopb->Parameters.Read.Length);

		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}
	finally
	{
		if (pStreamCtx != NULL)
		{
			FltReleaseContext(pStreamCtx);
		}

		if (pfNameInfo != NULL)
		{
			FltReleaseFileNameInformation(pfNameInfo);
		}
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
PostWrite(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	DbgPrint("\nIO_POST_WRITE\n");
	DbgPrint("    PostWrite Len: %d\n", Data->IoStatus.Information);
	return FLT_POSTOP_FINISHED_PROCESSING;
}
