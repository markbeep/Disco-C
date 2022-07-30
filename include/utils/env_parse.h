#ifndef DISCO_ENV
#define DISCO_ENV

/**
 * @brief Returns the value of a .env file variable.
 *
 * @param file Filename
 * @param name Name of the variable. Can't be over 1000 characters long.
 * @return char*
 */
char *d_getenv(const char *file, const char *name);

#endif
