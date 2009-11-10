  if(pdu[17] != 0xff)
  {
    if(pdu[17] == 0x0a){
      fprintf( stderr, " > Error: Trying to access a DB that does not exist\n");
      fprintf( stderr, "          Please, check that DB is set.   (error code: 10 (0x0a))\n");
      return -(pdu[17])
    }
    else if(pdu[17] == 0x05){
      fprintf(stderr, " > Error: Trying to access an address that does not exist.\n");
      fprintf(stderr, "          Please, check the address range. (error code: 5 (0x05))\n");
      return -(pdu[17]);
    }
      else if(pdu[17] == 0x07){
      fprintf(stderr, " > Error: the write data size doesn't fit item size\n"); // NO TESTED!!!
      fprintf(stderr, "          Please, check the data size.     (error code: 7 (0x07))\n");
      return -(pdu[17]);
    }
    else{
      fprintf(stderr, " > Error: unknown error  (código %x!=0xff)\n", pdu[17]);
      return -(pdu[17]);
    }
  }
