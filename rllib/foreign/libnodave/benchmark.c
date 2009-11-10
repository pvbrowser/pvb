    void rBenchmark(daveConnection * dc) {
	int i,res,maxReadLen;
	double usec;
	PDU p;
#ifdef UNIX_STYLE    
	struct timeval t1, t2;
#endif    
#ifdef BCCWIN    
	clock_t t1, t2;
#endif
	maxReadLen=dc->maxPDUlength-18;
    	printf("Now going to do read benchmark with minimum block length of 1.\n");
	wait();
#ifdef UNIX_STYLE    
	gettimeofday(&t1, NULL);
#endif    
#ifdef BCCWIN    
	t1=clock();
#endif	    
	for (i=1;i<101;i++) {
    	    daveReadBytes(dc,daveFlags,0,0,1,NULL);
	    if (i%10==0) {
	        printf("...%d",i);
	        fflush(stdout);
	    }
	}	
#ifdef UNIX_STYLE        
	gettimeofday(&t2, NULL);
	usec = 1e6 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec;
	usec/=1e6;
#endif    
#ifdef BCCWIN    
        t2=clock();
        usec = 0.001*(t2 - t1);
#endif
        printf(" 100 reads took %g secs. \n",usec);
        printf("Now going to do read benchmark with shurely supported block length %d.\n",maxReadLen);
        wait();
#ifdef UNIX_STYLE    
        gettimeofday(&t1, NULL);
#endif    
#ifdef BCCWIN    
        t1=clock();
#endif	    
        for (i=1;i<101;i++) {
	    daveReadBytes(dc,daveFlags,0,0,maxReadLen,NULL);
	    if (i%10==0) {
	        printf("...%d",i);
	        fflush(stdout);
	    }
	}	
#ifdef UNIX_STYLE    
	gettimeofday(&t2, NULL);
	usec = 1e6 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec;
	usec/=1e6;
#endif    
#ifdef BCCWIN    
	t2=clock();
	usec = 0.001*(t2 - t1);
#endif
	printf(" 100 reads took %g secs. \n",usec);
    	wait();
	    
	printf("Now going to do read benchmark with 5 variables in a single request.\n");
	printf("running...\n");
#ifdef UNIX_STYLE    
	gettimeofday(&t1, NULL);
#endif    
#ifdef BCCWIN    
	t1=clock();
#endif	    
	for (i=1;i<101;i++) {
	    davePrepareReadRequest(dc, &p);
	    daveAddVarToReadRequest(&p,daveInputs,0,0,36);
	    daveAddVarToReadRequest(&p,daveFlags,0,0,36);
	    daveAddVarToReadRequest(&p,daveFlags,0,10,36);
	    daveAddVarToReadRequest(&p,daveFlags,0,20,36);
	    daveAddVarToReadRequest(&p,daveFlags,0,40,36);
	    res=daveExecReadRequest(dc, &p, NULL);
	    if (i%10==0) {
	        printf("...%d",i);
	        fflush(stdout);
	    }
	}	
#ifdef UNIX_STYLE    
	gettimeofday(&t2, NULL);
	usec = 1e6 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec;
	usec/=1e6;
#endif    
#ifdef BCCWIN    
	t2=clock();
	usec = 0.001*(t2 - t1);
#endif
	printf(" 100 reads took %g secs.\n",usec);
}

void wBenchmark(daveConnection * dc) {	    
    int i,c;
    double usec;
#ifdef UNIX_STYLE    
    struct timeval t1, t2;
#endif    
#ifdef BCCWIN    
    clock_t t1, t2;
#endif

    printf("Now going to do write benchmark with minimum block length of 1.\n");
    wait();
#ifdef UNIX_STYLE    
    gettimeofday(&t1, NULL);
#endif    
#ifdef BCCWIN    
    t1=clock();
#endif	    
    for (i=1;i<101;i++) {
        daveWriteBytes(dc,daveFlags,0,0,1,&c);
        if (i%10==0) {
	    printf("...%d",i);
	    fflush(stdout);
	}
    }
#ifdef UNIX_STYLE    
    gettimeofday(&t2, NULL);
    usec = 1e6 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec;
    usec/=1e6;
#endif    
#ifdef BCCWIN    
    t2=clock();
    usec = 0.001*(t2 - t1);
#endif
    printf(" 100 writes took %g secs. \n",usec);
	    
    printf("Now going to do write benchmark with shurely supported block length 200.\n");
    wait();
#ifdef UNIX_STYLE    
    gettimeofday(&t1, NULL);
#endif    
#ifdef BCCWIN    
    t1=clock();
#endif	    
    for (i=1;i<101;i++) {		
        daveWriteBytes(dc,daveFlags,0,0,200,&c);
        if (i%10==0) {
	    printf("...%d",i);
	    fflush(stdout);
	}
    }    
#ifdef UNIX_STYLE    
    gettimeofday(&t2, NULL);
    usec = 1e6 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec;
    usec/=1e6;
#endif    
#ifdef BCCWIN    
    t2=clock();
    usec = 0.001*(t2 - t1);
#endif		
    printf(" 100 writes took %g secs. \n",usec);
    wait();
}
