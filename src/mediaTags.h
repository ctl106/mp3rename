/** @file mediaTags.h
 * @author Champagne Lewis	<champagne.t.lewis@gmail.com>
 * @brief Contains mediaTags struct and related defines.
 *
 */


#ifndef MEDIATAGS_H_INCLUDED
#define MEDIATAGS_H_INCLUDED


#include <stdio.h>

// should be set to contain the largest field of any tag type +1 to null terminate
#define MEDIATAGS_TITLE_MAX_LENGTH		61
#define MEDIATAGS_ARTIST_MAX_LENGTH		61
#define MEDIATAGS_ALBUM_MAX_LENGTH		61
#define MEDIATAGS_YEAR_MAX_LENGTH		5
#define MEDIATAGS_COMMENT_MAX_LENGTH	44
#define MEDIATAGS_TRACK_MAX_LENGTH		3
#define MEDIATAGS_GENRE_MAX_LENGTH		31
#define MEDIATAGS_SUBGENRE_MAX_LENGTH	21

/** @brief Struct for containing tag metadata from a media file
 */
typedef struct MediaTags {
	char title[MEDIATAGS_TITLE_MAX_LENGTH];
	char artist[MEDIATAGS_ARTIST_MAX_LENGTH];
	char album[MEDIATAGS_ALBUM_MAX_LENGTH];
	char year[MEDIATAGS_YEAR_MAX_LENGTH];
	char comment[MEDIATAGS_COMMENT_MAX_LENGTH];
	char track[MEDIATAGS_TRACK_MAX_LENGTH];
	char genre[MEDIATAGS_GENRE_MAX_LENGTH];
	char subgenre[MEDIATAGS_SUBGENRE_MAX_LENGTH];

	/**< The size of the tag when applied to a media file. */
	size_t tagSize;

	/**< Callback function for applying the tag to media. */
	int (*applyTag)(FILE *media, struct MediaTags mediaTags);
} MediaTags;

#endif // MEDIATAGS_H_INCLUDED
