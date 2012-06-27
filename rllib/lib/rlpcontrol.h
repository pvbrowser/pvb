/***************************************************************************
                          rlspawn.h  -  description
                             -------------------
    begin                : Wed Dec 11 2002
    copyright            : (C) 2002 by R. Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#ifndef _RL_PCONTROL_H_
#define _RL_PCONTROL_H_

#include "rltime.h"

/*! <pre>
A class for starting/stoping/monitoring other processes.
</pre> */
class rlPcontrol
{
  public:
    /*! <pre>
    construct a process control object
    </pre> */
    rlPcontrol();

    /*! <pre>
    deconstruct the process control object
    </pre> */
    virtual ~rlPcontrol();

    /*! <pre>
    set the startup command and the process_name
    </pre> */
    void          setStartupCommand(const char *command, const char *process_name);

    /*! <pre>
    start the process
    </pre> */
    int           start();

    /*! <pre>
    stop the process with SIGTERM
    </pre> */
    int           sigterm();

    /*! <pre>
    stop the process with SIGKILL
    </pre> */
    int           sigkill();

    /*! <pre>
    test if process is alive
    </pre> */
    int           isAlive();

    /*! <pre>
    get startup command
    </pre> */
    const char    *startupCommand();

    /*! <pre>
    get process name
    </pre> */
    const char    *processName();

    /*! <pre>
    get process time start/stop
    </pre> */
    rlTime        *processTime();

    /*! <pre>
    set process id
    </pre> */
    void           setPID(long pid);

    /*! <pre>
    get process id
    </pre> */
    long           pid();

    /*! <pre>
    get next object in list
    </pre> */
    rlPcontrol    *getNext();

    /*! <pre>
    add and return a new rlPcontrol object
    </pre> */
    rlPcontrol    *addNew();
#ifdef __VMS
    /*! <pre>
    set input filename or logical
    </pre> */
    void          setInput (const char *input);

    /*! <pre>
    set output filename or logical
    </pre> */
    void          setOutput(const char *output);

    /*! <pre>
    set error filename or logical
    </pre> */
    void          setError (const char *error);
#endif

    /*! <pre>
    set priority default=8
    </pre> */
    void          setPriority(int priority);

    /*! <pre>
    get priority
    </pre> */
    int           priority();
  
  private:
    int rlstrlen(const char *str);
    char *startup_command, *process_name;

#ifdef __VMS
    char *m_input, *m_output, *m_error;
#endif

#ifdef RLWIN32
    long m_dwProcessId;
#endif

    long m_pid;
    rlTime process_time;
    rlPcontrol *next;
    int prio;
};

#endif
