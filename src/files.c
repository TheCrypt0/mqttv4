#include "files.h"

int findFile(char *dirname, char *dir, char *filestart)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return -2;
    }

    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".", entry->d_name) == 0 ||
                strcmp("..", entry->d_name) == 0)
                continue;
        } else {
            if (strncmp(entry->d_name, filestart, strlen(filestart)) == 0) {
                strcpy(dirname, entry->d_name);
                chdir("..");
                closedir(dp);

                return 0;
            }
        }
    }
    chdir("..");
    closedir(dp);

    return -1;
}

int getMp4Files(char *output, int limit, time_t startTime, time_t endTime)
{
    time_t rawtime;
    struct tm *timeinfo;
    char sDir[15];
    char sFile[13];
    char sFilename[1024];
    char s8601date[64];
    int num = 0;
    int fileFound = 0;

    chdir(RECORD_PATH);

    sprintf(output + strlen(output), "{\n\"start\":");

    timeinfo=localtime(&startTime);
    if (strftime(s8601date, sizeof(s8601date), "%FT%T%z", timeinfo) == 0) {
        fprintf(stderr, "strftime returned 0");
        return -1;
    }
    sprintf(output + strlen(output), "\"%s\",\n\"end\":", s8601date);

    timeinfo=localtime(&endTime);
    if (strftime(s8601date, sizeof(s8601date), "%FT%T%z", timeinfo) == 0) {
        fprintf(stderr, "strftime returned 0");
        return -1;
    }
    sprintf(output + strlen(output), "\"%s\",\n\"files\":[ ", s8601date);

    timeinfo=localtime(&endTime);
    sprintf(sDir, "%dY%02dM%02dD%02dH", timeinfo->tm_year + 1900,
        timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour);
    sprintf(sFile, "%02dM", timeinfo->tm_min);

    while (findFile(sFilename, sDir, sFile) == 0) {
        fileFound = 1;
        sprintf(output + strlen(output), "\"%s/%s\", ", sDir, sFilename);

        num++;
        if (num >= limit) break;

        rawtime=mktime(timeinfo);
        rawtime-=60;
        timeinfo = localtime(&rawtime);

        if (rawtime < (startTime - 60)) break;

        sprintf(sDir, "%dY%02dM%02dD%02dH", timeinfo->tm_year + 1900,
            timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour);
        sprintf(sFile, "%02dM", timeinfo->tm_min);
    }

    if (fileFound) {
        output[strlen(output)-2] = ' ';
        output[strlen(output)-1] = '\0';
    }
    sprintf(output + strlen(output), "]\n}\n");

    return 0;
}
