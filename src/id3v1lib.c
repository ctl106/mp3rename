#include <string.h>
#include <stdlib.h>

#include "id3v1lib.h"


#define ID3_V1_SIZE		128
#define ID3_V1_OFFSET	-128

#define ID3_V1_HEADER_SIZE	3
#define ID3_V1_HEADER		"TAG"
#define ID3_ENHANCED_HEADER	"TAG+"
#define ID3_V1_2_HEADER		"EXT"


static void pad(char *string, int length)
{
	int i;

	i=strlen(string);
	while(i<length)
	{
		string[i] = ' ';
		i++;
	}

	string[i]='\0';
}


static void buildtag(char *buf, char *title, char *artist, char *album, char *year, char *comment, char *genre)
{

	strcpy(buf,"TAG");
	pad(title,30);
	strncat(buf,title,30);
	pad(artist,30);
	strncat(buf,artist,30);
	pad(album,30);
	strncat(buf,album,30);
	pad(year,4);
	strncat(buf,year,4);
	pad(comment,30);
	strncat(buf,comment,30);
	strncat(buf,genre,1);
}


bool hasId3V1(FILE *media) {
	if (fseek(media, ID3_V1_OFFSET, SEEK_END)) {
		return false;
	}

	char readHeader[ID3_V1_HEADER_SIZE];
	fread(readHeader, sizeof(char), ID3_V1_HEADER_SIZE, media);

	return 0 == strncmp(ID3_V1_HEADER, readHeader, sizeof(ID3_V1_HEADER));
}


int convertId3V1ToMediaTags(MediaTags *mediaTags, FILE *media) {
	int status = 0;
	if ((status = fseek(media, ID3_V1_OFFSET + ID3_V1_HEADER_SIZE, SEEK_END))) {
		return status;
	}

	fread(mediaTags->title, sizeof(char), ID3_V1_TITLE_SIZE, media);
	mediaTags->title[ID3_V1_TITLE_SIZE] = '\0';

	fread(mediaTags->artist, sizeof(char), ID3_V1_ARTIST_SIZE, media);
	mediaTags->artist[ID3_V1_ARTIST_SIZE] = '\0';

	fread(mediaTags->album, sizeof(char), ID3_V1_ALBUM_SIZE, media);
	mediaTags->album[ID3_V1_ALBUM_SIZE] = '\0';

	fread(mediaTags->year, sizeof(char), ID3_V1_YEAR_SIZE, media);
	mediaTags->year[ID3_V1_YEAR_SIZE] = '\0';

	fread(mediaTags->comment, sizeof(char), ID3_V1_COMMENT_SIZE, media);
	mediaTags->comment[ID3_V1_COMMENT_SIZE] = '\0';

	fread(mediaTags->genre, sizeof(char), ID3_V1_GENRE_SIZE, media);
	mediaTags->genre[ID3_V1_GENRE_SIZE] = '\0';

	// Check for presence of a track number. If comment[28] is 0x00, then comment[29] represents a track number
	if (mediaTags->comment[28] == '\x00') {
		//mediaTags->comment[28] = '\0';	// probably unnecessary, but Idk if '\0' is guaranteed to be 0x00
		long int track = strtol(&(mediaTags->comment[29]), NULL, 16);
		sprintf(mediaTags->track, "%ld", track);
	}

	mediaTags->tagSize = ID3_V1_OFFSET;
	mediaTags->applyTag = &setId3V1TagsInFile;

	return status;
}


int setId3V1TagsInFile(FILE *media, MediaTags mediaTags) {
	ssize_t tagOffset = 0;
	if (hasId3V1(media)) {
		tagOffset = ID3_V1_OFFSET;
	}
	fseek(media, tagOffset, SEEK_END);

	char fullline[228]="";
	buildtag(fullline,mediaTags.title,mediaTags.artist,mediaTags.album,mediaTags.year,mediaTags.comment,mediaTags.genre);
	fwrite(fullline, sizeof(fullline[0]), ID3_V1_SIZE, media);

	return 1;
}
