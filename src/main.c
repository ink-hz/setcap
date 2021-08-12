#include <stdio.h>
#include <unistd.h>

#include "setcap.h"

#define DEMO_CAP_CONF "/etc/setcap/demo_cap.conf"

int main() {
	int ret = 0;
	
	ret = set_process_capability(DEMO_CAP_CONF); 
	if (ret != SET_CAP_SUCCESS) {
		fprintf(stderr, "set demo process capability failed, ret = %d\n", ret);
		return -1;
	}
	
	daemon(0,0);

	while (1) {
		printf("hello world\n");
		sleep(10);
	}

	return 0;
}
