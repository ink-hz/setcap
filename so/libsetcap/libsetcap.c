#include <sys/capability.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "setcap.h"


#define MAX_USER_LEN (128)
#define MAX_CAP_STR_LEN (1024)


char *trim(char *str)
{
    char *start = str;
    char *end = str + strlen(str);

    while(*start && isspace(*start))
        start++;

    while(end > start && isspace(*(end - 1)))
        end--;

    *end = '\0';
    return start;
}

int parse_line(char *line, char **key, char **value)
{
    char *ptr = strchr(line, '=');
    if (ptr == NULL)
        return -1;

    *ptr++ = '\0';
    *key = trim(line);
    *value = trim(ptr);

    return 0;
}


int get_capability_from_file(const char* file_name, int* p_enable, char* user, char* capabilities) {

	FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *key, *value;

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if (parse_line(line, &key, &value))
            continue;

        if (strcmp(key, "enable") == 0) {
			*p_enable = strtol(value, NULL, 10);
            printf("enable is %d\n", *p_enable);
        } else if (strcmp(key, "user") == 0) {
			strncpy(user, value, MAX_USER_LEN);
            printf("user is %s\n", user);
        } else if (strcmp(key, "capabilities") == 0) {
			strncpy(capabilities, value, MAX_CAP_STR_LEN);
            printf("capabilities is %s\n", capabilities);
        }

    }

    free(line);
    fclose(fp);
	
	return 0;
}

int set_uid(const char* name) {
	if (name == NULL) {
		return -1;
	}

	struct passwd* pwd = getpwnam(name);
	if (!pwd) {
		return -1;
	}
	
	if (setuid(pwd->pw_uid) == -1) {
    	fprintf (stderr, "set uid failed.\n");
		return -1;
    }
	
	return 0;
}

int set_cap(char* capabilities) {
	if (capabilities == NULL || capabilities[0] == '\0') {
		return -1;
	}

	int ret = 0;
	int ncap = 0;
	char* tmp;
	cap_value_t caps_list[CAP_LAST_CAP] = {0};

	tmp = strtok(capabilities, ",");

	do {
		if (ncap > CAP_LAST_CAP) {
			return -1;
		}

		if (cap_from_name(tmp, &caps_list[ncap]) == -1) {
			return -1;
		}
		ncap++;	

		tmp = strtok(NULL, ",");
	} while (tmp != NULL);


	cap_t caps = cap_init();
	if (caps == NULL) {
		return -1;
	}

	ret = cap_set_flag(caps, CAP_EFFECTIVE, ncap, caps_list, CAP_SET);
	if (ret == -1) {
		goto end;
	}

	ret = cap_set_flag(caps, CAP_INHERITABLE, ncap, caps_list, CAP_SET);
	if (ret == -1) {
		goto end;
	}

	ret = cap_set_flag(caps, CAP_PERMITTED, ncap, caps_list, CAP_SET);
	if (ret == -1) {
		goto end;
	}

	ret = cap_set_proc(caps);
	if (ret == -1) {
		goto end;
	}

end:
	cap_free(caps);

	return 0;
}



int set_process_capability(const char* file_name) {
	int enable = 0;
	char user[MAX_USER_LEN] = {0};
	char capabilities[MAX_CAP_STR_LEN] = {0};
	
	if (get_capability_from_file(file_name, &enable, user, capabilities) == -1) {
		return SET_CAP_READ_FILE_FAILED;
	}
	
	if (enable == 0) {
		return SET_CAP_SUCCESS;
	}

	if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) == -1) {
		return SET_CAP_SET_KEEPCAPS_FAILED;
	}

	if (set_uid(user) == -1) {
		return SET_CAP_SET_UID_FAILED;
	}

	if (set_cap(capabilities) == -1) {
		return SET_CAP_SET_CAPABILITIES_FAILED;
	}

	return SET_CAP_SUCCESS;
}

