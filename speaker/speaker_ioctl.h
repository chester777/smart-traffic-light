/***** ioctl *****/
#define IOCTL_START_NUM	0x80
#define IOCTL_NUM1	IOCTL_START_NUM+1
#define IOCTL_MAGIC_NUM 'z'
#define SPEAKER_IOCTL1 _IOWR(IOCTL_MAGIC_NUM,IOCTL_NUM1,unsigned long*)
