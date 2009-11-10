#ifndef log__
#define log__
#define LOG1(x) fprintf(stderr,x)
#define LOG2(x,y) fprintf(stderr,x,y)
#define LOG3(a,b,c) fprintf(stderr,a,b,c)
#define LOG4(a,b,c,d) fprintf(stderr,a,b,c,d)
#define LOG5(a,b,c,d,e) fprintf(stderr,a,b,c,d,e)
#define FLUSH fflush(stderr)

#define LOG_1(a) fprintf(stderr,a)
#define LOG_2(a,b) fprintf(stderr,a,b)
				
#endif
