#include "CallbackRoutines.h"

static int cnt = 1;


FLT_PREOP_CALLBACK_STATUS
PreCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObejcts,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObejcts);
	UNREFERENCED_PARAMETER(CompletionContext);

	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
PostCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	BOOLEAN isDir = FALSE;
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION pfNameInfo = NULL;

	try
	{
		//
		// we don't need to monitor the directory
		//

		status = FltIsDirectory(FltObjects->FileObject, FltObjects->Instance, &isDir);
		if (isDir)
		{
			leave;
		}

		//
		// only autocad process can go down
		//

		PCHAR procName = GetProcessName();
		if (strncmp(procName, "acad.exe", strlen("acad.exe")) != 0)
		{
			leave;
		}

		//
		// get the file name we are visiting currently
		//

		status = FltGetFileNameInformation(Data,
			FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
			&pfNameInfo);
		if (!NT_SUCCESS(status) || pfNameInfo == NULL)
		{
			leave;
		}
		FltParseFileNameInformation(pfNameInfo);

		//
		// we filter the dwg file type, don't care other's file type
		//

		UNICODE_STRING ext = { 0 };
		RtlInitUnicodeString(&ext, L"dwg");
		if (RtlCompareUnicodeString(&ext, &(pfNameInfo->Extension), TRUE) != 0)
		{
			leave;
		}

		DbgPrint("Process: %s\n", procName);
		DbgPrint("File Name: %wZ\n", &(pfNameInfo->Name));
		DbgPrint("File Extension: %wZ\n", &(pfNameInfo->Extension));
		DbgPrint("Cnt: %d\n", cnt++);

	}
	finally
	{
		if(pfNameInfo!=NULL)
		{
			FltReleaseFileNameInformation(pfNameInfo);
		}
	}
	

	return FLT_POSTOP_FINISHED_PROCESSING;
}