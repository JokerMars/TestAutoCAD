#include "Common.h"


ULONG g_curProcessNameOffset = 0;
UNICODE_STRING ext = { 0 };

ULONG GetProcessNameOffset()
{
	PEPROCESS curproc = NULL;
	int i = 0;

	curproc = PsGetCurrentProcess();

	for (i = 0; i<3 * PAGE_SIZE; i++)
	{
		if (!strncmp("System", (PCHAR)curproc + i, strlen("System")))
		{
			return i;
		}
	}

	return 0;
}

PCHAR GetProcessName()
{
	PEPROCESS curproc = NULL;
	curproc = PsGetCurrentProcess();

	if (curproc != NULL)
	{
		return (PCHAR)curproc + g_curProcessNameOffset;
	}

	return NULL;
}


BOOLEAN IsFilteredFileProcess(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects,PCHAR IrpType)
{
	BOOLEAN isDir = FALSE;
	NTSTATUS status;
	PFLT_FILE_NAME_INFORMATION pfNameInfo = NULL;

	try {

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

		
		if (RtlCompareUnicodeString(&ext, &(pfNameInfo->Extension), TRUE) != 0)
		{
			leave;
		}

		DbgPrint("\n%s\n", IrpType);
		DbgPrint("\tProcess: %s\n", procName);
		DbgPrint("\tFile Name: %wZ\n", &(pfNameInfo->Name));
		DbgPrint("\tFile Extension: %wZ\n", &(pfNameInfo->Extension));

	

		return TRUE;

	}
	finally
	{
		if (pfNameInfo != NULL)
		{
			FltReleaseFileNameInformation(pfNameInfo);
		}
	}

	return FALSE;
}