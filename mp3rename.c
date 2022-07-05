/**
 * @file mp3rename.c
 *
 * Sander Janssen			<janssen@rendo.dekooi.nl>
 *
 * This software is covered by the GNU public license,
 * which should be in a file named LICENSE acompanying
 * this.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

// should be set to contain the largest field of any tag type +1 to null terminate
#define TITLE_MAX_LENGTH	61
#define ARTIST_MAX_LENGTH	61
#define ALBUM_MAX_LENGTH	61
#define YEAR_MAX_LENGTH		5
#define COMMENT_MAX_LENGTH	44
#define TRACK_MAX_LENGTH	3
#define GENRE_MAX_LENGTH	31
#define SUBGENRE_MAX_LENGTH	21

typedef struct MediaTags {
	char title[TITLE_MAX_LENGTH];
	char artist[ARTIST_MAX_LENGTH];
	char album[ALBUM_MAX_LENGTH];
	char year[YEAR_MAX_LENGTH];
	char comment[COMMENT_MAX_LENGTH];
	char track[TRACK_MAX_LENGTH];
	char genre[GENRE_MAX_LENGTH];
	char subgenre[SUBGENRE_MAX_LENGTH];

	// The size of the tag when applied to a media file.
	size_t tagSize;

	// Callback function for applying the tag to media.
	int (*applyTag)(FILE *media, struct MediaTags mediaTags);
} MediaTags;

#define OPTIONS_INITIALIZER	{.verbose = false, .forced = false, .burn = false, .info = false, .all = false}
typedef struct Options {
	bool verbose;
	bool forced;
	bool burn;
	bool info;
	bool all;
} Options;


int parseArguments(Options *options, int argc, char *argv[]);
void verifyOptions(Options options);

void pad(char *string, int length);
void display_help();
void buildtag(char *buf, char *title, char *artist, char *album, char *year, char *comment, char *genre);
void set_filename(int argc,char *argv[]);
size_t dirFromPath(char *directory, size_t n, char *fullPath);
bool isMp3File(FILE *media);

bool hasId3V1(FILE *media);
int convertId3V1ToMediaTags(MediaTags *mediaTags, FILE *media);
int setIdV3TagsInFile(FILE *media, MediaTags mediaTags);


int main(int argc, char *argv[])
{
	Options options = OPTIONS_INITIALIZER;
	MediaTags mediaTags = {};

	FILE *configFile;
	FILE *mediaFile;
	char input_char;
	size_t position;
	char filenamelook[100];
	char configFileName[100];


	if (argc < 2 ) /* If nothing is given */
	{
		fprintf(stderr,"Mp3rename\n\nusage: [-vfhsbia] [file ...]\n\n");
		return 0;
	}

	/* Lets checkout the options */
	int argsIndex = parseArguments(&options, argc, argv);
	argv += argsIndex;
	verifyOptions(options);

	sprintf(configFileName,"%s/.mp3rename",getenv("HOME")); /* Lets get the home dir */

	if (!(configFile = fopen(configFileName,"r"))) /* if we don't find our config file */
	{
		sprintf(filenamelook,"(&a)-&t");
		printf("Using default look, please use the -s option to set your own look\n");
	}
	else	/* found! */
	{
		fgets(filenamelook, 100, configFile);
		fclose(configFile);
	}

	if(!options.burn) /* If burn is on we will add the .mp3 later */
		strcat(filenamelook,".mp3"); /* add .mp3 so that the filename will be complete */

	do {
		//char title[31]="", artist[31]="", album[31]="", year[5]="", comment[31]="", genre[1];
		char newfilename[160]="", newFilePath[150]="", dir[150]="";
		char burnname[29]="";

		if ( !( mediaFile=fopen(*argv,"rb+") ) )		/* If the file doesn exist */
		{
			perror("Error opening file");
			++argv;												 /* Prepare for the next file */
			continue;
		}

		/* Lets check if we have a real mp3 file */
		if(!isMp3File(mediaFile))
		{
			fprintf(stderr,"%s is not an MP3 file!\n",*argv);
			fclose(mediaFile);
			++argv;
			continue;
		}


		/* Lets see if we already have a id3 tag */
		bool containsId3V1 = hasId3V1(mediaFile);

		// If file cannot be handled, move on to next file
		if (!(containsId3V1 || options.forced || options.verbose)) {
				printf("%s hasen't got a id3 tag\n",*argv);
				fclose(mediaFile);
				++argv;
				continue;
		}

		// If existing tags were found, read them in and set offset accordingly
		if (containsId3V1) {
			convertId3V1ToMediaTags(&mediaTags, mediaFile);
		}

		// If manual input was requested, time to ask for input!
		if (options.forced || (options.verbose && !containsId3V1)) {
			if(options.verbose)
				printf("%s hasen't got a id3 tag. \n",*argv);
			else
				printf("%s:\n",*argv);
			if(!options.all)
			{
				for(int i=0 ; i!=(strlen(filenamelook)) ; i++)
				{
					position = 0;
					if(filenamelook[i] == '&')
					{
						switch(filenamelook[i+1])
						{
							case 'a':
								printf("Please enter the artist's name.\n");
								do	/* Lets get the artist */
								{
									input_char = getchar();
									if (input_char != '\n' && position == 0)
										strcpy(mediaTags.artist,"");
									if (input_char != '\n' && input_char != EOF )
										sprintf(mediaTags.artist,"%s%c",mediaTags.artist,input_char);
									position++;
								} while (input_char != '\n');
								i++;
								break;

							case 't':
								printf("Please enter the title.\n");
								do	/* Lets get the song title */
								{
									input_char = getchar();
									if (input_char != '\n' && position == 0)
										strcpy(mediaTags.title,"");
									if (input_char != '\n' && input_char != EOF)
										sprintf(mediaTags.title,"%s%c",mediaTags.title,input_char);
									position++;
								} while (input_char != '\n');
								i++;
								break;

							case 'b':
								printf("Please enter the album.\n");
								do	/* Lets get the album */
								{
									input_char = getchar();
									if (input_char != '\n' && position == 0)
										strcpy(mediaTags.album,"");
									if (input_char != '\n' && input_char != EOF)
										sprintf(mediaTags.album,"%s%c",mediaTags.album,input_char);
									position++;
								} while (input_char != '\n');
								i++;
								break;

							case 'y':
								printf("Please enter the year.\n");
								do	/* Lets get the year */
								{
									input_char = getchar();
									if (input_char != '\n' && position == 0)
										strcpy(mediaTags.year,"");
									if (input_char != '\n' && input_char != EOF)
										sprintf(mediaTags.year,"%s%c",mediaTags.year,input_char);
									position++;
								} while (input_char != '\n');
								i++;
								break;

							default: /* the user has entered a character behind the & we don't know */
								printf("Illegal char in config file please use the option '-s help' for more information");
								exit(1);
						}
					}
				}
			}
			else
			{
				position = 0;
				printf("Please enter the artist's name.\n");
				do	/* Lets get the artist */
				{
					input_char = getchar();
					if (input_char != '\n' && position == 0)
						strcpy(mediaTags.artist,"");
					if (input_char != '\n' && input_char != EOF )
						sprintf(mediaTags.artist,"%s%c",mediaTags.artist,input_char);
					position++;
				} while (input_char != '\n');

				position = 0;
				printf("Please enter the title.\n");
				do	/* Lets get the song title */
				{
					input_char = getchar();
					if (input_char != '\n' && position == 0)
						strcpy(mediaTags.title,"");
					if (input_char != '\n' && input_char != EOF)
						sprintf(mediaTags.title,"%s%c",mediaTags.title,input_char);
					position++;
				} while (input_char != '\n');

				position = 0;
				printf("Please enter the album.\n");
				do	/* Lets get the album */
				{
					input_char = getchar();
					if (input_char != '\n' && position == 0)
						strcpy(mediaTags.album,"");
					if (input_char != '\n' && input_char != EOF)
						sprintf(mediaTags.album,"%s%c",mediaTags.album,input_char);
					position++;
				} while (input_char != '\n');

				position = 0;
				printf("Please enter the year.\n");
				do	/* Lets get the year */
				{
					input_char = getchar();
					if (input_char != '\n' && position == 0)
						strcpy(mediaTags.year,"");
					if (input_char != '\n' && input_char != EOF)
						sprintf(mediaTags.year,"%s%c",mediaTags.year,input_char);
					position++;
				} while (input_char != '\n');
			}
		}

		if(options.info)
		{
			printf("Artist : %s\n",mediaTags.artist);
			printf("Title : %s\n",mediaTags.title);
			printf("Album : %s\n",mediaTags.album);
			printf("Year : %s\n\n",mediaTags.year);
			++argv;
			continue;
		}

		/* Remove trailing spaces */
		int i;
		i=strlen(mediaTags.artist)-1;
		while (i && mediaTags.artist[i]==' ')
		{
			mediaTags.artist[i]='\0';
			i--;
		}

		i=strlen(mediaTags.title)-1;
		while (i && mediaTags.title[i]==' ')
		{
			mediaTags.title[i]='\0';
			i--;
		}
		i=strlen(mediaTags.album)-1;
		while (i && mediaTags.album[i]==' ')
		{
			mediaTags.album[i]='\0';
			i--;
		}
		i=strlen(mediaTags.year)-1;
		while (i && mediaTags.year[i]==' ')
		{
			mediaTags.year[i]='\0';
			i--;
		}

		/* We go through the filenamelook until we find a &x combination
			 then we replace the &x with album/title/year/artis						*/
		for(int i=0 ; i!=(strlen(filenamelook)) ; i++)
		{
			if(filenamelook[i] == '&')
			{
				char tmp[100];
				switch(filenamelook[i+1])
				{
					case 'a':
						sprintf(tmp,"%s%s",newfilename,mediaTags.artist);
						strcpy(newfilename,tmp);
						i++;
						break;

					case 't':
						sprintf(tmp,"%s%s",newfilename,mediaTags.title);
						strcpy(newfilename,tmp);
						i++;
						break;

					case 'b':
						sprintf(tmp,"%s%s",newfilename,mediaTags.album);
						strcpy(newfilename,tmp);
						i++;
						break;

					case 'y':
						sprintf(tmp,"%s%s",newfilename,mediaTags.year);
						strcpy(newfilename,tmp);
						i++;
						break;

					default:
						printf("Illegal char in config file please use the option '-s help' for more information\n");
						exit(1);
				}
			}
			else /* otherwise we just print the character in the filenamelook in the newfilename */
			{
				char tmp[100];
				sprintf(tmp,"%s%c",newfilename,filenamelook[i]);
				strcpy(newfilename,tmp);
			}
		}

		/* Lets find out what directory we are working in */
		int dirSize = sizeof(dir)/sizeof(dir[0]);
		dirFromPath(dir, dirSize, *argv);

		/* Build the new tag from the new names */
		mediaTags.applyTag(mediaFile, mediaTags);	// TODO: handle error cases. Possibly abort execution?
		fclose(mediaFile);

		/* Lets catch illegal characters */
		for (int i=0; i < strlen(newfilename); i++)
		{
			switch (newfilename[i])
			{
				case '<': newfilename[i] = '[';
					break;
				case '>': newfilename[i] = ']';
					break;
				case '|': newfilename[i] = '_';
					break;
				case '/': newfilename[i] = '-';
					break;
				case '\\': newfilename[i]= '-';
					break;
				case '*': newfilename[i] = '_';
					break;
				case '?': newfilename[i] = '_';
					break;
				case ':': newfilename[i] = ';';
					break;
				case '"': newfilename[i] = '-';
					break;
				default: break;
			}
		}

		/* Lets rename the file */
		if(options.burn) /* If burn is on the size */
		{					/* shouldn't be bigger than 32 chars including the .mp3 */
			strncpy(burnname,newfilename,sizeof(burnname)-1);
			sprintf(newFilePath,"%s%s.mp3",dir,burnname);
		}
		else
		{
			sprintf(newFilePath,"%s%s",dir,newfilename);
		}
		if(rename(*argv,newFilePath))
			printf("Error renaming %s\n",*argv);

		++argv;	 /* Prepare for the next */
	} while (*argv);

	return 0;
}


/** @brief Parses the command line arguments into an Options struct
 *
 * @param options Struct to store the options into
 * @param argc Size of argv
 * @param argv[] Array of command line arguments
 * @return Index of next element in argv
 *
 */
int parseArguments(Options *options, int argc, char *argv[]) {
	char option;
	while ((option = getopt(argc, argv, "vfhsbia")) != -1)
	switch (option)
	{
		case 'v':											/* Verbose mode */
			options->verbose = true;
			break;
		case 'f':											/* Always ask mode */
			options->forced = true;
			break;
		case 'h':											/* Display the help */
			display_help();
			exit(1);
		case 's':											/* Set the default look */
			set_filename(argc,argv);
			exit(1);
		case 'b':											/* Burn modus cut of at 32 chars */
			options->burn = true;
			break;
		case 'i':											/* Just the id3tag */
			options->info = true;
			break;
		case 'a':											/* Ask everything */
			options->all = true;
			break;
		default:											 /* If wrong option is given */
			fprintf(stderr,"Mp3rename\n\nusage: [-vfh] [file ...]\n\n");
			exit(1);
	}

	return optind;
}


/** @brief Checks whether the selected options are compatible with each other.
 * If options are not compatible, a message is printed, and execution is exited with a non-zero status.
 *
 * @param options Options selected by user
 *
 */
void verifyOptions(Options options) {
	if ( options.info && (options.forced || options.verbose))
	{
		printf("Info modus can not be used with other arguments.\n\n");
		exit(1);
	}
}


void buildtag(char *buf, char *title, char *artist, char *album, char *year, char *comment, char *genre)
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

void pad(char *string, int length)
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

void display_help()
{
	printf("Mp3rename 0.6\n\n");
	printf("Options:\n");
	printf("\t-f\t Force non id3 rename.\n");
	printf("\t-v\t Verbose mode.\n");
	printf("\t-h\t Display this help message.\n");
	printf("\t-b\t Limit the file size to 32 chars.\n");
	printf("\t-i\t Only show the id3tags.\n");
	printf("\t-a\t Ask everything for the id3tag.\n\n");
	printf("\t-s\t Set the default filename look.\n");
	printf("\t	\t for more help on this option: -s help\n\n");
	printf("Sander Janssen <janssen@rendo.dekooi.nl>\n\n");

}
void set_filename(int argc,char *argv[])
{
	FILE *fp;
	char str[100];

	argv += 2;

	if(*argv == NULL)
	{
		printf("No config format given\n\r");
		return;
	}

	if(!strcmp(*argv,"help"))
	{
		printf("Mp3rename 0.6\n\n");
		printf(" Use this option to set the default look of the file\n");
		printf(" The information is saved in ~/.mp3rename\n");
		printf(" You can use &t title, &b album, &y year and &a artist\n\n");
		printf(" Example : mp3rename -s '(&a)-&t-&b'\n");
		printf(" for (artist)-title-album.mp3\n\n");
		return;
	}
	sprintf(str,"%s/.mp3rename",getenv("HOME"));

	if ( !( fp=fopen(str,"w") ) )
	{
		printf("Error opening config file\n\r");
		return;
	}

	if(!strcmp(*argv,""))
	{
		printf("No config format given\n\r");
		return;
	}
	printf("Default is now set\n\n");

	fprintf(fp,*argv);
	fclose(fp);
}


/** @brief Copies the directory excluding the last element, assumed to be a file.
 *
 * @param directory Output buffer to store the directory in
 * @param n Maximum size of directory, including null terminator
 * @param fullPath Input buffer containing the full path
 * @return size of directory written, not including null terminator
 *
 */
size_t dirFromPath(char *directory, size_t n, char *fullPath) {
	int dirSize = n - 1;

	strncpy(directory, fullPath, dirSize);
	for (; dirSize > 0; dirSize--) {
		if (directory[dirSize] == '/') {
			directory[dirSize] = '\0';
			break;
		}
	}
	if (dirSize == 0) {
		directory[dirSize] = '\0';
	}


	return dirSize;
}


/** @brief Checks if the provided file is an mp3 or not.
 *
 * @param media File being checked
 * @return true if File is an mp3, else false
 *
 */
bool isMp3File(FILE *media){
	char sig[2];

	fseek(media, 0, SEEK_SET);
	fread(sig, sizeof(sig), 1, media);

	sig[0] &= 0xff;
	sig[1] &= 0xf0;
	return (sig[0] == 0xff) && (sig[1] == 0xf0);
}


/** TAG CONVERSION **/

static const char ID3_V1_HEADER[]		= "TAG";
static const char ID3_ENHANCED_HEADER[]	= "TAG+";
static const char ID3_V1_2_HEADER[]		= "EXT";

static const size_t ID3_V1_SIZE = 128;
static const ssize_t ID3_V1_HEADER_OFFSET = -128;

static const size_t ID3_V1_HEADER_SIZE = 3;
static const size_t ID3_V1_TITLE_SIZE = 30;
static const size_t ID3_V1_ARTIST_SIZE = 30;
static const size_t ID3_V1_ALBUM_SIZE = 30;
static const size_t ID3_V1_YEAR_SIZE = 4;
static const size_t ID3_V1_COMMENT_SIZE = 30;
static const size_t ID3_V1_GENRE_SIZE = 1;


/** @brief Checks if the provided char array contains an ID3 V1 tag.
 *
 * @param media The file to search for the tag in
 * @return true if an ID3 V1 header was found, false otherwise
 *
 */
bool hasId3V1(FILE *media) {
	if (fseek(media, ID3_V1_HEADER_OFFSET, SEEK_END)) {
		return false;
	}

	char readHeader[ID3_V1_HEADER_SIZE];
	fread(readHeader, sizeof(char), ID3_V1_HEADER_SIZE, media);

	return 0 == strncmp(ID3_V1_HEADER, readHeader, sizeof(ID3_V1_HEADER));
}


/** @brief Populates a MediaTags struct from the contents of a media file.
 *
 * @param mediaTags Pointer to the MediaTags struct to populate
 * @param media The file to pull the tag data from
 * @return 0 if completed successfully, else non-zero
 *
 */
int convertId3V1ToMediaTags(MediaTags *mediaTags, FILE *media) {
	int status = 0;
	if ((status = fseek(media, ID3_V1_HEADER_OFFSET + ID3_V1_HEADER_SIZE, SEEK_END))) {
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

	mediaTags->tagSize = ID3_V1_HEADER_OFFSET;
	mediaTags->applyTag = &setIdV3TagsInFile;

	return status;
}


/** @brief Inserts data from a MediaTags struct into the provided media file.
 *
 * @param media Stream of the file to update
 * @param mediaTags MediaTags struct to read tag data from
 * @return 0 if completed successfully, non-zero otherwise.
 *
 */
int setIdV3TagsInFile(FILE *media, MediaTags mediaTags) {
	ssize_t tagOffset = 0;
	if (hasId3V1(media)) {
		tagOffset = ID3_V1_HEADER_OFFSET;
	}
	fseek(media, tagOffset, SEEK_END);

	char fullline[228]="";
	buildtag(fullline,mediaTags.title,mediaTags.artist,mediaTags.album,mediaTags.year,mediaTags.comment,mediaTags.genre);
	fwrite(fullline, sizeof(fullline[0]), ID3_V1_SIZE, media);

	return 1;
}
