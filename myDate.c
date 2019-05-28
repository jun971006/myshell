#include <time.h>
#include <stdio.h>
#include <string.h>

int main(){
	struct tm *tm;
	time_t t;
	char day[7][4] = {"일", "월", "화", "수", "목", "금", "토"};
	tzset();
	time(&t);

	tm = localtime(&t);

	if(tm->tm_mon >= 9){
		if(tm->tm_min < 10){
			printf("%d. %d. %d. (%s) %d:0%d:%d\n", tm->tm_year+1900, tm->tm_mon+1,
					tm->tm_mday, day[tm->tm_wday], tm->tm_hour, tm->tm_min,
					tm->tm_sec);
		}
		else{
			printf("%d. %d. %d. (%s) %d:%d:%d\n", tm->tm_year+1900, tm->tm_mon+1,
					tm->tm_mday, day[tm->tm_wday], tm->tm_hour, tm->tm_min,
					tm->tm_sec);
		}
	}
	else {

		if(tm->tm_min < 10){
			printf("%d. 0%d. %d. (%s) %d:0%d:%d %s\n", tm->tm_year+1900, tm->tm_mon+1,
					tm->tm_mday, day[tm->tm_wday], tm->tm_hour, tm->tm_min,
					tm->tm_sec, tzname[0]);
		}
		else{
			printf("%d. 0%d. %d. (%s) %d:%d:%d %s\n", tm->tm_year+1900, tm->tm_mon+1,
					tm->tm_mday, day[tm->tm_wday], tm->tm_hour, tm->tm_min,
					tm->tm_sec, tzname[0]);
		}
	}

}
