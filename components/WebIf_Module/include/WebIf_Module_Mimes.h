#ifndef SCDE_Mimes_H
#define SCDE_Mimes_H

// List of possible file extensions + mappings to mime types
typedef struct {
	const char *ext;
	const char *mimetype;
} ContentTypes;

// make our list of available mimes available
extern ContentTypes AvailMimes[];


#endif /* SCDE_Mimes_H */
