/** @file id3v1lib.h
 * @author Champagne Lewis	<champagne.t.lewis@gmail.com>
 * @brief Library for reading and writing ID3 V1 tags from media files.
 *
 */


#ifndef ID3V1LIB_H_INCLUDED
#define ID3V1LIB_H_INCLUDED


#include <stdbool.h>
#include <stdio.h>

#include "mediaTags.h"


#define ID3_V1_TITLE_SIZE	30
#define ID3_V1_ARTIST_SIZE	30
#define ID3_V1_ALBUM_SIZE	30
#define ID3_V1_YEAR_SIZE	4
#define ID3_V1_COMMENT_SIZE	30
#define ID3_V1_GENRE_SIZE	1


/** @brief Checks if the provided char array contains an ID3 V1 tag.
 *
 * @param media The file to search for the tag in
 * @return true if an ID3 V1 header was found, false otherwise
 *
 */
bool hasId3V1(FILE *media);

/** @brief Populates a MediaTags struct from the contents of a media file.
 *
 * @param mediaTags Pointer to the MediaTags struct to populate
 * @param media The file to pull the tag data from
 * @return Zero if completed successfully, else non-zero
 *
 */
int convertId3V1ToMediaTags(MediaTags *mediaTags, FILE *media);

/** @brief Inserts data from a MediaTags struct into the provided media file.
 *
 * @param media Stream of the file to update
 * @param mediaTags MediaTags struct to read tag data from
 * @return Zero if completed successfully, non-zero otherwise.
 *
 */
int setId3V1TagsInFile(FILE *media, MediaTags mediaTags);


#endif // ID3V1LIB_H_INCLUDED
