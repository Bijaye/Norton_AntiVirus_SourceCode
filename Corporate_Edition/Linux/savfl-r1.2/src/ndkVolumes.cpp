#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <linux/posix_types.h>
#undef dev_t
#define dev_t __kernel_dev_t
#include <linux/loop.h>
#undef dev_t

typedef struct DRIVE_TABLE {
	char *name;
	char *path;
	char *mount_point;
	char *fs_type;
	char *mode;
	short int number;
	unsigned long blocks;
	struct DRIVE_TABLE *next;
	struct DRIVE_TABLE **parent;
	int pCount;
} VolumeTBL;

VolumeTBL *head;

char *GetNextLine(int fd)
{
	char ch[1];
	char *line = NULL;
	int length = 1;

	line = (char *) malloc(length);
	strcpy(line,"");	
	while (read(fd, ch, 1) > 0) {
		if ((ch[0] == '\n') || (ch[0] == '\0')) {
			break;
		}
		line = (char *) realloc(line, ++length);
		strncat(line, &ch[0], 1);
	}
	if (length==1) {
		return NULL;
	}
	else {
		return line;
	}
}

int DumpParts()
{
	int fd;
	VolumeTBL *tail;
	char *device;
	int major, minor, blocks;
	char *line = NULL;
		
	device = (char *) malloc(80);
	memset(device, 0, 80);
	tail = (VolumeTBL *) malloc(sizeof(VolumeTBL));

	tail = head;

	if ((fd = open("/proc/partitions", O_RDONLY)) == -1) {
		printf("error: DumpParts\n");
		return EXIT_FAILURE;
	} 
	GetNextLine(fd);
	GetNextLine(fd);
	while ((line = GetNextLine(fd)) != NULL) {
		sscanf(line, "%d %d %d %79s", &major, &minor, &blocks, device);
		tail->name = (char *) malloc(strlen(device)+1);
		strcpy(tail->name, device);
		tail->number = (major << 8) + minor;
		tail->blocks = blocks;

		tail->next = (VolumeTBL *) malloc(sizeof(VolumeTBL));
		memset(tail->next, 0, sizeof(VolumeTBL));
		tail = tail->next;
	}
	tail->next = NULL;
	close(fd);
	return EXIT_SUCCESS;
}

int DumpFilesystems()
{
	char *line;
	char *fs_type;
	int fd;

	fs_type = (char *) malloc(21);
	if ((fd = open("/proc/filesystems", O_RDONLY)) == -1) {
		printf("error: DumpFilesystems\n");
		return EXIT_FAILURE;
	} 

	while ((line = GetNextLine(fd)) != NULL) {
		memset(fs_type, 0, 21);
		sscanf(line,"nodev %20s", fs_type);
		if (strlen(fs_type) > 0) {
			VolumeTBL *newVol;

			newVol = (VolumeTBL *) malloc(sizeof(VolumeTBL));
			newVol->name = (char *) malloc(strlen(fs_type)+1);
			strcpy(newVol->name, fs_type);
			newVol->number = 0;
			newVol->blocks = 0;

			newVol->next = (VolumeTBL *) malloc(sizeof(VolumeTBL));
			memset(newVol->next, 0, sizeof(VolumeTBL));
			*newVol->next = *head;
			*head = *newVol;
		}
	}
	close(fd);
	return EXIT_SUCCESS;
}

VolumeTBL *GetVolNumber(short int number)
{
	VolumeTBL *vol;
	vol = (VolumeTBL *) malloc(sizeof(VolumeTBL));
	vol = head; 

	while(vol->next != NULL) {
		if (vol->number == number) {
			return vol;
		}
		vol = vol->next;
	}
	return NULL;
}

VolumeTBL *GetVolName(const char *name)
{
	VolumeTBL *vol;
	vol = (VolumeTBL *) malloc(sizeof(VolumeTBL));
	vol = head; 

	while(vol->next != NULL) {
		if (strcmp(vol->name, name) == 0) {
			return vol;
		}
		vol = vol->next;
	}
	return NULL;
}

int DumpMounts()
{
	int fd = 0;
	char *path, *mount_point, *fs_type;
	VolumeTBL *tempVol;
	char *line = NULL;
	struct stat statbuf;
	
	tempVol = (VolumeTBL *) malloc(sizeof(VolumeTBL));
	path = (char *) malloc(80);
	mount_point = (char *) malloc(80);
	fs_type = (char *) malloc(80);

	if ((fd = open("/proc/mounts", O_RDONLY)) == -1) {
		printf("error: DumpMounts1\n");
		return EXIT_FAILURE;
	} 

	while ((line = GetNextLine(fd)) != NULL) {
		sscanf(line, "%79s %79s %79s ", path, mount_point, fs_type);
		stat(mount_point, &statbuf);

		if ((tempVol = GetVolNumber(statbuf.st_dev)) != NULL) {
			tempVol->path = (char *) malloc(strlen(path)+1);
			strcpy(tempVol->path, path);
			tempVol->mount_point = (char *) malloc(strlen(mount_point)+1);
			strcpy(tempVol->mount_point, mount_point);
			tempVol->fs_type = (char *) malloc(strlen(fs_type)+1);
			strcpy(tempVol->fs_type, fs_type);
			continue;
		}
		if ((tempVol = GetVolName(fs_type)) != NULL) {
			tempVol->path = (char *) malloc(6);
			strcpy(tempVol->path, "nodev");
			tempVol->mode = (char *) malloc(3);
			strcpy(tempVol->mode, "sp"); 
			tempVol->mount_point = (char *) malloc(strlen(mount_point)+1);
			strcpy(tempVol->mount_point, mount_point);
			tempVol->fs_type = (char *) malloc(strlen(fs_type)+1);
			strcpy(tempVol->fs_type, fs_type);
			continue;
		}
		if ((statbuf.st_dev >> 8) == 7) {
			int fd;
			struct loop_info loopinfo;

			tempVol = (VolumeTBL *) malloc(sizeof(VolumeTBL));

    		tempVol->path = (char *) malloc(strlen(path)+1);
			strcpy(tempVol->path, path);
			tempVol->mount_point = (char *) malloc(strlen(mount_point)+1);
			strcpy(tempVol->mount_point, mount_point);
			tempVol->fs_type = (char *) malloc(strlen(fs_type)+1);
			strcpy(tempVol->fs_type, fs_type);
			tempVol->number = statbuf.st_dev;

			if ((fd = open(path, O_RDONLY)) == -1) {
				printf("error: DumpMounts2. Couldnt open loop device %s \n",path);
				perror("");
			}
			if (ioctl(fd, LOOP_GET_STATUS, &loopinfo) != -1) {
				tempVol->name = (char *) malloc(strlen(loopinfo.lo_name)+1);
				strcpy(tempVol->name, loopinfo.lo_name);
				stat(loopinfo.lo_name, &statbuf);
				tempVol->blocks = statbuf.st_size/1024;
				tempVol->next = (VolumeTBL *) malloc(sizeof(VolumeTBL));
				*tempVol->next = *head;
				*head = *tempVol;
			}
			close(fd);
			continue;
		}
		printf("error: DumpMounts3. couldnt find device [%s] in the /proc/partitions\n",path);
	}
	close(fd);
	return EXIT_SUCCESS;
}


int DumpMDstat(VolumeTBL *vol)
{
	char *line = NULL;
	char *drives = NULL;
	char *onedrive = NULL;
	int fd;
	VolumeTBL *parent;

	parent = (VolumeTBL *) malloc(sizeof(VolumeTBL));

	if ((fd = open("/proc/mdstat", O_RDONLY)) == -1) {
		printf("error: DumpMDstat\n");
		return EXIT_FAILURE;
	} 

	while ((line = GetNextLine(fd)) != NULL) {
		int tail_length = 0;
		int full_length = 0;

		if (strncmp(vol->name, line, strlen(vol->name)) != 0) {
			continue;
		}

		drives = (char *)realloc(drives, strlen(line));
		drives = strstr(line, " : active ");
		drives += 10;

		full_length = strlen(drives);
		onedrive = (char *)realloc(onedrive, full_length);
		while ((onedrive = strrchr(drives, ' ')) != NULL ) {
			VolumeTBL *parent_partition = NULL;

			tail_length = strlen(onedrive);
			drives[full_length - tail_length] = '\0'; 
			full_length = strlen(drives);

			onedrive++;
			onedrive[strlen(strstr(onedrive, "["))+1] = '\0';
			parent_partition = GetVolName(onedrive);	
			vol->pCount++;
			vol->parent = (DRIVE_TABLE **)realloc(vol->parent, vol->pCount);
			vol->parent[vol->pCount-1] = parent_partition;
		}
	}
	return EXIT_FAILURE;
}

int GetVolumeParents()
{
	struct stat statbuf;
	VolumeTBL *vol, *parents, *tempVol;
	int i,j;

	vol = head;	
	while(vol->next != NULL) {
		vol->pCount = 0;
		vol->parent = NULL;
		switch (vol->number >> 8) {
			case 7:
				stat(vol->name, &statbuf);
				parents = GetVolNumber(statbuf.st_dev);
				vol->pCount++;
				vol->parent = (DRIVE_TABLE **)realloc(vol->parent, vol->pCount);
				vol->parent[vol->pCount - 1] = parents;
				break;
			case 9: {
				DumpMDstat(vol);
				break;
			}
			case 58:
				break;
			default: {
				char parent_name[strlen(vol->name)+1];
				int i=0;
				strcpy(parent_name, vol->name);
				for (i = strlen(vol->name)-1; i > 0; i++) {
					if (isdigit(parent_name[i])) {
						parent_name[i] = '\0';
					}
					else {
						break;
					}
				}
				if ((parents = GetVolName(parent_name)) != NULL) {
					vol->pCount++;
					vol->parent = (DRIVE_TABLE **)realloc(vol->parent, vol->pCount*sizeof(VolumeTBL *));
					vol->parent[vol->pCount - 1] = parents;
				}
			}
		}
		vol = vol->next;
	}

	vol = head;
	while(vol->next != NULL) {
		i = 0;
		while (i < vol->pCount) {
			for(j = vol->parent[i]->pCount - 1; j > 0; j-- ) {
				vol->pCount++;
				tempVol = (VolumeTBL *) malloc(sizeof(VolumeTBL *));
				vol->parent = (DRIVE_TABLE **)realloc((VolumeTBL *)vol->parent, vol->pCount*sizeof(VolumeTBL));
				tempVol = vol->parent[i]->parent[j];
				vol->parent[vol->pCount - 1] = tempVol;
			}
			i++;			
		}
		vol = vol->next;
	}

	return EXIT_SUCCESS;
}

VolumeTBL *MountedVolumes()
{
	VolumeTBL *allVols, *vol, *tempVol;

	allVols = (VolumeTBL *) malloc(sizeof(VolumeTBL)); 
	vol = (VolumeTBL *) malloc(sizeof(VolumeTBL)); 
	memset(vol, 0, sizeof(VolumeTBL));
	tempVol = (VolumeTBL *) malloc(sizeof(VolumeTBL)); 

	allVols = head;

	while(allVols->next != NULL) {
		if (allVols->mount_point != NULL) {
			*tempVol = *allVols;
			tempVol->next = (VolumeTBL *) malloc(sizeof(VolumeTBL)); 
			memset(tempVol->next, 0, sizeof(VolumeTBL));
			*tempVol->next = *vol;
			*vol = *tempVol;
		}		
		allVols = allVols->next;
	}
	return vol;
}

VolumeTBL *GetNotUsedVols()
{
	VolumeTBL *vol, *mounted_part, *unused_part, *tempVol, *temp2Vol;
	int found = 0;

	vol = (VolumeTBL *) malloc(sizeof(VolumeTBL)); 
	mounted_part = (VolumeTBL *) malloc(sizeof(VolumeTBL));
	unused_part = (VolumeTBL *) malloc(sizeof(VolumeTBL));

	mounted_part = MountedVolumes();

	tempVol = head;
	while(tempVol->next != NULL) {
		found = 0;
		temp2Vol = mounted_part;
		while(temp2Vol->next != NULL) {
			int i=0;
			if (strcmp(tempVol->name, temp2Vol->name) == 0) {
				found = 1;
				break;
			}
			for (i = temp2Vol->pCount; i > 0; i--) {
				vol = temp2Vol->parent[i-1];
				if (strcmp(tempVol->name, vol->name) == 0) {
					found = 1;
					break;
				}
			}

			if (found == 1) {
				break;
			}
			temp2Vol = temp2Vol->next;
		}
		if (found == 0) {
			*vol = *tempVol;
			vol->next = (VolumeTBL *) malloc(sizeof(VolumeTBL));
			*vol->next = *unused_part;
			*unused_part = *vol;
		}
		tempVol = tempVol->next;
	}
	return unused_part;
}
 
int ndkVolumes_Whatever()
{

    VolumeTBL *vol, *tmp;
	head = (VolumeTBL *) malloc(sizeof(VolumeTBL));
	memset(head, 0, sizeof(VolumeTBL));
	vol = (VolumeTBL *) malloc(sizeof(VolumeTBL));
	memset(vol, 0, sizeof(VolumeTBL));
	vol = head;

	DumpParts();
	DumpFilesystems();
	DumpMounts();
	GetVolumeParents();

	printf("list all:\n");
    vol = head;
	while(vol->next != NULL) {
		printf("name: %s",vol->name);
		printf(" , path: %s ",vol->path);
		printf(" , type: %s",vol->fs_type);
		printf(" , mounted on: %s",vol->mount_point);
		printf(" , size: %ld\n",vol->blocks);
		vol = vol->next;
	}

	vol = MountedVolumes();
    printf("----------------------------\n");
	printf("mounts:\n");
	while(vol->next != NULL) {
        if (vol->pCount == 1) {
            tmp = vol->parent[0];
            printf("[%s] ",tmp->name);
		printf("name: %s",vol->name);
		printf(" , type: %s ",vol->fs_type);
		printf(" , mounted on: %s",vol->mount_point);
		printf(" , size: %ld\n",vol->blocks);
		}
		vol = vol->next;
	}

	vol = GetNotUsedVols();
    printf("----------------------------\n");
	printf("not used:\n");
	while(vol->next != NULL) {
        if (vol->pCount == 1) {
            tmp = vol->parent[0];
            printf("[%s] ",tmp->name);
		printf("name: %s",vol->name);
		printf(" , type: %s ",vol->fs_type);
		printf(" , mounted on: %s",vol->mount_point);
		printf(" , size: %ld\n",vol->blocks);
		} 
		vol = vol->next;
	}

	return EXIT_SUCCESS;
}



