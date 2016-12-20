#include "File.h"

NTSTATUS
File_GetFileSize(
	__in  PFLT_CALLBACK_DATA Data,
	__in  PFLT_RELATED_OBJECTS FltObjects,
	__in PLARGE_INTEGER FileSize
)
{
	NTSTATUS status;
	FILE_STANDARD_INFORMATION fileInfo;

	status = FltQueryInformationFile(FltObjects->Instance,
		FltObjects->FileObject,
		&fileInfo,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation,
		NULL
	);
	if (NT_SUCCESS(status))
	{
		FileSize->QuadPart = fileInfo.EndOfFile.QuadPart;
	}
	else
	{
		FileSize->QuadPart = 0;
	}

	return status;
}