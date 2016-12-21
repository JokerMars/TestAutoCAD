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
		/*status = FltGetStreamContext(FltObjects->Instance, FltObjects->FileObject, &pStreamCtx);
		if (!NT_SUCCESS(status) || pStreamCtx == NULL)
		{
			leave;
		}*/

		//
		// test if there is some other process write dwg
		//

		BOOLEAN isDir;
		status = FltIsDirectory(FltObjects->FileObject, FltObjects->Instance, &isDir);
		if (!NT_SUCCESS(status) || isDir)
		{
			leave;
		}

		status = FltGetFileNameInformation(Data,
			FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
			&pfNameInfo);
		if (!NT_SUCCESS(status) || pfNameInfo == NULL)
		{
			leave;
		}
		FltParseFileNameInformation(pfNameInfo);

		UNICODE_STRING ext = { 0 };
		RtlInitUnicodeString(&ext, MONITOR_EXT);
		if (RtlCompareUnicodeString(&ext, &(pfNameInfo->Extension), TRUE) != 0)
		{
			leave;
		}


		PCHAR procName = GetProcessName();
		if (procName == NULL)
		{
			leave;
		}



	/*	if (strncmp(procName, MONITOR_PROCESS, strlen(MONITOR_PROCESS)) != 0)
		{
			leave;
		}*/

		DbgPrint("\nIO_PRE_WRITE\n");
		//DbgPrint("    File Name: %wZ\n", &(pStreamCtx->fileName));
		DbgPrint("    File Name: %wZ\n", &(pfNameInfo->Name));
		DbgPrint("    Process Name: %s\n", procName);
		DbgPrint("    PreWrite Len: %d\n", iopb->Parameters.Write.Length);

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
