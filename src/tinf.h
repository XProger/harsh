/*
	based on tinf - tiny inflate library
	http://www.ibsensoftware.com/
*/

#ifndef TINF_H_INCLUDED
#define TINF_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define TINF_OK             0
#define TINF_DATA_ERROR    (-3)

/* function prototypes */
void tinf_init(void);
int tinf_uncompress(char *dest, int *destLen, const char *source);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TINF_H_INCLUDED */
