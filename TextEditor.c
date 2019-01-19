/**
 * Machine Problem: Text Editor
 * CS 241 - Fall 2017
 */
#include "document.h"
#include "editor.h"
#include "format.h"
#include "vector.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_filename(int argc, char *argv[]) {

	if(argc<2){ print_usage_error();}
	char *fname=argv[1]; 
   
return fname;
}


document *handle_create_document(const char *path_to_file) {

	document* doc2=document_create_from_file(path_to_file);

return doc2;
}



void handle_cleanup(document *document) {

    	document_destroy(document);

}

void handle_display_command(document *document, size_t start_line,ssize_t max_lines, size_t start_col_index, ssize_t max_cols) {


size_t lno4=document_size(document);

if(lno4==0){print_document_empty_error();}

size_t ln;

if(start_line==0){
print_usage_error();
}

if(max_lines==-1){
ln=start_line;
	
	while(ln<=lno4){
		print_line(document,ln,start_col_index,max_cols);
		++ln;
		}

}

if(max_lines>(ssize_t)lno4){
ln=start_line;
	
	while(ln<=lno4){
                print_line(document,ln,start_col_index,max_cols);
                ++ln;
                }

}

if(max_lines<=(ssize_t)lno4){
ln=start_line;
	
	while((ssize_t)ln<(max_lines+(ssize_t)start_line)){
                print_line(document,ln,start_col_index,max_cols);
                ++ln;
                }

}

	
}

void handle_insert_command(document *document, location loc, const char *line) {
    

size_t suz= document_size(document);

	if(loc.line_no>suz){
		char* satra=malloc(strlen(line)+loc.idx+10);
		char* remfree3=satra;
		memset((void *)remfree3,' ',strlen(line)+loc.idx+10 );
		satra=satra+loc.idx;

		while(*line){
                	*satra=*line;
                	++satra;
                	++line;
                }

		*satra=*line;     				//null character insertion

	size_t diff=loc.line_no-suz-1;
	size_t i=1;
        	
		while(diff){
                	//document_set_line(document,suz+i,"\n");
                	document_insert_line(document, suz+i,"");
			--diff;
                	++i;
                }

	//document_set_line(document,loc.line_no,remfree2);
	document_insert_line(document,loc.line_no,remfree3);

	free(remfree3);
	}

        else{
		const char* strx=document_get_line(document,loc.line_no);
		assert(strx!=NULL);
		char* strchan=malloc(strlen(strx)+strlen(line)+10);
		strcpy(strchan,strx);
		char* remfree1=strchan;
		
		char* sin=malloc(strlen(strx)+strlen(line)+10);
		strcpy(sin,remfree1);
		char* remfree2=sin;
		sin=sin+loc.idx;
		
		strchan=strchan+loc.idx;
        
			while(*line){
				*strchan=*line;
	        		++strchan;
				++line;
			}
       
			while(*sin){
			*strchan=*sin;
			++strchan;
			++sin;
			}

		*strchan=*sin;

		document_set_line(document,loc.line_no,remfree1);

		free(remfree1);
		free(remfree2);
	}
}

void handle_append_command(document *document, size_t line_no,const char *line) {

size_t shin= document_size(document);

if(line_no>shin){
	handle_write_command(document,line_no,line);
return;
}

const char* sola=document_get_line(document,line_no);
//char* sam=strdup(sola);
char* sam=malloc(strlen(sola)+strlen(line)+10);
strcpy(sam,sola);
char* remfree3=sam;
static int count=0;

	if(count==0)
		sam=sam+strlen(sam);						//pointing at null char

const char* sc;
size_t li=line_no;
		
	while(*line){

		if(*line=='\\'){
		++line;

			if(*line=='n'){
  				*sam='\0';
				document_set_line(document,li,remfree3);

					sc=document_get_line(document,shin);
                                        document_insert_line(document,shin+1,sc);
                                        --shin;
	
				while(shin!=li)
				{
					sc=document_get_line(document,shin);
                			document_set_line(document,shin+1,sc);
                			--shin;	
				}	
				
				++li;
				++count;
				++line;	
				free(remfree3);
	                        
					handle_append_command(document,li,line);
		      			
					return;	
			}
	
			else if(*line!='\0'){

				*sam=*line;
				++sam;
				++line;
			}

			else continue;

		}

		else{                        
		     	*sam=*line;
                        ++sam;
                        ++line;
                }
	}
*sam=*line;
document_set_line(document,li,remfree3);
free(remfree3);
count=0;
return;

}

void handle_write_command(document *document, size_t line_no,const char *line) {
size_t shi= document_size(document);

static int cnt=0;

if(line_no>shi ){

if(cnt==0){

	size_t differen=line_no-shi-1;
        size_t i=1;

                while(differen){
                        document_insert_line(document,shi+i,"");
                        --differen;
                        ++i;
                }

}


char* arr=malloc(strlen(line)+10);

char* rem=arr;

size_t ln=line_no;

        while(*line){

                if(*line=='\\'){
                ++line;

                        if(*line=='n'){
                                *arr='\0';
                                document_insert_line(document,ln,rem);
                                ++cnt;
				++ln;
                                ++line;
                                free(rem);

                                        handle_write_command(document,ln,line);
					return;
                                
			}

                        else if(*line!='\0'){

                                *arr=*line;
                                ++arr;
                                ++line;
                        }

                        else continue;

                }

                else{
                        *arr=*line;
                        ++arr;
                        ++line;
                }
        }

*arr=*line;
document_insert_line(document,ln,rem);
free(rem);
return;

}

else{
char* loda=malloc(strlen(line)+10);
char* remfree4=loda;                                            

const char* sci;

size_t lin=line_no;

        while(*line){

                if(*line=='\\'){
                ++line;

                        if(*line=='n'){
                                *loda='\0';
                                document_set_line(document,lin,remfree4);
				//++cnt;
           				
					if(shi>lin){
						sci=document_get_line(document,shi);
                                        	document_insert_line(document,shi+1,sci);
						--shi;
						while(shi!=lin){
                    
                                        		sci=document_get_line(document,shi);
                                        		document_set_line(document,shi+1,sci);
                                        		--shi;
                                		}
					}
					
				++lin;
                                ++line;
                                free(remfree4);

                                        handle_write_command(document,lin,line);
					return;
                        		        
			}

                        else if(*line!='\0'){
                                *loda=*line;
                                ++loda;
                                ++line;
                        }
			
			else continue;

                }

                else{
                        *loda=*line;
                        ++loda;
                        ++line;
                }
        }

*loda=*line;
document_set_line(document,lin,remfree4);
free(remfree4);
return;

}// else ends


}





void handle_delete_command(document *document, location loc, size_t num_chars) {

const char* str1=document_get_line(document,loc.line_no);   
//char* str2=strdup(str1);
char* str2=malloc(strlen(str1)+loc.line_no+10);
strcpy(str2,str1);
char* str3= str2;
char* remfree=str2;
size_t len=(size_t)strlen(str2);


	if(loc.idx+num_chars>=len){
		str2=str2+loc.idx;
		*str2='\0';
		document_set_line(document,loc.line_no,remfree);
	}	

	else{
		str2=str2+loc.idx+num_chars;
		str3=str3+loc.idx;
		size_t diff=len-(loc.idx+num_chars)+1;

		while(diff){
			*str3=*str2;
			++str2;
			++str3;
			--diff;
		}

		document_set_line(document,loc.line_no,remfree);
	}


free(remfree);

}


void handle_delete_line(document *document, size_t line_no) {

	document_delete_line(document,line_no);
}



location handle_search_command(document *document, location loc,const char *search_str) {

if(strlen(search_str)==0){			//EMPTY STRING
	return (location){0,0};
}

size_t ct=0;
size_t curline=loc.line_no;
size_t curloc=loc.idx;
size_t docsiz= document_size(document);
int i=2;

while(i){
	while(curline<=docsiz){

		const char* str1=document_get_line(document,curline);
		if(strlen(str1)<strlen(search_str)){
			curline++;curloc=0;}
                else{
		const char* str2=str1+curloc;
		const char* str3=str1;
		char* ret;

			
			ret=strstr(str2,search_str);

		if(ret!=NULL){
			while(str3!=ret){
				++ct;
				++str3;
			}
			return (location){curline,ct};		//STRING FOUND
		}

		if(ret==NULL){
		curline++;
		curloc=0;
		}
		}

	}
docsiz=loc.line_no;
curline=1;
curloc=0;
--i;
}
return (location){0, 0};			//STRING NOT FOUND
}

void handle_merge_line(document *document, size_t line_no) {
  
const char* str1=document_get_line(document,line_no);
const char* str2=document_get_line(document,line_no+1);
char* str3=malloc(strlen(str1)+strlen(str2)+10);
strcpy(str3,str1);
strcat(str3,str2);
handle_delete_line(document,line_no+1);
document_set_line(document,line_no,str3);
free(str3);







//handle_append_command(document,line_no,str1);
//handle_delete_line(document,line_no+1);
}

void handle_split_line(document *document, location loc) {
    const char* str1=document_get_line(document,loc.line_no);
    assert(str1!=NULL);
    char* str2=strdup(str1);
    char* str3=malloc(strlen(str2)+10);
    strcpy(str3,str2);
    char* remfree1=str2;
    char* remfree2=str3;

   	str2=str2+loc.idx;
	*str2='\0';
	document_set_line(document,loc.line_no,remfree1);
	free(remfree1);

		str3=str3+loc.idx;
		++loc.line_no;
                //printf("%s\n",str3);
		
		size_t siz=document_size(document);
		if(siz>=loc.line_no){
			const char* str=document_get_line(document,siz);
			document_insert_line(document,siz+1,str);
                
                        while(siz!=loc.line_no){
				--siz;
                                str=document_get_line(document,siz);
                                document_set_line(document,siz+1,str);
                         }
			
 		
		document_set_line(document,loc.line_no,str3);	
		
		}
		else{

			document_insert_line(document,loc.line_no,str3);

		}
free(remfree2);

}

void handle_save_command(document *document, const char *filename) {
    
	document_write_to_file(document,filename);
}