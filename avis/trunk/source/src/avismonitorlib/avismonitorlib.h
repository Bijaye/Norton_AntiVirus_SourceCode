#ifdef __cplusplus
extern "C" {
#endif

unsigned long AVISMonitorInit();
unsigned long AVISMonitorTerm();
unsigned long AVISMonitorSignal();

#define AVIS_MONITOR_ERROR      0
#define AVIS_MONITOR_ACTIVE     1
#define AVIS_MONITOR_INACTIVE   2
#define AVIS_MONITOR_UNKNOWN    3
int AVISMonitorCheck(char *filePath, long oldValue, long *newValue);

#ifdef __cplusplus
}
#endif
