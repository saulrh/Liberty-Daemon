#include "liberty.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <assert.h>
#include <ach.h>

extern "C" {
#include "daemonizer.h"
}

using namespace std;

int main()
{
    ach_channel_t chan;     
    int r;

	LIBERTY_STRUCT lbt;

    
    /************************
     * Daemonize 
     ***********************/

    daemonize("libertyd");


    /*************************
     * Open ach channel
     *************************/
    r = ach_open(&chan, "liberty", NULL);

    daemon_assert( ACH_OK == r, __LINE__ );
    cout <<"liberty channel open correctly" << endl;


    /***************************
     * Init and configure Liberty
     **************************/
    if (initLiberty(&lbt, NULL) < 0) {
        cout << "Initialize Liberty failed" << endl;
        exit(1);
    }

    configLiberty(&lbt, "U1");      // data in centimeter
    configLiberty(&lbt, "O*,2,7");  // XYZ and Quaternion
    configLiberty(&lbt, "F1");      // in binary 
    configLiberty(&lbt, "C");       // continuous reading

    // to make later read start from the beginning of a new record
    readInitLiberty(&lbt);


    /******************************************
     * Read entries from Liberty continuously
     *  Send 1 sensor data updated, all 8 sensors
     *  data on ach channel
     *******************************************/
    int ret;    // sensor number or error
    while ( (!daemon_sig_quit) && (ret = readEntryLiberty(&lbt)) > 0 ) {
//    while ( (ret = readEntryLiberty(&lbt)) > 0 ) {
        ach_put(&chan, lbt.sData, sizeof(lbt.sData));       
//        if (ret == 2)
//            printSensorLiberty(&lbt, ret);
        usleep(200);
    }


    daemon_close();

    return 0;
}
