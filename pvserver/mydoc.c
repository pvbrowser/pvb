#include <stdio.h>

int doc(FILE *fin, FILE *fout)
{
char line[1024],buf[80];
int html,literal,proto,descr,topic;

  fputs("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n",fout);
  fputs("<html>\n",fout);
  fputs("<head>\n",fout);
  fputs(" <META NAME=\"GENERATOR\" CONTENT=\"mydoc R. Lehrig\">\n",fout);
  //fputs(" <TITLE>ProcessViewServer Programming Reference</TITLE>\n",fout);
  fputs("</head>\n",fout);
  fputs("<body>\n",fout);

  topic = 1;
  html = literal = proto = descr = 0;
  while(fgets(line,sizeof(line)-1,fin) != NULL)
  {
    if(strncmp(line,"/*#HTML",7) == 0)
    {
      html = 1;
    }
    else if(strncmp(line,"/*#LITERAL",10) == 0)
    {
      fputs("<pre>\n",fout);
      literal = 1;
    }
    else if(strncmp(line,"/*#ENDLITERAL",13) == 0)
    {
      fputs("</pre>\n",fout);
      literal = 0;
    }
    else if(strncmp(line,"/*#PROTO",8) == 0)
    {
      proto = 1;
      sprintf(buf,"<h3> (%d) ",topic++);
      fputs(buf,fout);
    }
    else if(strncmp(line,"/*#DESCR",8) == 0)
    {
      if(proto)
      {
        if(proto) fputs("</h3>\n",fout);
        proto = 0;
      }
      fputs("<pre>\n",fout);
      descr = 1;
    }
    else if(strncmp(line,"**/",3) == 0 || strncmp(line,"*/",2) == 0)
    {
      if(proto) fputs("</h3>\n",fout);
      if(literal || descr)
      {
        fputs("</pre>\n",fout);
      }
      html = literal = proto = descr = 0;
    }
    else if(html || literal || proto || descr)
    {
      fputs(line,fout);
    }
  }
  fputs("</body>\n",fout);
  fputs("</html>\n",fout);
  return 0;
}

int main(int ac, char **av)
{
FILE *fin,*fout;

  if(ac != 3)
  {
    printf("usage: mydoc file.h doc.html\n");
    return 0;
  }
  fin = fopen(av[1],"r");
  if(fin == NULL)
  {
    printf("could not open %s",av[1]);
    return 0;
  }
  fout = fopen(av[2],"w");
  if(fout == NULL)
  {
    fclose(fin);
    printf("could not write %s",av[2]);
    return 0;
  }

  doc(fin,fout);
  return 0;
}

