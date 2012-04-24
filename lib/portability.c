/* vi: set sw=4 ts=4 :*/
/* portability.c - code to workaround the deficiencies of various platforms.
 *
 * Copyright 2012 Rob Landley <rob@landley.net>
 * Copyright 2012 Georgi Chorbadzhiyski <gf@unixsol.org>
 */

#include "toys.h"

#if defined(__APPLE__) || defined(__ANDROID__)
ssize_t getdelim(char **linep, size_t *np, int delim, FILE *stream)
{
	int ch;
	size_t new_len;
	ssize_t i = 0;
	char *line, *new_line;

	// Invalid input
	if (!linep || !np) {
		errno = EINVAL;
		return -1;
	}

	if (*linep == NULL || *np == 0) {
		*np = 1024;
		*linep = calloc(1, *np);
		if (*linep == NULL)
			return -1;
	}
	line = *linep;

	while ((ch = getc(stream)) != EOF) {
		if (i > *np) {
			// Need more space
			new_len  = *np + 1024;
			new_line = realloc(*linep, new_len);
			if (!new_line)
				return -1;
			*np    = new_len;
			*linep = new_line;
		}

		line[i] = ch;
		if (ch == delim)
			break;
		i += 1;
	}

	if (i > *np) {
		// Need more space
		new_len  = i + 2;
		new_line = realloc(*linep, new_len);
		if (!new_line)
			return -1;
		*np    = new_len;
		*linep = new_line;
	}
	line[i + 1] = '\0';

	return i > 0 ? i : -1;
}

ssize_t getline(char **linep, size_t *np, FILE *stream) {
	return getdelim(linep, np, '\n', stream);
}
#endif

#if defined(__APPLE__)
extern char **environ;

int clearenv(void) {
	*environ = NULL;
	return 0;
}
#endif

#if defined(__ANDROID__)
int sethostname(const char *name, size_t len) {
	return syscall(__NR_sethostname, name, len);
}

int swapoff(const char *path) {
	return syscall(__NR_swapon, path);
}

int swapon(const char *path, int swapflags) {
	return syscall(__NR_swapon, path, swapflags);
}
#endif
