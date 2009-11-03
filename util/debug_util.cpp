// debug_util.cpp

/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "stdafx.h"
#include "../db/cmdline.h"
#include "../db/jsobj.h"

namespace mongo {

#if defined(_DEBUG) && !defined(_WIN32)
    /* Magic gdb trampoline
     * Do not call directly! call setupSIGTRAPforGDB()
     * Assumptions:
     *  1) gdbserver is on your path
     *  2) You have run "handle SIGSTOP noprint" in gdb
     *  3) cmdLine.port + 2000 is free
     */
    void launchGDB(int){
        // Don't come back here
        signal(SIGTRAP, SIG_IGN);

        int newPort = cmdLine.port + 2000;
        string newPortStr = "localhost:" + BSONObjBuilder::numStr(newPort);
        string pidToDebug = BSONObjBuilder::numStr(getpid());

        cout << "\n\n\t**** Launching gdbserver on " << newPortStr << " ****" << endl << endl;
        if (fork() == 0){
            //child
            execlp("gdbserver", "gdbserver", "--attach", newPortStr.c_str(), pidToDebug.c_str(), NULL);
            perror(NULL);
        }else{
            //parent
            raise(SIGSTOP); // pause all threads until gdb connects and continues
            raise(SIGTRAP); // break inside gdbserver
        }
    }

    void setupSIGTRAPforGDB(){
        assert( signal(SIGTRAP , launchGDB ) != SIG_ERR );
    }
#else
    void setupSIGTRAPforGDB() {
    }
#endif
}
