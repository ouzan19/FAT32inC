#include <stdio.h>
#include <stdlib.h>

/**
 * holds the need parts, which is parsed, of an entry
 * */
typedef struct {
	char* name;
	int type;
	int address;
	int size;
	unsigned char date1;
	unsigned char date2;
	unsigned char time1;
	unsigned char time2;
	unsigned char cdate1;
	unsigned char cdate2;
	unsigned char ctime1;
	unsigned char ctime2;
	unsigned char longName;
} entryInfo;

    /**
     * Boot Sector variables
     * 
     * */
	int bytesPerSector;
	int sectorsPerCluster;
	int reservedSectors;
	int numberOfFats;	
	int numberOfSectors;
	int sectorsPerFat;
	int bigNumberOfSectors;
	int bigSectorsPerFat;
	int rootDirectoryStart;
	int rootEntriesStart;
    int currentDirectory;	
	char *currentDirectoryName;
	FILE *f;     // global file descriptor
	
 /**
  * performs exponential expression  (alt^ust)
  * 
  * @param alt : base of the expression
  * @param ust : exponent of the expression
  * */
int expo(int alt,int ust){
     int result=1,i=0;
	for(i=0;i<ust;i++){
		result= alt*result;
	}
return result;
}

/**
 * opens the file whose name is given
 * */
void openFile(char* fileName){
	
	f = fopen(fileName, "rt");
	}
	
/**
 * helper for reading
 * */
size_t fpread(unsigned char *buffer, size_t size, size_t nitems, size_t offset, FILE *fp){

     if (fseek(fp, offset, SEEK_SET) != 0)
         return 0;
     return fread(buffer, size, nitems, fp);
}

/**
 * reads given size byte from binary file starting from given offset
 * 
 * @param offset: starting point of reading
 * @param size  : number of bytes that will be read
 * 
 * */
unsigned char* read(size_t offset,int size){

	  unsigned char* result = malloc(sizeof(char)*size);	
      fpread(result,1,size,offset,f);
      
			return result;
}

/**
 * converts from hex to decimal (little endian)
 * @param buffer: integer array from little to big
 * @param size :  size of the array
 * 
 * */
unsigned int convertToInt(unsigned char* buffer,int size){

	unsigned int result=0,i=0;
	for(i=0;i<size;i++){
		
		result +=buffer[i]*expo(256,i);
	}
return result;
}

/**
*converts uppercase letters to the lowercase
*
*@param str: string to be changed
*@param size: size of str
*/
char* lower(char* str){
	int t=0;
	while(str[t]){
		
		if(str[t]>64 && str[t]<91)
			str[t]= str[t]+32;
		t++;
	}
	
	return str;
}

/**
*reads the needed number from the boot sector of FAT32 image file
*
*/
void readBootSector(){
	bytesPerSector = convertToInt(read(11,2),2);
	sectorsPerCluster = convertToInt(read(13,1),1);
    reservedSectors = convertToInt(read(0x0E,2),2);
	numberOfFats = convertToInt(read(0x10,1),1);
	bigNumberOfSectors = convertToInt(read(0x20,4),4);
	bigSectorsPerFat = convertToInt(read(0x24,4),4);
	rootDirectoryStart = convertToInt(read(0x2C,4),4);
}

/**
 * prints the boot sector variables
 * 
 * */
void showSystemInfo(){
	printf("%d\n",bytesPerSector);
	printf("%d\n",sectorsPerCluster);
    printf("%d\n",reservedSectors);
	printf("%d\n",numberOfFats);	
	printf("%d\n",bigSectorsPerFat);
	printf("%d\n",bigNumberOfSectors);

}

/***
 *parse the entry whose position is given and returns as a entryInfo struct
 * @param current: start position of the desired entry 
 * 
 */
char* lower2(char* xxx){
	int i=0;
	char* result= (char*) malloc(12);
	for(i=0;i<11;i++){
			if(xxx[i]>64 && xxx[i]<91)
					result[i]= xxx[i]+32;
				else    result[i] =xxx[i];
	

	}
	result[12]=0;
return result;
}

/***
 * reads the long name of the file whose actual entry's position is given
 * 
 * */
char* readLongName(position){
		char* longName = (char*) malloc(sizeof(char));
		int size=0;
		position -=32;
		while( convertToInt(read(position+11,1),1) == 15 ){		// until attribute is not 0x0F
				char* temp=read(position+1,10);
				int i=0;
				for(i=0;i<10;i++){
					if(temp[i] != -1 && temp[i]){
						
						longName[size] = temp[i];
						size++;
						longName = realloc(longName,size+1);
						}
				}
				temp=read(position+0x0E,12);
				i=0;
				for(i=0;i<12;i++){
					if(temp[i] != -1 && temp[i]){
						
						longName[size] = temp[i];
						size++;
						longName = realloc(longName,size+1);
						}
				}	
				temp=read(position+0x1C,4);
				i=0;
				for(i=0;i<4;i++){
					if(temp[i] != -1 && temp[i]){
						
						longName[size] = temp[i];
						size++;
						longName = realloc(longName,size+1);
						}
				}
			position-=32;
		}
	return longName;
	
	}
	
/**
 * parses the file entry whose position is given
 * 
 * @param current : starting position of the entry
 * */
entryInfo* getEntryInfo(int current){

	entryInfo *entry =(entryInfo*) malloc(sizeof(entryInfo));
	
	entry->name = lower2(read(current,11));
	current+=11;
	if(entry->name[6] == '~'){
		entry->name = readLongName(current-11);
		entry->longName =1;
	}
	else entry->longName=0;
	entry->type = convertToInt(read(current,1),1);
	current+=3;
	entry->ctime1 = convertToInt(read(current,1),1);
	current++;
	entry->ctime2 = convertToInt(read(current,1),1);
	current++;
	entry->cdate1=convertToInt(read(current,1),1);
	current++;
	entry->cdate2=convertToInt(read(current,1),1);
	current++;
	current+=2;
    int high = convertToInt(read(current,2),2);
    current+=2;
	entry->time1 = convertToInt(read(current,1),1);
	current++;
	entry->time2 = convertToInt(read(current,1),1);
	current++;
	entry->date1=convertToInt(read(current,1),1);
	current++;
	entry->date2=convertToInt(read(current,1),1);
	current++;
    int low = convertToInt(read(current,2),2);
	entry->address =sectorToPosition( high*256 + low);
    current+=2;
    entry->size =  convertToInt(read(current,4),4);
    return entry;
}

/** finds the next cluster given position in FAT
 * 
 **/
unsigned int getNextCluster(unsigned int position){
	int sector = positionToSector(position);
	int fatPosition = 32*512+4*sector;
	return convertToInt(read(fatPosition,4),4);
	
	}
	
/** finds the cluster chain given position in FAT until the EOC
 * 
 **/
unsigned int* getClusters(unsigned int position){
	
	unsigned int nextCluster=0,size=0;
	unsigned int* clusterChain = (int*) malloc(sizeof(int));
	while (  ( nextCluster = getNextCluster(position) ) != 0xFFFFFF8 && nextCluster !=0xFFFFFFF){
		clusterChain[size] = sectorToPosition(nextCluster);
		size++;
		clusterChain = realloc(clusterChain,size+1);
		position = sectorToPosition(nextCluster);
	}
	clusterChain[size] = 0;
	return clusterChain;
	}

/**
 * process the entries of the directory whose start position is given, prints the valid ones with their types ( d or -)
 * @param position: starting position of desired directory 
 * 
 * */
void ls(int position){
	
	unsigned int *chain =getClusters(position);
	int i=0,mode=1;
	char* temp,*bitvector;
	while(1){
		int it=0,son=position+512;
		if(rootEntriesStart != position && mode) // if not root and the first cluster, 
						position+=96;   		//skip . and ..
	
		while(position < son){
		entryInfo *tempEntry = getEntryInfo(position);
		bitvector=read(position+11,1);					//read attribute
		temp=lower(tempEntry->name);				//read name
		if(temp[0] && bitvector[0] && bitvector[0]!=0x0F && temp[0]!=-27) // if entry starts with -27, meaning that it is deleted
			{
				if(bitvector[0] == 16) printf("d ");	// if attribute is 16, then it is directory
				else printf("- ");
				if(!tempEntry->longName){
					for(it=0;it<8;it++)						// in 8.3 format firsdt print the name
						if(temp[it] != ' ')					// then print the extensin if exists
							printf("%c",temp[it]);
					if(temp[8]!=' '){
						printf(".");
						for(;it<11;it++)
							if(temp[it] != ' ')
								printf("%c",temp[it]);
					}
				}else{
						
							printf("%s",tempEntry->name);
					}
				printf("\n");
			}
			position+=32;								//next entry
		}
		mode=0;
		if(chain[i]) position = chain[i++];
		else break;
	}
}

/**
 * compares the two strings. one of them ends with '\0', other does not. 
 * @param str1: the string ends with '\0'. 
 * @param str2: the string does not end with '\0'
 * @param mode: whether one of the string has long name or not
 * */
int isEqual(char* str1,char*str2,int mode){
		if(mode) return !strcmp(str1,str2);
		int i2=0,i3=0;
		char * str3 = (char*) malloc(11);
		for(i2=0;i2<8;i2++)
			if(str2[i2] != ' ')
				str3[i3++]=str2[i2];
		if(str2[8] != ' '){
			str3[i3++] = '.';
			str3[i3++]= str2[8];
			str3[i3++]= str2[9];
			str3[i3++]= str2[10];
	}
	str3[i3]=0;
	return !strcmp(str1,str3);
}

/**
 * returns only the directories in the given directory.
 * @param position: starting position of desired directory 
 * */
entryInfo** getDirectories(int position){
	int it=0,size=0;
	char* temp,*bitvector;
	entryInfo **entry= (entryInfo**)malloc(sizeof(entryInfo*));
	while(1){
	bitvector=read(position+11,1);				//read attribute
	temp=lower2(read(position,11));			//read name
	if(!temp[0]) break;
	if(bitvector[0] && temp[0]!='a' && temp[0]!=-27)   // if attribute is zero or file is deleted, then skip
		{
		if(temp[8]==' '){								// meaning that it is a directory,not a file
			entry=realloc(entry,(size+2)*sizeof(entryInfo*));
			size++;
			entry[size-1] = getEntryInfo(position);	
		}

	}
	position+=32;			// next entry
	}
	entry[size]=NULL;		// put a NULL at the end to identify where the array is end
	return entry;
}


/**
 * finds the directory whose name is given in the directory whose position is given
 * @param d: the file name which searched for
 * @param position: starting position of the parent directory
 * */
entryInfo* findDirectory(char* d, int position){
	int it=0;
	entryInfo** entries = getDirectories(position);// get all directories
	while(entries[it]){
		if(isEqual(d,entries[it]->name,entries[it]->longName))			// search for desired one
			return entries[it];						//if found return it
		it++;
	}
	return NULL;									// else return NULL
}

/**
 * converts the given sector to the position in the image file
 * @param sector: sector number to be converted to the position
 * */
int sectorToPosition(int sector){
return reservedSectors*bytesPerSector+bigSectorsPerFat*bytesPerSector*2+(sector-2)*bytesPerSector;
}

/**
 * converts the given podition to the sector number in the image file
 * @param position: position to be converted to the sector number
 * */
int positionToSector(position){
	return (position-34*512-552*512*2)/512 +4;
	}

/**
 * searches for the position the parent of the entry whose starting position is given
 *  @param position: starting position of the desired entry
 * */
int findParent(int position){
	position+=32;							// parent entry (..)
	int parent = getEntryInfo(position)->address; // get its position
	if((parent+1024) == rootEntriesStart){		// if it is root,change the current directory's name as " ", return it
		int i=0;
		for(i=0;i<8;i++)
			currentDirectoryName[i] = ' ';
		return rootEntriesStart;
	}
	int grandParent= getEntryInfo(parent+32)->address ;		// find grand parent to get the parent's name
	if(rootEntriesStart-grandParent ==1024) grandParent+=1024;
	int it=0;
	while(1){
	entryInfo* temp = getEntryInfo(grandParent);			//get parent candidates's info
	if(temp->address == parent)								// if the name holds
		{
		currentDirectoryName = temp->name;					// change current directory's name and return it
		return parent;
		break;
		}
	grandParent+=32;										// next candidate
	}
}

/**
 * changes the current directory to the directory whose name is given
 * 	@param directory: target directory
 * */
void cd(char* directory){
	if(!strcmp(directory,".")) return;			// if ., nothing changes. return
	if(!strcmp(directory,"..")){				// if .., go to root
		if(currentDirectory == rootEntriesStart) return;
		currentDirectory = findParent(currentDirectory);
		return;
	}
	
	int it=0;
	entryInfo *ee;							
	if(!(ee=findDirectory(directory,currentDirectory))){	// find the  target directory
		printf("No such directory\n");						// if not exist, give an error
	}
	else{
	currentDirectory = ee->address;							//if exists, set it as current directory
	int it=0;
	while(it<8)
	currentDirectoryName[it] =ee->name[it++];				// update the current directory name also
	
	}
}

/***
 *	prints the current directory's name. NOT NEEDED. TEST PURPOSE FUNCTION 
 */
void printCurrentDirectory(){
	int it=0;
	if(currentDirectoryName[0] != ' '){
		while(it<8 && currentDirectoryName[it] != ' ')
			printf("%c",currentDirectoryName[it++]);
	}
	else printf("");

}

/**
 * finds the all parents up to the root. returns them as an array. root is at the end of the array as NULL.
 * @param position: position of desired place
 * */
char** whereAmI(int position){
	char** path =(char**) malloc(sizeof(char*));
	if(position == rootEntriesStart){	//if it is root, no parent. returns the array whose only member is NULL
		path[0] = NULL;
		return path;
	}
	int size=0;
	char* tempName=(char*) malloc(sizeof(char)*8);
	int it=0;
	while(it<8)							// since findParent() function changes the currentDirectoryName, to restore keep it
		tempName[it] = currentDirectoryName[it++];	
		
	while( position != rootEntriesStart ){	// until root, iterate
	        position=findParent(position);			// find parent 
		it=0;
		path[size]= (char*) malloc(sizeof(char)*8);	// put into the array
		while(it<8)
			path[size][it] = currentDirectoryName[it++];
		size++;
		path = realloc(path,(size+1)*sizeof(char*));
	}
	
	it=0;
	while(it<8)							// write back the current directory's name
		currentDirectoryName[it] = tempName[it++];
		
	path[size] = NULL;				// represents root
	return path;
}

/**
 * prints the path from the root
 * */
void pwd(int position){
	char** path = whereAmI(position);
	int it=0,size=-1;
	
	while(path[it++])
		size++;
	for(it=size-1;it>=0;it--){
			printf("/");
		int bb =0;
		while(bb<8 && path[it][bb]!=' ')
			printf("%c",path[it][bb++]);
	}
	printf("/");
	printCurrentDirectory();
	printf("\n");
}

/**
 * initialize the system variables
 * @param fileName: path of the FAT32 image file
 * */
void initialize(char* fileName){
	openFile(fileName);
	readBootSector();
	currentDirectoryName = (char*) malloc(sizeof(char)*8);
	currentDirectoryName[0] = ' ';
	currentDirectoryName[1] = ' ';
	currentDirectoryName[2] = ' ';
	currentDirectoryName[3] = ' ';
	currentDirectoryName[4] = ' ';
	currentDirectoryName[5] = ' ';
	currentDirectoryName[6] = ' ';
	currentDirectoryName[7] = ' ';
 	rootEntriesStart =sectorToPosition(rootDirectoryStart);
	currentDirectory=rootEntriesStart;
}

/**
 * prints the date and time of the given entry. 
 * @param e: entry to be printed
 * @param mode: if mode is 0 creation date and time will be printed, else modification date will be printed
 * */
void printDate(entryInfo *e,int mode){
	if(mode){
		
	char year = e->date2 >> 1 ;
	char month = (((e->date2)&1 )*8)+ ((e-> date1) >> 5 );
	char day = (e -> date1) & 31;
	printf("%d.",year+1980);
	if(month<10) printf("0");
	printf("%d.",month);
	if(day<10) printf("0");
	printf("%d ",day); 
        char hour =  e->time2 >> 3;
	char minute =  ((e->time2 & 7))*8 +  (e->time1 >>5);
        if(hour<10) printf("0");
	printf("%d:",hour);
	if(minute<10) printf("0");
	printf("%d\n",minute); 
	} 
	else {
		
	char year = e->cdate2 >> 1 ;
	char month = (((e->cdate2)&1 )*8)+ (e-> cdate1) >> 5;
	char day = (e -> cdate1) & 31;
	printf("%d.",year+1980);
	if(month<10) printf("0");
	printf("%d.",month);
	if(day<10) printf("0");
	printf("%d ",day); 
        char hour =  e->ctime2 >> 3;
	char minute =  ((e->ctime2 & 7))*8 +  (e->ctime1 >>5);
	 if(hour<10) printf("0");
	printf("%d:",hour);
	if(minute<10) printf("0");
	printf("%d\n",minute); 
	}
}

/**
 * searches for a file whose name is given in the current directory
 * @param fileName: name of the searched file
 * */
int findEntry(char *fileName){
	int position=currentDirectory;
	unsigned int *chain =getClusters(position);
	int i=0;
	while(1){
		int son=position+512;
		while( position<son ){	// itareate until entries end
			entryInfo* e = getEntryInfo(position);		// get info of the entry
			if(isEqual(fileName,e->name,e->longName)) return position;	// if the name holds, return the position
			position+=32;						// next entry
		}
		if(chain[i]) position = chain[i++];
		else break;
	}
	return -1;					// if not found,return -1 indicating an error
}

/**
 * prints the date and time of the file whose name is given. 
 * @param fileName: name of the file whose date to be printed
 * @param mode: if mode is 0 creation date and time will be printed, else modification date will be printed
 * */
void date(char* fileName,int mode){
	int position = findEntry(fileName);					// find entry
	if(position == -1) { printf("No such an entry\n");return;} //if not exist, print error
	printDate(getEntryInfo(position),mode);					// else print
	
}

/**
 * prints the size of the file whose name is given. 
 * @param fileName: name of the file whose size to be printed
 * */
void size(char* fileName){
	int position = findEntry(fileName);				//find entry
	if(position == -1) { printf("No such an entry\n");return;}	//if not exist, print error
	printf("%d\n",getEntryInfo(position)->size);	 // else print
}

/**
 * prints the content of the file whose name is given. 
 * @param fileName: name of the file whose contentto be printed
 * */
void cat(char* fileName){
	int position = findEntry(fileName);				//find entry
	if(position == -1) { printf("No such an entry\n");return;}//if not exist, print error
	entryInfo* e = getEntryInfo(position);  	  // else get the data position
	char* content = read(e->address,e->size);    //read and print
	int i=0;
	for(i=0;i<e->size;i++)
		printf("%c",content[i]);
	printf("\n");

}

int main(char argc, char** args){
	initialize(args[1]);
	char * command=(char*) malloc(sizeof(char)*8);
	char * dir=(char*) malloc(sizeof(char)*12);
	
	/** infinite loop until quit is typed*/
	while(1){
	char** path = whereAmI(currentDirectory);						// find the path from the root to print prompt
	if(path[1] && currentDirectory!= rootEntriesStart) printf("..."); 
	printf("/");
	printCurrentDirectory();
	printf(">");
			        
		scanf("%s",command);
		if(isEqual(command,"cd",0)){
		
			scanf("%s",dir);
			cd(lower(dir));
		}
		else if(isEqual(command,"quit",0)) break;
		else if(isEqual(command,"ls",0)) ls(currentDirectory);
		else if(isEqual(command,"pwd",0)) pwd(currentDirectory);
		else if(isEqual(command,"info",0)) showSystemInfo();
		else if(isEqual(command,"crdt",0)) {
			scanf("%s",dir);
			date(lower(dir),0);
		}
		else if(isEqual(command,"moddt",0)) {
			scanf("%s",dir);
			date(lower(dir),1);
		}
		else if(isEqual(command,"size",0)) {
			scanf("%s",dir);
			size(lower(dir));
		}
		else if(isEqual(command,"cat",0)) {
			scanf("%s",dir);
			cat(lower(dir));
		}
		else
			printf("No such command!\n");
	}
		return 5;
}
