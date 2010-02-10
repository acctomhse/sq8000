/* $Id: strcspn.c,v 1.3 1999/03/08 07:34:26 gernot Exp $ */

int strcspn (char *p, char *s)
{
    int             i, j;

    for (i = 0; p[i]; i++) {
	for (j = 0; s[j]; j++) {
	    if (s[j] == p[i])
		break;
	}
	if (s[j])
	    break;
    }
    return (i);
}
