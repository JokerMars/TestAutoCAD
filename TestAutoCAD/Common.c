#include "Common.h"


ULONG g_curProcessNameOffset = 0;

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
