////////////////////////////////////////////////////////////////////////
// COMP1521 23T1 --- Assignment 2: `rain', a simple file archiver
// <https://www.cse.unsw.edu.au/~cs1521/23T1/assignments/ass2/index.html>
//
// Written by YOUR-NAME-HERE (z5555555) on INSERT-DATE-HERE.
//
// 2021-11-08   v1.1    Team COMP1521 <cs1521 at cse.unsw.edu.au>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "rain.h"



// ADD ANY extra #defines HERE
// ADD YOUR FUNCTION PROTOTYPES HERE

mode_t convert_permissions(const char *permissions);
void create_drop_single(FILE *drop_file, uint8_t format,
    char *pathname,int recurse);
void free_sub_paths(char **sub_paths, int num_sub_paths);
char** split_path(const char *pathname, int *num_sub_paths);

void convert_7bit_to_8bit(const uint8_t *input, size_t input_size, uint8_t *output);
void convert_6bit_to_8bit(const uint8_t *input, size_t input_size, uint8_t *output);
void convert_8bit_to_7bit(const uint8_t *input, uint64_t input_size, uint8_t *output);
void convert_8bit_to_6bit(const uint8_t *input, uint64_t input_size, uint8_t *output);


// print the files & directories stored in drop_pathname (subset 0)
//
// if long_listing is non-zero then file/directory permissions, formats & sizes are also printed (subset 0)
void list_drop(char *drop_pathname, int long_listing) {
    //L => long_listing= true 
    //l => long_listing= false 
    FILE *fp;
    uint8_t  c,hash,format, permissions[11], *pathname, *content;;
    uint16_t pathnamelen;
    uint64_t contentlen; 
    //initializing
    pathnamelen=0;
    contentlen=0;
    permissions[10] = '\0'; // ensure null-termination

    fp = fopen(drop_pathname, "rb");
    if (fp == NULL) {
        return;}

    while (fread(&c, sizeof(c), 1, fp) == 1) {
        if (c == 0x63) {
            fread(&format, sizeof(format), 1, fp);
            fread(permissions, sizeof(permissions)-1, 1, fp);
            fread(&pathnamelen, 2, 1, fp); 
            pathname = (unsigned char *) malloc(pathnamelen + 1);

            fread(pathname, sizeof(char), pathnamelen, fp);
            pathname[pathnamelen] = '\0'; 
            
            fread(&contentlen, 6, 1, fp);
            
            //compensiate for the 7,6 bit
            uint64_t contentlenC= ((format-48)*contentlen)/8;
            content = (unsigned char *) malloc(contentlenC + 1);
          
            fread(content, sizeof(char), contentlenC, fp);
            content[contentlenC] = '\0'; // ensure null-termination
            
            fread(&hash, sizeof(hash), 1, fp);

            //print out the final result
            if (long_listing) {
                // Print out the extracted information
                printf("%s  %c  %5lu  %s\n", permissions,format,contentlen,pathname);   
            }
            else
            {
                printf("%s\n", pathname);
            }
            free(pathname);
            free(content);
        }
    }
    fclose(fp);
}


// check the files & directories stored in drop_pathname (subset 1)
//
// prints the files & directories stored in drop_pathname with a message
// either, indicating the hash byte is correct, or
// indicating the hash byte is incorrect, what the incorrect value is and the correct value would be
void check_drop(char *drop_pathname) {
    FILE *file = fopen(drop_pathname, "rb");
    if (!file) {
        return;}

    uint8_t c, byte_value,calculated_hash,original_hash,format;
    uint16_t filename_length=0;
    uint64_t content_length=0;

    while (fread(&c, sizeof(c), 1, file) == 1) {
        if (c == 0x63) {
        // Read 12-byte header and calculate hash
        calculated_hash = 0;
        calculated_hash = droplet_hash(calculated_hash, c);
        
        format = getc(file);
        calculated_hash = droplet_hash(calculated_hash, format);

        for (int i = 0; i < 10; ++i) {
            byte_value = getc(file);
            calculated_hash = droplet_hash(calculated_hash, byte_value);
        }

        // Read 2-byte filename length
        fread(&filename_length, 2, 1, file);
        calculated_hash = droplet_hash(calculated_hash, filename_length & 0xFF);
        calculated_hash = droplet_hash(calculated_hash, (filename_length >> 8) & 0xFF);

        // Read filename and calculate hash
        char *filename = (char *)malloc((filename_length + 1) * sizeof(char));
        for (int i = 0; i < filename_length; ++i) {
            byte_value = getc(file);
            calculated_hash = droplet_hash(calculated_hash, byte_value);
            filename[i] = byte_value;
        }
        filename[filename_length] = '\0';

        // Read 6-byte content length and calculate hash
        fread(&content_length, 6, 1, file);
        for (int i = 0; i < 6; ++i) {
            calculated_hash = droplet_hash(calculated_hash, (content_length >> (8 * i)) & 0xFF);
        }

        // Read content and calculate hash
        uint64_t tmp= content_length;
        content_length= ((format-48)*content_length)/8;
        if(format-48==7 || format-48==6)
        {if(tmp%8!=0){content_length++;}}

        for (uint64_t i = 0; i < content_length; ++i) {
            byte_value = getc(file);
            calculated_hash = droplet_hash(calculated_hash, byte_value);
        }

        // Read 1-byte original hash
        original_hash = getc(file);

        // Compare calculated hash and original hash
        if (calculated_hash == original_hash) {
            printf("%s - correct hash\n", filename);
        } 
        else {
            printf("%s - incorrect hash 0x%x should be 0x%x\n", filename,calculated_hash, original_hash );
        }
        free(filename);
    }
    else
    {
        //if the magic is not correct
        fprintf(stderr,"error: incorrect first droplet byte: 0x%x should be 0x63\n", c);
        fclose(file);
        return;
    }
    }
    fclose(file);
}



// extract the files/directories stored in drop_pathname (subset 2 & 3)
void extract_drop(char *drop_pathname) {
    FILE *file = fopen(drop_pathname, "rb");

    if (!file) {
        return;
    }

    uint8_t byte_value,format;
    uint16_t filename_length=0;
    uint64_t content_length=0;

    while (!feof(file)) 
    {
    byte_value = getc(file);
    if(byte_value== 0x63)
    {
    format = getc(file);
    
    //read the permissions
    char permissions[10];
    for (int i=0; i<10;i++){permissions[i]=getc(file);}
    mode_t file_permissions = convert_permissions(permissions);   
    file_permissions &= ~(S_IFDIR); //remove d

    // Read 2-byte filename length
    fread(&filename_length, 2, 1, file);
    // Read filename
    char *filename = (char *) malloc((filename_length + 1) * sizeof(char));
    fread(filename, 1, filename_length, file);
    filename[filename_length] = '\0';

    if(permissions[0]=='d')
    {
        printf("Creating directory: %s\n",filename);
        fseek(file, 7, SEEK_CUR); //IGNORE THE HASH
        //creating the directory
        if(mkdir(filename,0755)!=0)
            {return;}
        continue;
    }

    else
    {
        printf("Extracting: %s\n", filename);
        // Read 6-byte content length
        fread(&content_length, 6, 1, file);
        uint64_t content_lengthG= ((format-48)*content_length)/8;
        //if(format-48==7 || format-48==6)
        //   {if(content_length%8!=0){content_lengthG++;}}

        // Create a new file with the extracted filename
        FILE *output_file = fopen(filename, "wb");
        if (!output_file) 
        {
            free(filename);
            fclose(file);
            return;
        }

        // Write content to the output file
        uint8_t *content = (uint8_t *) malloc((content_lengthG + 1) * sizeof(uint8_t));
        for (uint64_t i = 0; i < content_lengthG; ++i) 
        {
            content[i] = getc(file);
        }
        if(format-48==7){
            uint8_t *out = (uint8_t *) malloc((content_length + 1) * sizeof(uint8_t));
            convert_7bit_to_8bit(content, content_lengthG, out);
            out[content_length-1]=0x0a;
            fwrite(out, content_length, 1, output_file);
            free(out);
        }
        else if(format-48==6){
            uint8_t *out = (uint8_t *) malloc((content_length + 1) * sizeof(uint8_t));
            convert_6bit_to_8bit(content, content_lengthG, out);
            for(uint64_t i=0 ; i<content_length; i++)
            {

                out[i]=droplet_from_6_bit(out[i]);

            }
            out[content_length-1]=0x0a;
            fwrite(out, content_length, 1, output_file);
            free(out);
        }
        else
        {
        fwrite(content, content_lengthG, 1, output_file);
        }

        free(content);
        fseek(file, 1, SEEK_CUR); //IGNORE THE HASH

        //change the permission of the file
        fchmod(fileno(output_file),file_permissions);
        fclose(output_file);
        free(filename);
        }
    }}
    fclose(file);
}


// create drop_pathname containing the files or directories specified in pathnames (subset 3)
//
// if append is zero drop_pathname should be over-written if it exists
// if append is non-zero droplets should be instead appended to drop_pathname if it exists
//
// format specifies the droplet format to use, it must be one DROPLET_FMT_6,DROPLET_FMT_7 or DROPLET_FMT_8

void create_drop(char *drop_pathname, int append, int format,
                int n_pathnames, char *pathnames[]){
            //opening the dropfile
            FILE *drop_file; 
            if (append){drop_file = fopen(drop_pathname, "ab");}
            else{drop_file = fopen(drop_pathname, "wb");}
            if (drop_file == NULL){return;}


            for(int i=0; i< n_pathnames ;i++)
            {
            int num_sub_paths;
            char **sub_paths=split_path(pathnames[i], &num_sub_paths);
            
            for (int j=0; j< num_sub_paths-1; j++)
            {
                create_drop_single(drop_file,format,sub_paths[j],0);
            }

            //creating the filedata
            create_drop_single(drop_file,format,sub_paths[num_sub_paths-1],1);
            }
            }


// ADD YOUR EXTRA FUNCTIONS HERE
mode_t convert_permissions(const char *permissions) {
    mode_t file_permissions = 0;

    if (permissions[1] == 'r') file_permissions |= S_IRUSR;
    if (permissions[2] == 'w') file_permissions |= S_IWUSR;
    if (permissions[3] == 'x') file_permissions |= S_IXUSR;
    if (permissions[4] == 'r') file_permissions |= S_IRGRP;
    if (permissions[5] == 'w') file_permissions |= S_IWGRP;
    if (permissions[6] == 'x') file_permissions |= S_IXGRP;
    if (permissions[7] == 'r') file_permissions |= S_IROTH;
    if (permissions[8] == 'w') file_permissions |= S_IWOTH;
    if (permissions[9] == 'x') file_permissions |= S_IXOTH;

    return file_permissions;}


void create_drop_single(FILE *drop_file, uint8_t format,char *pathname,int recurse){
        uint8_t start_byte = 0x63;
        uint8_t format_byte = (uint8_t)format;
        uint16_t pathname_length = strlen(pathname);
        uint64_t content_length = 0;

        //opning the file
        FILE *content_file = fopen(pathname, "rb");
        if (content_file == NULL)
        {fclose(drop_file);return;}

        //calculating the permissions
            struct stat file_stat;
            if(stat(pathname,&file_stat)!=0){return;}   
            mode_t mode = file_stat.st_mode;
            char permissions[10];
            permissions[0] = S_ISDIR(file_stat.st_mode) ? 'd' : '-'; //if its a directory
            for (int j = 0; j < 9; j++)
            {permissions[j+1] = (mode & (1 << (8 - j))) ? "rwxrwxrwx"[j] : '-';}

        //Calculating the content length
        if(permissions[0] == 'd')
        {content_length = 0;}
        else
        {
            fseek(content_file, 0, SEEK_END);
            content_length = ftell(content_file);
            fseek(content_file, 0, SEEK_SET);
        }

        uint8_t hash = 0;
        //writing to the file
        fwrite(&start_byte, 1, 1, drop_file);
        hash = droplet_hash(hash,start_byte);
        fwrite(&format_byte, 1, 1, drop_file);
        hash = droplet_hash(hash,format_byte);
        fwrite(permissions, 1, 10, drop_file);
        for (uint32_t j = 0; j < 10; j++)
        {hash = droplet_hash(hash, permissions[j]);}

        fwrite(&pathname_length, sizeof(pathname_length), 1, drop_file);
        hash = droplet_hash(hash, pathname_length & 0xFF);
        hash = droplet_hash(hash, (pathname_length >> 8) & 0xFF);
        fwrite(pathname, 1, pathname_length, drop_file);
        for (uint32_t j = 0; j < pathname_length; j++)
        {hash = droplet_hash(hash, pathname[j]);}
        fwrite(&content_length, 6, 1, drop_file);
        for (int j = 0; j < 6; j++)
        {hash = droplet_hash(hash, ((content_length >> (8 * j)) & 0xFF));}

        //malloc space for content if permission is of directory      
        if(permissions[0] == 'd')
        {
            printf("Adding: %s\n",pathname);
            fwrite(&hash, 1, 1, drop_file);
            //-------------------------
            DIR *d;
            struct dirent *dir;
            d= opendir(pathname);
            if(d && recurse)
            {
                while((dir=readdir(d))!= NULL)
                {
                if(strcmp(dir->d_name,".") && strcmp(dir->d_name,".."))
                    {
                    //creating the address of the file
                    char *result;
                    result= (char *)malloc(strlen(pathname) +strlen(dir->d_name)+ 2);
                    strcpy(result,pathname);
                    strcat(result,"/");
                    strcat(result, dir->d_name);
                    create_drop_single(drop_file, format, result,1);
                    }
                }                
                closedir(d);
            }
        }

        else   
        {  
        uint64_t content_lengthG= ((format-48)*content_length)/8;
        if(format==DROPLET_FMT_6 || format==DROPLET_FMT_7)
        {if(content_length%8!=0){content_lengthG++;}}

        unsigned char *content = (unsigned char *)malloc((content_length + 1) * sizeof(char));
        content[content_length]='\0';

        if (content == NULL){fclose(content_file);fclose(drop_file);return;}
            fread(content, 1, content_length, content_file);
            
            //Check format
            if(format==DROPLET_FMT_7){
                //check for error
                for(uint64_t i=0;i<content_length;i++)
                {
                    if(content[i]>127)
                    {
                        fprintf(stderr,"error: byte 0x%x can not be represented in 7-bit format\n",content[i]);
                        return;
                    }
                }
                //code to convert from 8bit to 7bit
                unsigned char *out = (unsigned char *)malloc((content_lengthG+1) * sizeof(char));

                convert_8bit_to_7bit(content, content_lengthG, out);
                //write back to content
                for(uint64_t i=0;i<content_lengthG;i++)
                    {content[i]=out[i];}
                    content[content_lengthG]='\0';
                free(out);
            }

            else if(format==DROPLET_FMT_6){
                //check for error
                for(uint64_t i=0;i<content_length;i++)
                {
                    if(droplet_to_6_bit(content[i])==-1)
                    {
                        fprintf(stderr,"error: byte 0x%x can not be represented in 6-bit format\n",content[i]);
                        return;
                    }
                    content[i]=droplet_to_6_bit(content[i]);
                }
                

                //code to convert from 8bit to 6bit
                unsigned char *out = (unsigned char *)malloc((content_lengthG+1) * sizeof(char));
                convert_8bit_to_6bit(content, content_lengthG, out);
                //write back to content
                for(uint64_t i=0;i<content_lengthG;i++){content[i]=out[i];}
                content[content_lengthG]='\0';  
                free(out);
            }

            printf("Adding: %s\n",pathname);
            fwrite(content, 1, content_lengthG, drop_file);
            for (uint64_t j = 0; j < content_lengthG; j++)
            {hash = droplet_hash(hash, content[j]);}
            free(content);
            fwrite(&hash, 1, 1, drop_file);            
        }
        
        //write the hash
        fclose(content_file);
        return;}


void convert_7bit_to_8bit(const uint8_t *input, uint64_t input_size, uint8_t *output) {
   uint8_t mask = 0; 
   uint8_t shift= 0;
   uint8_t shiftR= 8;
   uint8_t bits=0;
   uint64_t j=0;    

   for (uint64_t i=0 ;i<input_size; i++)
   {
    mask=(mask*2)+1;
    shift++;shiftR--;
    output[j]=input[i]>>shift;
    output[j]=output[j] | (bits<<shiftR);
    bits=input[i] & mask;
    if(mask==0x7f){
        j++;
        output[j]= bits;
        mask=0;shift=0;shiftR=8; bits=0;
        }  
    j++;
   }
}
void convert_6bit_to_8bit(const uint8_t *input, uint64_t input_size, uint8_t *output) {
   uint8_t mask = 0; 
   uint8_t shift= 0;
   uint8_t shiftR= 8;
   uint8_t bits=0;
   uint64_t j=0;

   for (uint64_t i=0 ;i<input_size; i++)
   {
    shift+=2;shiftR-=2;
    mask=(mask*4)+3;
    output[j]=input[i]>>shift;
    output[j]=output[j] | (bits<<shiftR);
    bits=input[i] & mask;
    if(mask==0x3f){
        j++;
        output[j]= bits;
        mask=0;shift=0;shiftR=8; bits=0;
    } 
    j++;
   }
}
void convert_8bit_to_7bit(const uint8_t *input, uint64_t input_size, uint8_t *output)
{
   uint8_t mask = 128;  //1000 0000
   uint8_t shift= 0;
   uint8_t shiftR= 7;
   uint8_t bits=0;
   uint64_t j=0;    

   for (uint64_t i=0 ;i<input_size; i++)
   {
    mask= mask|(mask>>1); //1100 0000
    shift++;shiftR--;

    bits=(input[j+1] & mask)>>shiftR; //bits from next 
    output[i]=input[j]<<shift;
    output[i]=output[i]|bits;
    if(mask==0xFF)
    {
        mask = 128;shift=0;shiftR=7; bits=0;
        j++;
    } 
    j++; 
   }}
void convert_8bit_to_6bit(const uint8_t *input, uint64_t input_size, uint8_t *output)
{
   uint8_t mask = 0xc0;  //1100 0000
   uint8_t shift= 0;
   uint8_t shiftR= 6;
   uint8_t bits=0;
   uint64_t j=0;    

   for (uint64_t i=0 ;i<input_size; i++)
   {
    mask= mask|(mask>>2); //1111 0000
    shift+=2;shiftR-=2;
    bits=(input[j+1] & mask) >>shiftR; //bits from next 
    output[i]= input[j]<<shift;
    output[i]= output[i]|bits;
    if(mask==0xff)
    {
        mask = 0xc0;shift=0;shiftR=6; bits=0;
        j++;
    } 
    j++; 
   }}

char** split_path(const char *pathname, int *num_sub_paths) {
    // Count the number of sub-paths
    *num_sub_paths = 1;
    for (int i = 0; pathname[i] != '\0'; i++) {
        if (pathname[i] == '/') {
            (*num_sub_paths)++;
        }
    }

    // Allocate memory for sub-paths array
    char **sub_paths = (char **)malloc(*num_sub_paths * sizeof(char *));
    int start = 0;
    int sub_path_index = 0;
    for (int i = 0; pathname[i] != '\0'; i++) {
        if (pathname[i] == '/') {
            int len = i;
            sub_paths[sub_path_index] = (char *)malloc((len + 1) * sizeof(char));
            strncpy(sub_paths[sub_path_index], pathname, len);
            sub_paths[sub_path_index][len] = '\0';
            start = i + 1;
            sub_path_index++;
        }
    }

    // Add the last sub-path
    sub_paths[sub_path_index] = (char *)malloc((strlen(pathname) + 1) * sizeof(char));
    strcpy(sub_paths[sub_path_index], pathname);

    return sub_paths;
}

void free_sub_paths(char **sub_paths, int num_sub_paths) {
    for (int i = 0; i < num_sub_paths; i++) {
        free(sub_paths[i]);
    }
    free(sub_paths);
}