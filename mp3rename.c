/*
 * mp3rename.c
 *
 * Sander Janssen      <janssen@rendo.dekooi.nl>
 *
 * This software is covered by the GNU public license, 
 * which should be in a file named LICENSE acompanying 
 * this.          
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

void pad(char *string, int length);
void display_help();
void buildtag(char *buf, char *title, char *artist, char *album, char *year, char *comment, char *genre);
void set_filename(int argc,char *argv[]);

int main(int argc, char *argv[]) 
{
  FILE *fp;
  int verbose = 0, forced = 0, burn = 0, info = 0, all = 0;
  unsigned char sig[2];
  char genre[1];
  char input_char;
  int i=0,plaatsen = 0;
  int ch,p;
  char filenamelook[100];
  char str[100];
 
  
  if (argc < 2 ) /* If nothing is given */
  {
    fprintf(stderr,"Mp3rename\n\nusage: [-vfhsbia] [file ...]\n\n");
    return 0;
  }
  
  /* Lets checkout the options */

  while ((ch = getopt(argc, argv, "vfhsbia")) != -1)
    switch (ch) 
    {
      case 'v':                      /* Verbose mode */
        verbose = 1;
        break;
      case 'f':                      /* Always ask mode */
        forced = 1;
        break;
      case 'h':                      /* Display the help */
        display_help();
        exit(1);
      case 's':                      /* Set the default look */
        set_filename(argc,argv);
        exit(1);
      case 'b':                      /* Burn modus cut of at 32 chars */
        burn = 1;
        break;
      case 'i':                      /* Just the id3tag */
	info = 1;
	break;
      case 'a':                      /* Ask everything */
	all = 1;
	break;
      default:                       /* If wrong option is given */
        fprintf(stderr,"Mp3rename\n\nusage: [-vfh] [file ...]\n\n");
        exit(1);
    }
  argv += optind;

  if ( info == 1 && ( forced == 1 || verbose == 1)) 
    {
      printf("Info modus can not be used with other arguments.\n\n");
      exit(1);
    }
  sprintf(str,"%s/.mp3rename",getenv("HOME")); /* Lets get the home dir */

  if ( !( fp=fopen(str,"r") ) ) /* if we don't find our config file */
    {
     sprintf(filenamelook,"(&a)-&t");
     printf("Using default look, please use the -s option to set your own look\n");
    }
  else  /* found! */
      fgets(filenamelook, 100, fp);

 if(burn != 1) /* If burn is on we will add the .mp3 later */
  strcat(filenamelook,".mp3"); /* add .mp3 so that the filename will be complete */

do {
 char title[31]="", artist[31]="", album[31]="", year[5]="", comment[31]="", fbuf[4], newfilename[160]="",nieuw[150]="",dir[150]="",dirsource[200],fullline[228]="", burnname[29]=""; 
  plaatsen = 0;

  if ( !( fp=fopen(*argv,"rb+") ) )    /* If the file doesn exist */
  {
     perror("Error opening file");
     ++argv;                         /* Prepare for the next file */ 
     continue;
  }

  /* Lets check if we have a real mp3 file */

  fread(sig,sizeof(sig),1,fp);                         
  sig[0] &= 0xff;
  sig[1] &= 0xf0;                                                         
  if(!((sig[0] == 0xff) && (sig[1] == 0xf0)))
  {
    fprintf(stderr,"%s is not an MP3 file!\n",*argv);        
    fclose(fp);
    ++argv;
    continue;
  }                 

  /* Lets go to the beginning of the tag */
   if ( fseek(fp, -128, SEEK_END )) 
   { 
     fclose(fp);
     ++argv;
     continue; 
   }

   /* Lets see if we already have a id3 tag */
   fread(fbuf,1,3,fp); fbuf[3] = '\0';
   if (!strcmp("TAG",fbuf) && !forced)
   {
     fseek(fp, -125, SEEK_END);                          
     fread(title,1,30,fp); title[30] = '\0';            
     fread(artist,1,30,fp); artist[30] = '\0';            
     fread(album,1,30,fp); album[30] = '\0';
     fread(year,1,4,fp); year[4] = '\0';
     fread(comment,1,30,fp); comment[30] = '\0';
     fread(genre,1,1,fp);
     fseek(fp, -128, SEEK_END); /* back to the beginning of the tag */
   } 
   else 
   {
    if (!strcmp("TAG",fbuf) ) /* go to the position to append one */
      {
        if(forced)
	  {
	    fseek(fp, -125, SEEK_END);                          
	    fread(title,1,30,fp); title[30] = '\0';            
	    fread(artist,1,30,fp); artist[30] = '\0';            
	    fread(album,1,30,fp); album[30] = '\0';
	    fread(year,1,4,fp); year[4] = '\0';
	    fread(comment,1,30,fp); comment[30] = '\0';
	    fread(genre,1,1,fp);
	  }
        fseek(fp, -128, SEEK_END); 
      }
    else
      fseek(fp, 0, SEEK_END);
      if(verbose || forced)            /* Manual change of the name */
      {
        if(verbose)
          printf("%s hasen't got a id3 tag. \n",*argv);
        else
          printf("%s:\n",*argv);
        if(!all)
	  {
	    for( i=0 ; i!=(strlen(filenamelook)) ; i++)
	      {
		p = 0;
		if(filenamelook[i] == '&')
		  {
		    switch(filenamelook[i+1])
		      {
		        case 'a':
			  printf("Please enter the artist's name.\n");
			  do  /* Lets get the artist */
			    { 
			      input_char = getchar();
			      if (input_char != '\n' && p == 0)
				strcpy(artist,"");
			      if (input_char != '\n' && input_char != EOF )
				sprintf(artist,"%s%c",artist,input_char);
			      p++;
			    } while (input_char != '\n');
			  i++;
			  break;
		        case 't':
			  printf("Please enter the title.\n");
			  do  /* Lets get the song title */
			    { 
			      input_char = getchar(); 
			      if (input_char != '\n' && p == 0)
				strcpy(title,"");
			      if (input_char != '\n' && input_char != EOF)
				sprintf(title,"%s%c",title,input_char);
			      p++;
			    } while (input_char != '\n');
			  i++;
			  break;
		        case 'b':
			  printf("Please enter the album.\n");
			  do  /* Lets get the album */
			    { 
			      input_char = getchar(); 
			      if (input_char != '\n' && p == 0)
				strcpy(album,"");
			      if (input_char != '\n' && input_char != EOF)
				sprintf(album,"%s%c",album,input_char);
			      p++;
			    } while (input_char != '\n');
			  i++;
			  break;
		        case 'y':
			  printf("Please enter the year.\n");
			  do  /* Lets get the year */
			    { 
			      input_char = getchar(); 
			      if (input_char != '\n' && p == 0)
				strcpy(year,"");
			      if (input_char != '\n' && input_char != EOF)
				sprintf(year,"%s%c",year,input_char);
			      p++;
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
	    p = 0;
	    printf("Please enter the artist's name.\n");
	    do  /* Lets get the artist */
	      { 
		input_char = getchar();
		if (input_char != '\n' && p == 0)
		  strcpy(artist,"");
		if (input_char != '\n' && input_char != EOF )
		  sprintf(artist,"%s%c",artist,input_char);
		p++;
	      } while (input_char != '\n');
	    p = 0;
	    printf("Please enter the title.\n");
	    do  /* Lets get the song title */
	      { 
		input_char = getchar(); 
		if (input_char != '\n' && p == 0)
		  strcpy(title,"");
		if (input_char != '\n' && input_char != EOF)
		  sprintf(title,"%s%c",title,input_char);
		p++;
	      } while (input_char != '\n');
	    i++;
	    p = 0;
	    printf("Please enter the album.\n");
	    do  /* Lets get the album */
	      { 
		input_char = getchar(); 
		if (input_char != '\n' && p == 0)
		  strcpy(album,"");
		if (input_char != '\n' && input_char != EOF)
		  sprintf(album,"%s%c",album,input_char);
		p++;
	      } while (input_char != '\n');
	    p = 0;
	    printf("Please enter the year.\n");
	    do  /* Lets get the year */
	      { 
		input_char = getchar(); 
		if (input_char != '\n' && p == 0)
		  strcpy(year,"");
		if (input_char != '\n' && input_char != EOF)
		  sprintf(year,"%s%c",year,input_char);
		p++;
	      } while (input_char != '\n');
	  }
        }
      else   /* If we aren't in verbose or forced mode */
        {
          printf("%s hasen't got a id3 tag\n",*argv); 
          fclose(fp);
          ++argv;
          continue;
        }
       
   }
  
  if(info == 1)
    {
      printf("Artist : %s\n",artist);
      printf("Title : %s\n",title);
      printf("Album : %s\n",album);
      printf("Year : %s\n\n",year);
      ++argv;
      continue;
    }

   /* Remove trailing spaces */
  i=strlen(artist)-1;
  while (i && artist[i]==' ') 
  {
    artist[i]='\0';
    i--;
  }

  i=strlen(title)-1;
  while (i && title[i]==' ') 
  {
    title[i]='\0';
    i--;
  }
  i=strlen(album)-1;
  while (i && album[i]==' ') 
  {
    album[i]='\0';
    i--;
  }
  i=strlen(year)-1;
  while (i && year[i]==' ') 
  {
    year[i]='\0';
    i--;
  }

  /* We go through the filenamelook until we find a &x combination
     then we replace the &x with album/title/year/artis            */ 
  for( i=0 ; i!=(strlen(filenamelook)) ; i++)
    {
     if(filenamelook[i] == '&') 
       {
         char tmp[100];
         switch(filenamelook[i+1])
	  {
	     case 'a':
                sprintf(tmp,"%s%s",newfilename,artist);
                strcpy(newfilename,tmp);
                i++;
                break;
             case 't':
                sprintf(tmp,"%s%s",newfilename,title);
                strcpy(newfilename,tmp);
                i++;
                break;
             case 'b':
                sprintf(tmp,"%s%s",newfilename,album);
                strcpy(newfilename,tmp);
                i++;
                break;
             case 'y':
                sprintf(tmp,"%s%s",newfilename,year);
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

  sprintf(dirsource,"%s",*argv);
  i=strlen(dirsource)+1;
  while (i>-1) 
  { 
   if(dirsource[i] == '/')
    plaatsen = 1;
   if(plaatsen)
    dir[i] = dirsource[i];
   else
    dir[i] = '\0';
    i--;
  }

  /* Build the new tag from the new names */ 

  buildtag(fullline,title,artist,album,year,comment,genre); 
  fwrite(fullline,1,128,fp);

  fclose(fp);

  /* Lets catch illegal characters */
 for (i=0; i < strlen(newfilename); i++)
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
  if(burn == 1) /* If burn is on the size */
    {	        /* shouldn't be bigger than 32 chars including the .mp3 */
      strncpy(burnname,newfilename,sizeof(burnname)-1);
      sprintf(nieuw,"%s%s.mp3",dir,burnname);
    }
  else
    {
      sprintf(nieuw,"%s%s",dir,newfilename);
    }    
  if(rename(*argv,nieuw))
    printf("Error renaming %s\n",*argv);

  ++argv;   /* Prepare for the next */


} while (*argv);

return 0;
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
  printf("\t  \t for more help on this option: -s help\n\n");
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




